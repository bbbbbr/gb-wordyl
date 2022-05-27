// keyboard.c

#include <gbdk/platform.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "gfx_common.h"
#include "gameboy_color.h"
#include "sgb_border.h"

#include "keyboard.h"

// Keyboard map
const char *kb[KEYBD_ROWS] = {
"QWERTYUIOP",
 "ASDFGHJKL",
  "ZXCVBNM"CEDILLA_MAPPED_CHR_STRQUOTE}; // [ is Ç

// Stores color coding for keyboard basde on guess data
uint8_t kb_status[KEYBD_ROWS][10];


// Length of each keyboard row in characters
const int8_t kb_row_len[KEYBD_ROWS] = {
    10,
    9,
    8 // 7
};

// Which tile column each keyboard row starts on
const int8_t kb_offsets[KEYBD_ROWS] = {
    0,
    1,
    2
};

// Keyboard cursor location
int8_t kb_x;
int8_t kb_y;

// Reset keyboard cursor and clear highlighting
void keyboard_reset(void) {
    kb_x = 0;
    kb_y = 0;

    keyboard_redraw_clean();

    // Show cursor on default keyboard key
    keyboard_update_cursor();
}


// return currently selected letter in keyboard
inline char keyboard_get_letter(void) {

    return kb[kb_y][kb_x];
}


// Set the CGB color for a keyboard letter tile
void keyboard_fill_letter_cgb_pal(uint8_t row, uint8_t col, uint8_t palnum) {

    // Select CGB attribute tile mode for bkg map access
    VBK_REG = VBKF_BKG_ATTRIB;

    // Fill region with requested CGB palette attribute
    set_bkg_tile_xy(KEYBD_START_X + (col * 2) + kb_offsets[row], KEYBD_START_Y + row, palnum & CGB_PAL_MASK);

    // Return to normal tile BKG mode
    VBK_REG = VBKF_BKG_TILES;
}


const uint8_t keyboard_cgb_colors[] = {

    KEYBD_CGB_PAL_NORMAL,       // LETTER_NOT_SET
    KEYBD_CGB_PAL_NOT_IN_WORD,  // LETTER_NOT_MATCHED
    KEYBD_CGB_PAL_CONTAINS,     // LETTER_WRONG_PLACE
    KEYBD_CGB_PAL_MATCHED,      // LETTER_RIGHT_PLACE
};

// DMG color array is 2x colors per entry
const uint8_t keyboard_dmg_colors[] = {
    KEYBD_COLOR_NORMAL,          // LETTER_NOT_SET
    KEYBD_COLOR_NOT_IN_WORD,     // LETTER_NOT_MATCHED
    KEYBD_COLOR_CONTAINS,        // LETTER_WRONG_PLACE
    KEYBD_COLOR_MATCHED,         // LETTER_RIGHT_PLACE
};

// SGB color array is 2x colors per entry
const uint8_t keyboard_sgb_colors[] = {
    KEYBD_COLOR_NORMAL_SGB,          // LETTER_NOT_SET
    KEYBD_COLOR_NOT_IN_WORD_SGB,     // LETTER_NOT_MATCHED
    KEYBD_COLOR_CONTAINS_SGB,        // LETTER_WRONG_PLACE
    KEYBD_COLOR_MATCHED_SGB,         // LETTER_RIGHT_PLACE
};


// Set highlight color for a letter on the keyboard based on guess status
void keyboard_set_color_for_letter(uint8_t row, uint8_t col, uint8_t match_type, uint8_t tile_id) {


    // Reject color letter downgrades (only allowed after a keyboard status reset)
    // Otherwise update status
    if (match_type < kb_status[row][col])
        return;
    else
        kb_status[row][col] = match_type;


    if (IS_CGB) {

        // CGB doesn't use DMG style VRAM tile redrawing except on
        // clean redraw where it uses normal print style
        SET_PRINT_COLOR_NORMAL;

        // Apply the CGB coloring
        keyboard_fill_letter_cgb_pal(row, col, keyboard_cgb_colors[match_type]);
    }
    else {

        // DMG/SGB mode
        const uint8_t * p_keyboard_colors;

        if (SGB_IS_ENABLED)
            p_keyboard_colors = keyboard_sgb_colors;
        else
            p_keyboard_colors = keyboard_dmg_colors;

        // DMG/SGB color array is 2x colors per entry
        // Use match type to select correct array index
        p_keyboard_colors +=  (match_type << 1);
        set_1bpp_colors(*p_keyboard_colors, *(p_keyboard_colors + 1));
    }

    // DMG: Always needs to draw
    // CGB: Only draw when redrawing whole keyboard (match_type == LETTER_NOT_SET) CGB just updates the attribute map
    // if (needs_redraw)
    if ((match_type == LETTER_NOT_SET) || (!IS_CGB))
        draw_letter_to_tileid(kb[row][col], tile_id);

}


// Update keyboard highlighting based on a guessed word evaluation
// TODO: could be optimized
void keyboard_update_from_guess(void) {

    uint8_t highest_match_type;
    uint8_t tile_id = BG_TILES_KEYBD_START;

    // Loop through keyboard, check for letters to highlight
    for(uint8_t row = 0; row < 3; row++) {

        uint8_t kbl = kb_row_len[row];
        for(uint8_t col=0; col < kbl; col++) {

            char letter = kb[row][col];

            // See if current keyboard letter is in guess word
            highest_match_type = LETTER_NOT_MATCHED;

            for (int c = 0; c < WORD_LENGTH; c++) {

                if (letter == guess[c]) {

                    // find highest match type in the guess word
                    if (guess_eval[c] > highest_match_type)
                        highest_match_type = guess_eval[c];

                    keyboard_set_color_for_letter(row, col, highest_match_type, tile_id);
                }
            }
            tile_id++;
        }
    }
}


// Reset / Redraw the entire keyboard with no letters highlighted
void keyboard_redraw_clean(void) {

    uint8_t tile_id = BG_TILES_KEYBD_START;
    for(uint8_t row = 0; row < 3; row++) {

        uint8_t kbl = kb_row_len[row];
        for(uint8_t col=0; col < kbl; col++) {

            // Reset keyboard status
            kb_status[row][col] = LETTER_NOT_SET;

            keyboard_set_color_for_letter(row, col, LETTER_NOT_SET, tile_id);
            tile_id++;
        }
    }
}


// Draw a map of incrementing VRAM tiles for the keyboard
// These get drawn into later
void keyboard_draw_map(void) {

    uint8_t tile_id = BG_TILES_KEYBD_START;

    for(uint8_t row = 0; row < 3; row++) {
        uint8_t kbl = kb_row_len[row];

        // Add a blank space between every horizontal letter
        for(uint8_t col=0; col < kbl * 2; col += 2) {
            set_bkg_tile_xy(KEYBD_START_X + col + kb_offsets[row], KEYBD_START_Y + row, tile_id);
            tile_id++;
        }
    }

    // Bar across top of keyboard
    fill_bkg_rect(KEYBD_START_X, KEYBD_START_Y-1, 33, 1, BG_TILES_INTRO_DIALOG_START + DIALOG_TILE_TOP);
}


// Move the cursor to highlight a key
void keyboard_update_cursor(void) {

    uint8_t x = (kb_x * 16) + (kb_offsets[kb_y] * 8) + DEVICE_SPRITE_PX_OFFSET_X;
    uint8_t y = ((KEYBD_START_Y + kb_y) * 8) + DEVICE_SPRITE_PX_OFFSET_Y - KEYBD_CURSOR_OFFSET_Y;

    move_sprite(SP_ID_CURSOR_KBD_START     , x     , y);
    move_sprite(SP_ID_CURSOR_KBD_START + 1u, x + 8u, y);
    move_sprite(SP_ID_CURSOR_KBD_START + 2u, x     , y + 8u);
    move_sprite(SP_ID_CURSOR_KBD_START + 3u, x + 8u, y + 8u);
    // Takes more ROM space:
    // for (uint8_t i = 0; i < SP_ID_CURSOR_BOARD_LEN; i++) {
    //     move_sprite(SP_ID_CURSOR_KBD_START + i, x + sp_cursor_offset_x[i], y + sp_cursor_offset_y[i] );
    // }
}



const int8_t keyboard_dpad_movement[] = {
    1,  0, // J_RIGHT 0x01
   -1,  0, // J_LEFT  0x02
    0, -1, // J_UP    0x04
    0,  0, // shim for gap
    0,  1, // J_DOWN  0x08
};

void keyboard_move_cursor(uint8_t dpad_key) {

    // Using this LUT saves about 50 bytes versus a switch()
    // in main loop and passing x,y dirs
    //
    // LUT is 2 entries per key
    // Mask lowest dpad bit (J_RIGHT) to get
    // So this: 1 2 4 8 -> becomes -> 0 2 4 ~6~ 8
    // Then just use a single shim entry for missing 6 slot
    dpad_key &= 0xFEu;
    int8_t move_x = keyboard_dpad_movement[dpad_key++];
    int8_t move_y = keyboard_dpad_movement[dpad_key];

    // Update Y first (may change X) and handle wraparound
    if (move_y != 0) {
        kb_y += move_y;

        if (kb_y < 0) {
            kb_y = ARRAY_LEN(kb) - 1;
        }
        else if (kb_y >= ARRAY_LEN(kb)) {
            kb_y = 0;
        }

        // Bump X to end or row if it's shorter X if needed on row change
        if(kb_x >= kb_row_len[kb_y]) {
            kb_x = kb_row_len[kb_y] - 1;
        }
    }


    // Update X and handle wraparound
    kb_x += move_x;

    if (kb_x >= kb_row_len[kb_y])
        kb_x = 0;
    else if (kb_x < 0)
        kb_x = kb_row_len[kb_y] - 1;


    keyboard_update_cursor();
}