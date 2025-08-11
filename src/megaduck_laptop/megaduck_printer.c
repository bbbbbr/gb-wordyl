#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#include <duck/laptop_io.h>
#include "megaduck_printer.h"

static bool print_blank_row(uint8_t printer_type);
static void prepare_tile_row(uint8_t row, uint8_t tile_bitplane_offset);
static void convert_tile(uint8_t * p_out_buf, uint8_t * p_tile_buf);
static void convert_tile_dithered(uint8_t * p_out_buf, uint8_t * p_tile_buf);
static bool duck_io_send_tile_row_1pass(void);

#define BYTES_PER_PRINTER_TILE  8u
#define BYTES_PER_VRAM_TILE     16u
#define TILE_HEIGHT             8u
#define TILE_WIDTH              8u

uint8_t tile_row_buffer[DEVICE_SCREEN_WIDTH * BYTES_PER_PRINTER_TILE];


// Expects duck_io_tx_buf to be pre-loaded with payload
//
// The System ROM uses an infinite retry which would block
// program execution forever if the printer failed. Instead
// 10x has been determined via trial and error as a reasonable
// number of retries.
static bool print_send_command_and_buffer_delay_1msec_10x_retry(uint8_t command) {
    
    uint8_t retry = 10u;
    while (retry--) {
        bool result = duck_io_send_cmd_and_buffer(command);
        delay(1);
        if (result == true) return true;
    }
    return false;
}


// Currently unknown:
// - Single pass printer probably does not support variable image width
// - Double pass printer might, since it has explicit Carriage Return and Line Feed commands, but it's not verified
//
// So for the time being require full screen image width
//
// The Duck Printer mechanical Carriage Return + Line Feed process takes about
// 500 msec for the print head to travel back to the start of the line.
//
// After that there is about a 600 msec period before the printer head
// starts moving. The ASIC between the CPU and the printer may be
// buffering printer data during that time so it can stream it out
// with the right timing.
//
// So giving the printer ~1000 msec between tile rows is needed
// to avoid printing glitches. The duration was determined through
// trial and error.
//
// The same  ~1000 msec delay should be present after the last
// tile row is sent to allow the printer to finish processing
// that row before sending other commands. For example if this
// isn't done and a keyboard poll is sent immediately after
// the last tile row, then the peripheral controller seems to
// trigger a cpu reset.
//
bool duck_io_print_screen(void) {

    bool return_status = true;

    // Check for printer connectivity
    uint8_t printer_type = duck_io_printer_query();
    // Fix up printer status == 3 to be printer type 1, sort of a hack
    if (printer_type == DUCK_IO_PRINTER_MAYBE_BUSY)  printer_type = DUCK_IO_PRINTER_TYPE_1_PASS;
    if (printer_type != DUCK_IO_PRINTER_TYPE_1_PASS) return false;

    // Turn off VBlank interrupt during printing
    uint8_t int_enables_saved = IE_REG;
    set_interrupts(IE_REG & ~VBL_IFLAG);

    // Starting with a blank row (like system rom does) avoids a glitch where
    // a tile is skipped somewhere in the very first row printed
    print_blank_row(printer_type);

    // Send the tile data row by row
    for (uint8_t map_row = 0; map_row < DEVICE_SCREEN_HEIGHT; map_row++) {
        prepare_tile_row(map_row, BITPLANE_BOTH);
        return_status = duck_io_send_tile_row_1pass();
        if (return_status == false) break;
    }

    // Print up to N blank rows to scroll the printed result up
    // past the printer tear off position
    if (return_status) {
        for (uint8_t blank_row=0u; blank_row < PRINT_NUM_BLANK_ROWS_AT_END; blank_row++) {
            print_blank_row(printer_type);
        }
    }

    // Restore VBlank interrupt
    set_interrupts(int_enables_saved);

    return return_status;
}


// Prints a blank tile row by filling the pattern data with all zeros
// and then printing a row
static bool print_blank_row(uint8_t printer_type) {

    // Fill print buffer with zero's
    uint8_t * p_buf = tile_row_buffer;
    for (uint8_t c = 0u; c < (DEVICE_SCREEN_WIDTH * BYTES_PER_PRINTER_TILE); c++) {
        *p_buf++ = 0x00u;
    }
    
    return duck_io_send_tile_row_1pass();
}


static void prepare_tile_row(uint8_t row, uint8_t tile_bitplane_offset) {
    
    uint8_t tile_buffer[BYTES_PER_VRAM_TILE];
    uint8_t * p_row_buffer = tile_row_buffer;

    bool    use_win_data = (((row * TILE_HEIGHT) >= WY_REG) && ( LCDC_REG & LCDCF_WINON));
    uint8_t col = 0;

    if (!use_win_data) {
        // Add Scroll offset to closest tile (if rendering BG instead of Window)
        row += (SCY_REG / TILE_HEIGHT);
        col += (SCX_REG / TILE_WIDTH);
    }

    // Loop through tile columns for the current tile row
    for (uint8_t tile = 0u; tile < DEVICE_SCREEN_WIDTH; tile++) {
        
        uint8_t tile_id;
        // Get the Tile ID from the map then copy it's tile pattern data into a buffer
        if (use_win_data) {
            // Read window data instead if it's enabled and visible
            tile_id = get_win_tile_xy((col + tile) & (DEVICE_SCREEN_BUFFER_HEIGHT - 1), 
                    (row - (WY_REG / TILE_HEIGHT)) & (DEVICE_SCREEN_BUFFER_HEIGHT - 1));
        } else {
            // Otherwise use the normal BG
            tile_id = get_bkg_tile_xy((col + tile) & (DEVICE_SCREEN_BUFFER_HEIGHT - 1), 
                                               row & (DEVICE_SCREEN_BUFFER_HEIGHT - 1));
        }
        get_bkg_data(tile_id, 1u, tile_buffer);

        // Mirror, Rotate -90 degrees and reduce tile to 1bpp
        if (tile_bitplane_offset == BITPLANE_BOTH)
            convert_tile_dithered(p_row_buffer, tile_buffer);
        else
            convert_tile(p_row_buffer, tile_buffer + tile_bitplane_offset);
        p_row_buffer += BYTES_PER_PRINTER_TILE;
    }
}


// Transforming tile data for Printer use
//
// This (1bpp) input tile               Should be transformed to the following PRINTER formatted output:
//
//      *BITS* (X)        Tile                 bytes (X)
//       7 ___ 0          Bytes               0 ___ 7
//                         |
//    0 X.......  = [0] = 0x80              0 X.......
// (Y)| X.......  = [1] = 0x80           (Y)| X.......
//    | X.......  = [2] = 0x80            * | X.......
//  b | X.......  = [3] = 0x80            B | X.......
//  y | ........  = [4] = 0x00            I | ........
//  t | ........  = [5] = 0x00            T | ........
//  e | ........  = [6] = 0x00            S | ........
//  s 7 .XXXXXXX  = [7] = 0x7F            * 7 .XXXXXXX
//                                          [0 ...  7] <- Tile Bytes <- {0xF0, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01}
//
//  The first ROW in above                 The first COLUMN in above
//  represents the byte 0x80               represents the byte 0xF0


// Converts one plane (i.e. monochrome, not 4 shades of grey)
// of an 8x8 Game Boy format tile for printing on the Mega Duck Printer. 
static void convert_tile(uint8_t * p_out_buf, uint8_t * p_tile_buf) {

    // Clear printer tile
    for (uint8_t c = 0u; c < BYTES_PER_PRINTER_TILE; c++) {
        p_out_buf[c] = 0u;
    }

    // Transform tile bytes into printable row buffer bytes
    // Tile must get flipped horizontally and rotated -90 degrees
    // 
    // Note the +2 increment for tile row, skipping the interleaved higher bit plane
    //
    // For each tile row byte take the X axis bits representing pixels
    // and transform them into column oriented bits spread across 8 bytes
    //
    uint8_t out_bit = 0x80u;  // X axis bit to set in the output for corresponding input
    for (uint8_t vram_tile_row = 0u; vram_tile_row < BYTES_PER_VRAM_TILE; vram_tile_row += 2u) {
        uint8_t tile_byte = p_tile_buf[vram_tile_row];

        // Scan X axis Left to right
        uint8_t tile_bit = 0x80u;
        for (uint8_t out_col = 0; out_col < TILE_HEIGHT; out_col++) {

            if (tile_byte & tile_bit) p_out_buf[out_col] |= out_bit;
            tile_bit >>= 1;
        }
        out_bit >>= 1;
    }
}


// Converts one both planes (i.e. 4 shades of grey) of an 8x8 Game Boy format
// tile into partially dithered monochrome for printing on the Mega Duck Printer.
//
// Color 0: always white
// Color 1: white or black based on checkerboard dither pattern
// Color 2 or 3: always black
static void convert_tile_dithered(uint8_t * p_out_buf, uint8_t * p_tile_buf) {

    // Clear printer tile
    for (uint8_t c = 0u; c < BYTES_PER_PRINTER_TILE; c++) {
        p_out_buf[c] = 0u;
    }

    // Transform tile bytes into printable row buffer bytes
    // Tile must get flipped horizontally and rotated -90 degrees
    //
    // For each tile row byte take the X axis bits representing pixels
    // and transform them into column oriented bits spread across 8 bytes
    //
    uint8_t out_bit = 0x80u;  // X axis bit to set in the output for corresponding input
    uint8_t dither  = 0xAAu;  // Dither pattern
    for (uint8_t vram_tile_row = 0u; vram_tile_row < BYTES_PER_VRAM_TILE; vram_tile_row += 2u) {
        uint8_t tile_byte0 = p_tile_buf[vram_tile_row];
        uint8_t tile_byte1 = p_tile_buf[vram_tile_row+1];

        // LSByte first, Scan X axis Left to right
        uint8_t tile_bit = 0x80u;
        for (uint8_t out_col = 0; out_col < TILE_HEIGHT; out_col++) {

            if (tile_byte1 & tile_bit) {
                p_out_buf[out_col] |= out_bit;; // Color 2 or 3 = always on
            } else if ((tile_byte0 & dither) & tile_bit) {
                p_out_buf[out_col] |= out_bit; // Color 1 enabled based on checkerboard dither pattern
            }

            tile_bit >>= 1;
        }
        // Flip dither pattern for next source tile row
        dither = ~dither;
        out_bit >>= 1;
    }
}


static bool duck_io_send_tile_row_1pass(void) {

    uint8_t * p_row_buffer = tile_row_buffer;

    // Send 13 x 12 byte packets with row data
    duck_io_tx_buf_len = PRINTER_LEN_12_ROW_DATA;
    for (uint8_t packet = 0u; packet < PRINTER_1_PASS_ROW_NUM_PACKETS; packet++) {

        for (uint8_t c = 0u; c < (duck_io_tx_buf_len); c++)
            duck_io_tx_buf[c] = *p_row_buffer++;

        if (!print_send_command_and_buffer_delay_1msec_10x_retry(DUCK_IO_CMD_PRINT_SEND_BYTES)) {
            // This delay seems to fix periodic skipped tile glitching
            delay(PRINT_DELAY_BETWEEN_ROWS_1000MSEC);

            return false; // Fail out if there was a problem
        }
    }

    uint8_t txbyte;
    // Now send remaining bulk non-packetized data (unclear why transmit methods are split)
    for (txbyte = 0u; txbyte < PRINTER_1_PASS_ROW_NUM_BULK_DATA_BYTES; txbyte++) {
        duck_io_read_byte_with_msecs_timeout(PRINTER_1_PASS_BULK_ACK_TIMEOUT_100MSEC);
        duck_io_send_byte(*p_row_buffer++);
    }

    // Send last four bulk bytes after end of tile data, unclear what they are for
    for (txbyte = 0u; txbyte < PRINTER_1_PASS_ROW_NUM_BULK_UNKNOWN_BYTES; txbyte++) {
        duck_io_read_byte_with_msecs_timeout(PRINTER_1_PASS_BULK_ACK_TIMEOUT_100MSEC);
        duck_io_send_byte(0x00);
    }

    // Wait for last bulk data ACK (with 1msec delay for unknown reason)
    duck_io_read_byte_with_msecs_timeout(PRINT_ROW_END_ACK_WAIT_TIMEOUT_200MSEC);
    
    // End of row: wait for Carriage Return confirmation ACK from the printer
    // System ROM doesn't seem to care about the return value, so we won't either for now
    duck_io_read_byte_with_msecs_timeout(PRINT_ROW_END_ACK_WAIT_TIMEOUT_200MSEC);

    // This delay seems to fix periodic skipped tile glitching
    // as well as peripheral controller asic lockup and cpu reset
    // if the keyboard is polled too soon after the end of a 
    // print row is sent.
    delay(PRINT_DELAY_BETWEEN_ROWS_1000MSEC);

    return true; // Success
}
