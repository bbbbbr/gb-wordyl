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
#define PRINTER_1_PASS_BULK_ACK_TIMEOUT_100MSEC    100

#define PRINT_ROW_END_ACK_WAIT_TIMEOUT_200MSEC  200 // Presumably waiting for a carriage return confirmation ACK from the printer

// Bitplane offsets into tile pattern data for 2 pass printing
#define BITPLANE_0    0
#define BITPLANE_1    1
#define BITPLANE_BOTH 2



bool duck_io_print_screen(void);

#endif // _MEGADUCK_PRINTER_H
