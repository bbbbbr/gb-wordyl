// gameboy_color.c

#include <gbdk/platform.h>

#include "common.h"
#include "gfx_common.h"
#include "gameboy_color.h"

// These store a map for where to apply the CGB attributes for a given tile letter
cgb_attr_xy board_cgb_addrs[BOARD_GRID_H * BOARD_GRID_W];
uint8_t board_cgb_addrs_last;


const palette_color_t cgb_palettes[] = {
                                // First Pal (White) has second color a little darker than others
                                // so that it shows up ok on unmodded CGB hardware when used
                                // to show not-matched-letter tiles
                               RGB8(255u,255u,255u), RGB8(125u,125u,125u),           RGB8(85u,85u,85u),RGB8(0u,0u,0u),
                               // RGB8(255u,243u, 86u), RGB8(255u /2, 243u /2, 86u  /2),RGB8(85u,85u,85u),RGB8(0u,0u,0u), // col 0 = yellow
                               RGB8(255u,120u, 90u), RGB8(255u /2, 120u /2, 90u  /2),RGB8(85u,85u,85u),RGB8(0u,0u,0u), // col 0 = orqnge
                               RGB8(130u,255u,130u), RGB8(130u /2, 255u /2, 130u /2),RGB8(85u,85u,85u),RGB8(0u,0u,0u), // col 0 = green
                               RGB8(168u,210u,255u), RGB8(168u /2, 240u /2, 255u /2),RGB8(85u,85u,85u),RGB8(0u,0u,0u), // col 0 = blue
                               //RGB8(255u,150u,150u), RGB8(255u /2, 150u /2, 150u /2),RGB8(85u,85u,85u),RGB8(0u,0u,0u),        // Pink BG  / Black Text
                               // RGB8(255u,150u,150u), RGB8(255u /2, 150u /2, 150u /2),RGB8(85u,85u,85u),RGB8(255u,255u,255u), // Pink BG  / White Text
                               RGB8(255u,190u,190u), RGB8(255u /2, 150u /2, 150u /2),RGB8(85u,85u,85u),RGB8(160u,59u,55u),      // Pink BG  / Red Text
                               // RGB8(255u,255u,255u), RGB8(255u,255u,255u),           RGB8(85u,85u,85u),RGB8(190u,190u,190u), // White BG / Light Grey Text
                               // RGB8(180u,180u,180u), RGB8(255u,255u,255u),           RGB8(85u,85u,85u),RGB8(255u,255u,255u), // Light Grey BG / White Text
                               RGB8(168u,210u,255u), RGB8(168u /2, 240u /2, 255u /2),RGB8(85u,85u,85u),RGB8(168u /2, 240u /2, 255u /2), // col 0 = blue #2
                               RGB8(255u,255u,255u), RGB8(255u,255u,255u),           RGB8(0u,0u,0u),   RGB8(136u,136u,136u), // White v2 (used for sprite overlay, needs non-transparent white)
                          };

// Now handled by Fade in/out instead
//
// // Initialize CGB palettes
// void cgb_set_palettes(void) {
//     set_bkg_palette(0, ARRAY_LEN(cgb_palettes) / 4, cgb_palettes);
//     set_sprite_palette(0, ARRAY_LEN(cgb_palettes) / 4, cgb_palettes);
// }


// Use CGB 2x mode if available and set up CGB palettes
void cgb_check_and_init(void) {

    if (IS_CGB) {
        cpu_fast();
        // cgb_set_palettes();
    }
}


// Set up colors for board area
void splash_initgfx_cgb(void) {

    // Select CGB attribute tile mode for bkg map access
    VBK_REG = VBKF_BKG_ATTRIB;

    // Fill screen top region under board with white
    fill_bkg_rect(0,0, DEVICE_SCREEN_BUFFER_WIDTH,
                  DEVICE_SCREEN_BUFFER_HEIGHT,
                  CGB_PAL_WHITE);

    // Return to normal tile BKG mode
    VBK_REG = VBKF_BKG_TILES;
}


// Set up colors for board area
void board_initgfx_cgb(void) {

    // Select CGB attribute tile mode for bkg map access
    VBK_REG = VBKF_BKG_ATTRIB;

    // Fill screen top region under baord with blue
    fill_bkg_rect(0,0, DEVICE_SCREEN_BUFFER_WIDTH,
                  BOARD_GRID_Y + BOARD_GRID_TILE_H,
                  CGB_PAL_SCREEN_BKG);

    // Return to normal tile BKG mode
    VBK_REG = VBKF_BKG_TILES;
}


// Set the CGB color for a board letter square
void board_fill_letter_cgb_pal(uint8_t row, uint8_t col, uint8_t palnum) {

    // Select CGB attribute tile mode for bkg map access
    VBK_REG = VBKF_BKG_ATTRIB;

    uint8_t idx = col + (row * BOARD_GRID_W);

    // Don't render anything at 0,0 which is unpopulated
    if (idx <= board_cgb_addrs_last)
    {
        // Fill region with requested CGB palette attribute
        fill_bkg_rect(board_cgb_addrs[idx].x,
                      board_cgb_addrs[idx].y,
                      BOARD_TILE_W, BOARD_TILE_H,
                      palnum & CGB_PAL_MASK);
    }
/*    fill_bkg_rect(BOARD_GRID_X + (col * BOARD_TILE_W),
                  BOARD_GRID_Y + (row * BOARD_TILE_H),
                  BOARD_TILE_W, BOARD_TILE_H,
                  palnum & CGB_PAL_MASK);
*/
    // Return to normal tile BKG mode
    VBK_REG = VBKF_BKG_TILES;
}