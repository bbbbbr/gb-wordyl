#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef _MEGADUCK_PRINTSCREEN_H
#define _MEGADUCK_PRINTSCREEN_H

#define PRINT_NUM_BLANK_ROWS_AT_END                  3u

// Bitplane offsets into tile pattern data for 2 pass printing
#define BITPLANE_0    0
#define BITPLANE_1    1
#define BITPLANE_BOTH 2

bool duck_print_screen(void);
bool duck_print_blank_row(void);

#endif // _MEGADUCK_PRINTSCREEN_H
