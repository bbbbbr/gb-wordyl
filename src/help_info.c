// help_info.c

#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#include "stats.h"
#include "common.h"
#include "input.h"
#include "gfx_common.h"
#include "gameboy_color.h"

#include "window.h"

#include "board.h"
#include "gameplay.h"

#include <lang_text.h>


static void help_info_draw_color_legend(void);


// Upper right corner of Window, taking popup dialog borders into consideration
#define HELP_LEGEND_MAP_X (20u - 3u)
    // These two are in the translation file in case they need to be adjusted
    // #define HELP_LEGEND_MAP_Y (2u)
    // #define HELP_LEGEND_MAP_Y_GAP (1u)
#define HELP_LEGEND_MAP_W 2u
#define HELP_LEGEND_MAP_H ((BOARD_TILE_H + HELP_LEGEND_MAP_Y_GAP) * (BG_TILES_HELP_LEGEND_NUM_LETTERS))


// CGB color array is 1 color per entry
const uint8_t help_cgb_match_colors[] = {
    BOARD_CGB_PAL_NOT_IN_WORD, // LETTER_NOT_MATCHED
    BOARD_CGB_PAL_CONTAINS,    // LETTER_WRONG_PLACE
    BOARD_CGB_PAL_MATCHED,     // LETTER_RIGHT_PLACE
};


// DMG color array is 2x colors per entry
const uint8_t help_dmg_match_colors[] = {
    BOARD_DMG_COLOR_NOT_IN_WORD,  // LETTER_NOT_MATCHED
    BOARD_DMG_COLOR_CONTAINS,     // LETTER_WRONG_PLACE
    BOARD_DMG_COLOR_MATCHED,      // LETTER_RIGHT_PLACE
};


// Display help info in popup windows
void help_info_show(void) {

    // Prevent the cursor from flashing between popups
    // (The window dialog will handle hiding them)
    WIN_DIALOG_RESTORE_SPRITES_AFTER_NO;

    // Controls popup
    win_dialog_show_message(INTRO_DIALOG_WIN_Y, __INTRO_MESSAGE_STR, NULL);

    // Color Legend popup
    WIN_DIALOG_SET_FUNC_RUN(&help_info_draw_color_legend);
    win_dialog_show_message(HELP_LEGEND_DIALOG_WIN_Y, __HELP_LEGEND_DIALOG_STR, NULL);

    // Restore normal sprite hiding behavior
    WIN_DIALOG_RESTORE_SPRITES_AFTER_YES;
    gameplay_restore_sprites();
}


// Draw a legend for game match status colors
void help_info_draw_color_legend(void) {

    // Draw letter into VRAM address for desired board tile
    // From offset into 1bpp letter source of 4x4 metatiles
    for (uint8_t c = 0; c < BG_TILES_HELP_LEGEND_NUM_LETTERS; c++) {

        uint8_t y_pos = HELP_LEGEND_MAP_Y + (c * (BOARD_TILE_H + HELP_LEGEND_MAP_Y_GAP));

        // Set background tile IDs to ones which will be custom rendered into vram for each 2x2 Board letter
        set_win_based_tiles(HELP_LEGEND_MAP_X, y_pos, BOARD_TILE_W, BOARD_TILE_H,
                            board_map_letter, (BG_TILES_HELP_LEGEND_START) + (c * BOARD_GRID_TILES_PER_LETTER));

        if (IS_CGB) {

            SET_BOARD_COLOR_FOR_CGB;
            // Select CGB attribute tile mode for bkg map access
            VBK_REG = VBKF_BKG_ATTRIB;
            // Fill region with requested CGB palette attribute
            fill_win_rect(HELP_LEGEND_MAP_X, y_pos, BOARD_TILE_W, BOARD_TILE_H, help_cgb_match_colors[c] & CGB_PAL_MASK);
            // Return to normal tile BKG mode
            VBK_REG = VBKF_BKG_TILES;

        } else {
            // DMG color array is 2x colors per entry
            uint8_t match_type = c << 1;
            // Set Foreground/Background DMG colors based on match type
            set_1bpp_colors(help_dmg_match_colors[match_type], help_dmg_match_colors[match_type + 1]);
        }

        // Render the tiles into the reserved VRAM tile slots with the specified Foreground/Background colors
        set_win_1bpp_data(BG_TILES_HELP_LEGEND_START + (c * BOARD_GRID_TILES_PER_LETTER),
                          BOARD_GRID_TILES_PER_LETTER,
                          board_letters_decomp_buf + (('A' - 'A') * BOARD_GRID_VRAM_BYTES_PER_LETTER));
    }

    waitpadticked_lowcpu(J_ANY_KEY);

    // Clear out any CGB coloring from the Window
    if (IS_CGB) {
        // Select CGB attribute tile mode for bkg map access
        VBK_REG = VBKF_BKG_ATTRIB;
        fill_win_rect(HELP_LEGEND_MAP_X, HELP_LEGEND_MAP_Y,
                      HELP_LEGEND_MAP_W, HELP_LEGEND_MAP_H,
                      0x00); // Back to default palette zero
        // Return to normal tile BKG mode
        VBK_REG = VBKF_BKG_TILES;
    }
}
