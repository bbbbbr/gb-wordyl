#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#include <duck/laptop_io.h>

#include "megaduck_printer.h"

uint8_t tile_row_buffer[DEVICE_SCREEN_WIDTH * BYTES_PER_PRINTER_TILE];

// For sending a bulk printer command to megaduck printer
//
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


// Transforming tile data for Printer use
//
//       SCREEN                --->          PRINTER
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


// Converts one plane (i.e. monochrome, not 4 shades of grey) of
// an 8x8 Game Boy format tile for printing on the Mega Duck Printer. 
void duck_printer_convert_tile(uint8_t * p_out_buf, uint8_t * p_tile_buf) {

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
    for (uint8_t tile_row = 0u; tile_row < BYTES_PER_VRAM_TILE; tile_row += 2u) {
        uint8_t tile_byte = p_tile_buf[tile_row];

        // Scan X axis of tile Left to right
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
void duck_printer_convert_tile_dithered(uint8_t * p_out_buf, uint8_t * p_tile_buf) {

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
    for (uint8_t tile_row = 0u; tile_row < BYTES_PER_VRAM_TILE; tile_row += 2u) {
        uint8_t tile_byte0 = p_tile_buf[tile_row];
        uint8_t tile_byte1 = p_tile_buf[tile_row+1];

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


// Send a prepared 20 x 8x8 row of tile data in duck_io_tx_buf[]
// to the Mega Duck printer in 1bpp format.
//
// The tile data should already be transformed using one of the
// duck_printer_convert_tile_...() functions.
bool duck_printer_send_tile_row_1pass(void) {

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
