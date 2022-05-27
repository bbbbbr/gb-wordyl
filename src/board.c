// board.c

#include <gbdk/platform.h>
#include <gbdk/incbin.h>
#include <gb/gbdecompress.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "gfx_common.h"
#include "sfx_common.h"
#include "gameboy_color.h"

#include "keyboard.h"
#include "board.h"

char empty_word_buf[WORD_LENGTH + 1] = "     ";

uint8_t g_board_tile_flip_speed;

uint8_t g_board_letter_cursor = 0;

/*
// 5 x 6 array of 2x2 metatiles arranged as first row:0,1 second row: 2,3
const uint8_t board_map[]  = {
 0,1,   4,5,   8, 9, 12,13, 16,17,
 2,3,   6,7,  10,11, 14,15, 18,19,

20,21, 24,25, 28,29, 32,33, 36,37,
22,23, 26,27, 30,31, 34,35, 38,39,

40,41, 44,45, 48,49, 52,53, 56,57,
42,43, 46,47, 50,51, 54,55, 58,59,

60,61, 64,65, 68,69, 72,73, 76,77,
62,63, 66,67, 70,71, 74,75, 78,79,

80,81, 84,85, 88,89, 92,93, 96,97,
82,83, 86,87, 90,91, 94,95, 98,99,

100,101, 104,105, 108,109, 112,113, 116,117,
102,103, 106,107, 110,111, 114,115, 118,119
};
*/

#define BRD_CRD_Y(y) (BOARD_TILE_Y_START + ((y) * BOARD_TILE_H))
#define BRD_CRD_X(x) (BOARD_TILE_X_START + ((x) * BOARD_TILE_W))
#define BRD_CRD_X_END(x, x_len) (BOARD_TILE_X_START + ((x + x_len) * BOARD_TILE_W))
#define BRD_ROW(y, x, x_len) BRD_CRD_Y(y) , BRD_CRD_X(x) , BRD_CRD_X_END(x, x_len)
#define BRD_CRD_STOP 0xFFu

// Range for consecutive board rows: start offset, length
// This makes a 5 x 6 grid for gameplay
// y, x_start, x_length -> transformed to -> x, x_start, x_end
const uint8_t board_row_ranges_game[] =
  {BRD_ROW(0u, 0u, BOARD_GRID_W),
   BRD_ROW(1u, 0u, BOARD_GRID_W),
   BRD_ROW(2u, 0u, BOARD_GRID_W),
   BRD_ROW(3u, 0u, BOARD_GRID_W),
   BRD_ROW(4u, 0u, BOARD_GRID_W),
   BRD_ROW(5u, 0u, BOARD_GRID_W),
   BRD_CRD_STOP};

// This makes an irregular grid for the intro screen
const uint8_t board_row_ranges_splash[] =
  {BRD_ROW(0u, 1u, 4u), // Game
   BRD_ROW(1u, 3u, 3u), // Boy
   BRD_ROW(2u, 2u, 1u), // Blank
   BRD_ROW(3u, 1u, 4u), // Word..
   BRD_ROW(4u, 0u, 3u), // Blank
   BRD_ROW(4u, 4u, 2u), // ..yl
   BRD_CRD_STOP};

const uint8_t * p_board_layout;

// 2x2 BG metatiles on the board
const uint8_t board_map_letter[]  = {0, 1, 2, 3};


// This is about smaller in code size than the const map version above ^^^
//
// Draw the board tile map
// * Game Board is 5 x 6 array of 2x2 metatiles arranged as first row:0,1 second row: 2,3
// * Also supports aribtrary row starts and lengths
void board_map_fill() {

    const uint8_t * p_range = p_board_layout;
    // const uint8_t * p_range = board_row_ranges_game;
    uint8_t tile_index = BG_TILES_BOARD_LETTERS_START;

    uint8_t * p_board_cgb_addrs = (uint8_t *)board_cgb_addrs;

    while (1) {
        uint8_t y     = *p_range++;
        if (y == BRD_CRD_STOP) break;

        uint8_t start = *p_range++;
        uint8_t end   = *p_range++;

        for (uint8_t x = start; x < end; x += BOARD_TILE_W) {

            // Store x,y coordinates of tile for CGB attribute drawing later
            *p_board_cgb_addrs++ = x;
            *p_board_cgb_addrs++ = y;

            set_bkg_based_tiles(x, y, BOARD_TILE_W, BOARD_TILE_H,
                                board_map_letter, tile_index);
            tile_index += (BOARD_TILE_W * BOARD_TILE_H);
        }
    }

    // board_cgb_addrs_last = board_cgb_addrs_index;
    board_cgb_addrs_last = (tile_index - (BG_TILES_BOARD_LETTERS_START + (BOARD_TILE_W * BOARD_TILE_H))) >> 2;
}


// Reset guess auto-filled flags
// Should be called **before** autofill is called at start of a new guess and game
void board_reset_autofilled_flags(void) {
    for (uint8_t c=0; c < WORD_LENGTH; c++)
        guess_auto_filled[c] = false;
}


// Auto-populate current guess with exact matches from previous guess
//
// Can be called manually (hotkey) or automatically via an option
void board_autofill_matched_letters(void) {

    // Tile flip is ON for manual trigger, but OFF for automatic since it
    // creates a disorienting blur between guess reveal and subsequent copy-down.
    if (game_settings.opt_autofill_enabled)
        BOARD_SET_FLIP_SPEED(BOARD_TILE_FLIP_NONE);

    // Don't auto-fill on the first guess (nothing to fill with) nor last guess
    if ((guess_num > 0) && (guess_num < MAX_GUESSES)) {
        // Fill in end to start for ease of setting the cursor left-most
        for (int8_t c = WORD_LENGTH - 1; c >= 0; c--) {

            if (exact_matches[c]) {
                play_sfx(SFX_TILE_REVEAL_RESULT);

                // Copy letter into current guess
                // and flag as auto-filled
                guess[c] = exact_matches[c];
                guess_auto_filled[c] = true;

                // Make sure autofill coloring will draw corerctly
                guess_eval[c] = LETTER_RIGHT_PLACE;
                board_draw_letter(guess_num, c, guess[c], BOARD_HIGHLIGHT_YES);
            }
            else if (!guess[c]) {
                // This will end up with cursor set at left-most
                // un-filled letter in the current guess
                guess_letter_cursor = c;
            }
        }
    }

    // Redraw the letter cursor in case it moved
    board_update_letter_cursor();
}


// Tile flip animation frames
const uint8_t board_flip_anim[] = {
    (BOARD_LETTERS_FLIP_1),
    (BOARD_LETTERS_FLIP_2),
    (BOARD_LETTERS_FLIP_3),
    (BOARD_LETTERS_FLIP_4),
    (BOARD_LETTERS_FLIP_3),
    (BOARD_LETTERS_FLIP_2),
    (BOARD_LETTERS_FLIP_1)
};



#define BOARD_LETTER_CURSOR_X_AT_COL(col) (5u + (BOARD_GRID_X * 8u) + (col * (BOARD_TILE_W * 8u)) + DEVICE_SPRITE_PX_OFFSET_X)
#define BOARD_LETTER_CURSOR_Y_AT_ROW(row) (1u + (row * (BOARD_TILE_W * 8u)) + (BOARD_TILE_Y_START * 8u) + DEVICE_SPRITE_PX_OFFSET_Y)

// Move the cursor to highlight the current row
void board_update_letter_cursor(void) {

    uint8_t x = BOARD_LETTER_CURSOR_X_AT_COL(guess_letter_cursor);
    uint8_t y = BOARD_LETTER_CURSOR_Y_AT_ROW(guess_num);

    move_sprite(SP_ID_CURSOR_LETTER_START     , x     , y);
    move_sprite(SP_ID_CURSOR_LETTER_START + 1u, x + 8u, y);
    move_sprite(SP_ID_CURSOR_LETTER_START + 2u, x -2u    , y + 8u -2u);
    move_sprite(SP_ID_CURSOR_LETTER_START + 3u, x + 8u - 2u, y + 8u - 2u);
}

// Move the cursor to highlight the current row
void board_hide_letter_cursor(void) {

    for (uint8_t i = 0; i < SP_ID_CURSOR_LETTER_LEN; i++) {
        hide_sprite(SP_ID_CURSOR_LETTER_START + i);
    }
}



/*
TODO: delete if no longer used
// Move the cursor to highlight the current row
void board_hide_row_cursor(void) {

    for (uint8_t i = 0; i < SP_ID_CURSOR_BOARD_LEN; i++) {
        hide_sprite(SP_ID_CURSOR_BOARD_START + i);
    }
}
*/

#define CURSOR_BOARD_X_POS (((BOARD_TILE_X_START * 8)- 16u) + DEVICE_SPRITE_PX_OFFSET_X)

// Move the cursor to highlight the current row
void board_update_row_cursor(void) {

    // guess_num is desired row
    uint8_t x = CURSOR_BOARD_X_POS;
    uint8_t y = (guess_num * (8u * BOARD_TILE_W)) + (DEVICE_SPRITE_PX_OFFSET_Y + (BOARD_TILE_Y_START * 8u));

    move_sprite(SP_ID_CURSOR_BOARD_START     , x     , y);
    move_sprite(SP_ID_CURSOR_BOARD_START + 1u, x + 8u, y);
    move_sprite(SP_ID_CURSOR_BOARD_START + 2u, x     , y + 8u);
    move_sprite(SP_ID_CURSOR_BOARD_START + 3u, x + 8u, y + 8u);

    // Takes more ROM space:
    /*
    for (uint8_t i = 0; i < SP_ID_CURSOR_BOARD_LEN; i++) {
        move_sprite(SP_ID_CURSOR_BOARD_START + i,
                    CURSOR_BOARD_X_POS + sp_cursor_offset_x[i],
                    y + sp_cursor_offset_y[i]);
    }
    */
}


// Move the cursor to highlight the current row
void board_hide_cursor(void) {

    for (uint8_t i = 0; i < SP_ID_CURSOR_BOARD_LEN; i++) {
        hide_sprite(SP_ID_CURSOR_BOARD_START + i);
    }
}



// Draw the letters for a guess as they are entered
void board_render_guess_letter_at_cursor(void) {

    BOARD_SET_FLIP_SPEED(BOARD_TILE_FLIP_FAST);

    // Cheat Mode: Highlight word letters as typed in debug mode
    #ifdef DEBUG_REVEAL_WHILE_TYPE
        // Note: This debug feature might be broken since adding a movable letter cursor
        //
        // evaluate_letters(guess);
        // board_set_color_for_letter(guess_num, col, BOARD_HIGHLIGHT_YES);
        board_draw_word(guess_num, guess, BOARD_HIGHLIGHT_YES);
    #else
        // Hide and then re-show the letter cursor so it doesn't obstruct the animation
        // the letter cursor will get restored and updated in main loop
        board_hide_letter_cursor();
        board_draw_letter(guess_num, guess_letter_cursor, guess[guess_letter_cursor], BOARD_HIGHLIGHT_NO);
        // board_update_letter_cursor();
    #endif
}


bool opt_skip_matched = true;

// Add a guess letter to the board
void board_add_guess_letter(void) {

    // Add letter to the space if it's not already filled
    if (! guess[guess_letter_cursor]) {
        guess[guess_letter_cursor] = keyboard_get_letter();
        board_render_guess_letter_at_cursor();

    }

    // Advance the cursor if applicible
    if (game_settings.opt_skip_autofilled && (guess_num != (GUESS_NUM_FIRST))) {
        // If skip autofilled is enabled AND it's not the first guess
        // then skip over auto-filled board letter slots
        for (int8_t c = guess_letter_cursor + 1; c < WORD_LENGTH; c++) {
            // Only select slot if not currently auto-filled
            if (guess_auto_filled[c] == false) {
                guess_letter_cursor = c;
                break;
            }
        }
    } else {
        // Default behavior: move to next letter regardless if auto-match
        if (guess_letter_cursor < LETTER_CURSOR_MAX) {
            guess_letter_cursor++;
        }
    }
}


// Add a guess letter to the board
void board_remove_guess_letter(void) {

    // If current letter is blank, act as Backspace key
    // otherwise behave as a Delete key
    if (! guess[guess_letter_cursor]) {

        // Advance the cursor if applicible
        if (game_settings.opt_skip_autofilled && (guess_num != (GUESS_NUM_FIRST))) {
            // If skip autofilled is enabled AND it's not the first guess
            // then skip over auto-filled board letter slots
            for (int8_t c = guess_letter_cursor - 1; c >= 0; c--) {
                // Only select slot if not currently auto-filled
                if (guess_auto_filled[c] == false) {
                    guess_letter_cursor = c;
                    break;
                }
            }
        } else {
            // Default behavior: move to previous letter regardless if auto-match
            if (guess_letter_cursor > LETTER_CURSOR_START) {
                guess_letter_cursor--;
            }
        }

    }

    if (guess[guess_letter_cursor]) {
        // Clear letter and remove potential auto-fill flag
        guess[guess_letter_cursor] = 0;
        guess_auto_filled[guess_letter_cursor] = false;
        board_render_guess_letter_at_cursor();
    }
}


// Redraw the board for all current guesses
void board_redraw_clean(void) {

    for(int i=0; i < MAX_GUESSES; i++) {
        board_draw_word(i, NULL, BOARD_HIGHLIGHT_NO);
    }
}


// Direct render a tile to VRAM to fill a board letter square
// Previous calls to set_1bpp_colors() will affect colors produced here
void board_draw_letter_bits(uint8_t row, uint8_t col, uint8_t letter) {

    // Draw letter into VRAM address for desired board tile
    // From offset into 1bpp letter source of 4x4 metatiles
    set_bkg_1bpp_data(BG_TILES_BOARD_LETTERS_START
                        + (col * BOARD_GRID_TILES_PER_LETTER)
                        + (row * BOARD_GRID_TILES_PER_ROW),
                      BOARD_GRID_TILES_PER_LETTER,
                      board_letters_decomp_buf + (letter * BOARD_GRID_VRAM_BYTES_PER_LETTER));
}


// Board tile flip animation: Direct render a tile to VRAM to fill a board letter square
// Previous calls to set_1bpp_colors() will affect colors produced here
//
// Faster flip speeds look ok on all hardware except stock CGB LCD
// which seems to drop some frames?
void board_draw_tile_flip_anim(uint8_t row, uint8_t col) {


    for (uint8_t c = 0; c < ARRAY_LEN(board_flip_anim); c++) {
        board_draw_letter_bits(row, col, board_flip_anim[c]);

        // One frame between animations in all modes
        wait_vbl_done();

        // Another frame of delay in slow mode
        if (g_board_tile_flip_speed == BOARD_TILE_FLIP_SLOW) {
            wait_vbl_done();
        }
    }
}


// == Lookup tables for colorizing board letters ==

// CGB color array is 1 color per entry
const uint8_t board_cgb_colors[] = {
    BOARD_CGB_PAL_BLANK,       // LETTER_NOT_SET
    BOARD_CGB_PAL_NOT_IN_WORD, // LETTER_NOT_MATCHED
    BOARD_CGB_PAL_CONTAINS,    // LETTER_WRONG_PLACE
    BOARD_CGB_PAL_MATCHED,     // LETTER_RIGHT_PLACE
    BOARD_CGB_PAL_ENTRY,       // LETTER_BEING_ENTERED
};

// DMG color array is 2x colors per entry
const uint8_t board_dmg_colors[] = {
    BOARD_DMG_COLOR_BLANK,        // LETTER_NOT_SET
    BOARD_DMG_COLOR_NOT_IN_WORD,  // LETTER_NOT_MATCHED
    BOARD_DMG_COLOR_CONTAINS,     // LETTER_WRONG_PLACE
    BOARD_DMG_COLOR_MATCHED,      // LETTER_RIGHT_PLACE
    BOARD_DMG_COLOR_ENTRY,        // LETTER_BEING_ENTERED
};


// Set board letter color highlighting for match status, blank, or letter entry
inline void board_set_highlight_color_for_letter(uint8_t row, uint8_t col, uint8_t match_type) {

    if (IS_CGB) {
        // Apply the CGB coloring
        SET_BOARD_COLOR_FOR_CGB;
        board_fill_letter_cgb_pal(row, col, board_cgb_colors[match_type]);
    }
    else {
        // DMG mode
        // DMG color array is 2x colors per entry
        match_type <<= 1;
        set_1bpp_colors(board_dmg_colors[match_type], board_dmg_colors[match_type + 1]);
    }
}


// Direct render a tile to VRAM to fill a board letter square
// Previous calls to set_1bpp_colors() will affect colors produced here
//
void board_draw_letter(uint8_t row, uint8_t col, uint8_t letter, bool do_highlight) {

    uint8_t match_type;

    if (g_board_tile_flip_speed) {
        // Used for tile flip, but may get overridden below by board_set_color_for_letter()
        SET_BOARD_COLOR_ENTRY;
        board_draw_tile_flip_anim(guess_num, col);
    }


    if ((letter == ' ') || (letter == 0x00))
        letter = BOARD_LETTERS_SPACE_CHAR;
    else if (letter >= 'a')
        letter -= 'a';
    else if (letter >= 'A')
        letter -= 'A';


    // Select highlighting mode
    if (do_highlight)
        match_type = guess_eval[col];
    else if (letter == BOARD_LETTERS_SPACE_CHAR)
        match_type = LETTER_NOT_SET;
    else
        match_type = LETTER_BEING_ENTERED;

    board_set_highlight_color_for_letter(row, col, match_type);

    // Now draw the letter
    board_draw_letter_bits(row, col, letter);
}


// Render a word at * p_guess onto the board
void board_draw_word(uint8_t row, uint8_t * p_guess, bool do_highlight) {

    // If it's a request to draw an empty word, use a shim empty string
    if (p_guess == NULL) {
        p_guess = empty_word_buf;
        BOARD_SET_FLIP_SPEED(BOARD_TILE_FLIP_NONE);
    } else {
        BOARD_SET_FLIP_SPEED(BOARD_TILE_FLIP_FAST);
    }

    // Flag guess letters as: LETTER_NOT_IN_WORD, LETTER_WRONG_PLACE or LETTER_RIGHT_PLACE
    evaluate_letters(guess);

    // col maps to the individual letters in the word/guess
    for (uint8_t col = 0; col < BOARD_GRID_W; col ++) {
        if (do_highlight) {
            play_sfx(SFX_TILE_REVEAL_RESULT);
        }
        board_draw_letter(row, col, p_guess[col], do_highlight);
    }
}

