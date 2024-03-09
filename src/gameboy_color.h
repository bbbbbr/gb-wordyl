// gameboy_color.h

#ifndef _GAMEBOY_COLOR_H
#define _GAMEBOY_COLOR_H

#include <gbdk/platform.h>
#include "common.h"

#define IS_CGB  (_cpu == CGB_TYPE)

#define VBKF_BKG_TILES  0b00000000 // CGB Only: Select Regular Map
#define VBKF_BKG_ATTRIB 0b00000001 // CGB Only: Select Map

#define CGB_PAL_MASK 0x07
#define CGB_PAL_0  0u
#define CGB_PAL_1  1u
#define CGB_PAL_2  2u
#define CGB_PAL_3  3u
#define CGB_PAL_4  4u
#define CGB_PAL_5  5u
#define CGB_PAL_6  6u
#define CGB_PAL_7  7u


#define CGB_PAL_WHITE   CGB_PAL_0
// #define CGB_PAL_YELLOW  CGB_PAL_1
#define CGB_PAL_ORANGE  CGB_PAL_1
#define CGB_PAL_GREEN   CGB_PAL_2
#define CGB_PAL_BLUE    CGB_PAL_3
#define CGB_PAL_RED     CGB_PAL_4
#define CGB_PAL_BLUE_2  CGB_PAL_5

#define CGB_PAL_WHITE_2   CGB_PAL_6  // Used for Sprite overlay (non-transparent white) and faded board letters
#define CGB_PAL_LITE_GREY CGB_PAL_6

#define CGB_PAL_COUNT 7

#define CGB_PAL_SCREEN_BKG CGB_PAL_BLUE_2

// Board Match Color Highlighting
#define BOARD_CGB_PAL_NORMAL       CGB_PAL_WHITE
#define BOARD_CGB_PAL_CONTAINS     CGB_PAL_ORANGE // CGB_PAL_YELLOW
#define BOARD_CGB_PAL_MATCHED      CGB_PAL_BLUE // CGB_PAL_GREEN
#define BOARD_CGB_PAL_NOT_IN_WORD  CGB_PAL_LITE_GREY
// Board NON-Match Color Highlighting
#define BOARD_CGB_PAL_BLANK        CGB_PAL_LITE_GREY
#define BOARD_CGB_PAL_ENTRY        CGB_PAL_WHITE

// Keyboard
#define KEYBD_CGB_PAL_NORMAL       CGB_PAL_WHITE
#define KEYBD_CGB_PAL_CONTAINS     CGB_PAL_ORANGE // CGB_PAL_YELLOW
#define KEYBD_CGB_PAL_MATCHED      CGB_PAL_BLUE // CGB_PAL_GREEN
#define KEYBD_CGB_PAL_NOT_IN_WORD  CGB_PAL_RED

typedef struct cgb_attr_xy {
    uint8_t x;
    uint8_t y;

} cgb_attr_xy;
extern cgb_attr_xy board_cgb_addrs[];
extern uint8_t board_cgb_addrs_last;


extern const palette_color_t cgb_palettes[];

void cgb_check_and_init(void);
void splash_initgfx_cgb(void);
void cgb_set_palettes(void);
void board_initgfx_cgb(void);
void board_fill_letter_cgb_pal(uint8_t row, uint8_t col, uint8_t palnum);

#endif
