#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef _MEGADUCK_PRINTER_H
#define _MEGADUCK_PRINTER_H

// // Thermal Printer related 
#define PRINTER_CARRIAGE_RETURN       0x0D  // Return print head to start of 8 pixel high row
#define PRINTER_LINE_FEED             0x0A  // Feed printer paper to next 8 pixel high row (there are two print passes per-row [to print different greys], so LF only happens every other row of printing)
#define PRINTER_LEN_END_ROW_DATA_SZ   4     // 4 data bytes
#define PRINTER_LEN_5_END_ROW_CR      5     // 5 bytes (4 data bytes + CR + LF)
#define PRINTER_LEN_6_END_ROW_CRLF    6     // 6 bytes (4 data bytes + CR + LF)
#define PRINTER_LEN_12_ROW_DATA       12    // 12 data bytes
#define PRINTER_CR_IDX                (PRINTER_LEN_5_END_ROW_CR - 1)   // Byte number 5
#define PRINTER_LF_IDX                (PRINTER_LEN_6_END_ROW_CRLF - 1) // Byte number 6

#define PRINTER_2_PASS_ROW_NUM_PACKETS  14u
#define PRINTER_2_PASS_ROW_LAST_PACKET  (PRINTER_2_PASS_ROW_NUM_PACKETS - 1u)

#define PRINTER_1_PASS_ROW_NUM_PACKETS             4u
#define PRINTER_1_PASS_ROW_NUM_BULK_BYTES          118u
#define PRINTER_1_PASS_ROW_NUM_BULK_DATA_BYTES     112u
#define PRINTER_1_PASS_ROW_NUM_BULK_UNKNOWN_BYTES  (PRINTER_1_PASS_ROW_NUM_BULK_BYTES - PRINTER_1_PASS_ROW_NUM_BULK_DATA_BYTES)
#define PRINTER_1_PASS_BULK_ACK_TIMEOUT_100MSEC    100u

#define PRINT_ROW_END_ACK_WAIT_TIMEOUT_200MSEC     200u // Presumably waiting for a carriage return confirmation ACK from the printer
#define PRINT_DELAY_BETWEEN_ROWS_1000MSEC         1000u // Waiting for the print head to return to the start of the line and buffer to be cleared


#define BYTES_PER_PRINTER_TILE  8u
#define BYTES_PER_VRAM_TILE     16u
#define TILE_HEIGHT             8u
#define TILE_WIDTH              8u

// Buffer to store an entire queued tile row for printing
extern uint8_t tile_row_buffer[DEVICE_SCREEN_WIDTH * BYTES_PER_PRINTER_TILE];


bool duck_printer_send_tile_row_1pass(void);
void duck_printer_convert_tile(uint8_t * p_out_buf, uint8_t * p_tile_buf);
void duck_printer_convert_tile_dithered(uint8_t * p_out_buf, uint8_t * p_tile_buf);
bool duck_printer_send_tile_row_1pass(void);


#endif // _MEGADUCK_PRINTER_H
