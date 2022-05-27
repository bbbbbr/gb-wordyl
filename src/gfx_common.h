// gfx_common.h

#ifndef _GFX_COMMON_H
#define _GFX_COMMON_H

// Sprite Data
#include "../res/cursor_tiles.h"

// Map Data
//#include "../res/board_grid_tiles_gbcompress.h"
#include "../res/font_tiles_1bpp_gbcompress.h"
#include "../res/font_num_tiles_1bpp_gbcompressed.h"
#include "../res/board_letter_tiles_8x8_1bpp_gbcomp.h"

// Intro / Window Dialog data
// #include "../res/dialog_tiles_2bpp_gbcompressed.h"
#include "../res/intro_and_dialog_tiles.h"
#include "../res/intro_dialog_map_gbcompress.h"
#include "../res/intro_stars_map_gbcompress.h"


#define PRINT_BKG 0
#define PRINT_WIN 1

extern uint8_t map_decomp_buf[];
extern uint8_t board_letters_decomp_buf[];
extern uint8_t font_letters_decomp_buf[];


extern const uint8_t tile_blank[];

void gfx_clear_bkg_blank_tile(void);

void sprites_hide_all_offscreen(void);

void print_gotoxy(uint8_t x, uint8_t y, uint8_t target);
void print_str(char * txt);

void opt_hardmode_display(void);

void draw_letter_to_tileid(uint8_t letter, uint8_t index);

void gfx_load(void);



// Hardware correction factor for X position of Window when displayed on screen
#define WIN_X_OFFSET 7


#define SP_TILES_CURSOR_START               0u
    // Keyboard current letter cursor
    #define SP_TILES_CURSOR_KBD_START       (SP_TILES_CURSOR_START)
    #define SP_TILES_CURSOR_KBD_LEN         1u

    // Board row cursor
    #define SP_TILES_CURSOR_BOARD_START     (SP_TILES_CURSOR_KBD_START + SP_TILES_CURSOR_KBD_LEN)
    #define SP_TILES_CURSOR_BOARD_LEN       4u

    // Cursor that makes a border around the current letter tile
    #define SP_TILES_CURSOR_LETTER_START    (SP_TILES_CURSOR_BOARD_START + SP_TILES_CURSOR_BOARD_LEN)
    #define SP_TILES_CURSOR_LETTER_LEN      2u

    // Cursor that makes a border around the current letter tile
    #define SP_TILES_CURSOR_MENU_START      (SP_TILES_CURSOR_LETTER_START + SP_TILES_CURSOR_LETTER_LEN)
    #define SP_TILES_CURSOR_MENU_LEN        1u

#define SP_TILES_CURSOR_COUNT_TOTAL         ((SP_TILES_CURSOR_MENU_START + SP_TILES_CURSOR_MENU_LEN) - SP_TILES_CURSOR_START)


#define SP_ID_CURSORS_START            0u
    #define SP_ID_CURSOR_KBD_START     0u
    #define SP_ID_CURSOR_KBD_LEN       4u
    #define SP_ID_CURSOR_BOARD_START   (SP_ID_CURSOR_KBD_START + SP_ID_CURSOR_KBD_LEN)
    #define SP_ID_CURSOR_BOARD_LEN     4u
    #define SP_ID_CURSOR_LETTER_START  (SP_ID_CURSOR_BOARD_START + SP_ID_CURSOR_BOARD_LEN)
    #define SP_ID_CURSOR_LETTER_LEN    4u
    #define SP_ID_CURSOR_MENU_START    (SP_ID_CURSOR_LETTER_START + SP_ID_CURSOR_LETTER_LEN)
    #define SP_ID_CURSOR_MENU_LEN      2u
#define SP_ID_CURSORS_COUNT_TOTAL      ((SP_ID_CURSOR_MENU_START + SP_ID_CURSOR_MENU_LEN) - SP_ID_CURSORS_START)

// Used for hiding all sprites
#define SP_ID_COUNT_TOTAL        (SP_ID_CURSORS_COUNT_TOTAL)


// TODO: could move to keybaord.h
#define KEYBD_START_X     0u // Starting Tile Col of Keyboard
#define KEYBD_START_Y    14u // Starting Tile Row of Keyboard


// TODO: COUNT needs to be BOARD SIZE IN TILES (5 * 6 * 4)
#define BOARD_LETTERS_BYTES_PER_TILE  8u // 1bpp tiles = 8 bytes per 8x8 tile
#define BOARD_LETTERS_ALPHA           (26u + 1u) // +1 for Cedilla
#define BOARD_LETTERS_BLANK           1u
#define BOARD_LETTERS_FLIP            4u
#define BOARD_LETTERS_COUNT          (BOARD_LETTERS_ALPHA + BOARD_LETTERS_BLANK + BOARD_LETTERS_FLIP) // Last entry is blank space
#define BOARD_LETTERS_TILES_PER       4u // Last entry is blank space
// Empty space is last char in board font tile set
#define BOARD_LETTERS_SPACE_CHAR (BOARD_LETTERS_COUNT - (BOARD_LETTERS_BLANK + BOARD_LETTERS_FLIP))
#define BOARD_LETTERS_FLIP_1     (BOARD_LETTERS_COUNT - BOARD_LETTERS_FLIP)
#define BOARD_LETTERS_FLIP_2     (BOARD_LETTERS_FLIP_1 + 1)
#define BOARD_LETTERS_FLIP_3     (BOARD_LETTERS_FLIP_2 + 1)
#define BOARD_LETTERS_FLIP_4     (BOARD_LETTERS_FLIP_3 + 1)


#define BOARD_GRID_X  4u // Start x,y in Tiles
#define BOARD_GRID_Y  1u
#define BOARD_GRID_W  (WORD_LENGTH) // from common.h // 5u // Size in Tiles
#define BOARD_GRID_H  (MAX_GUESSES) // from common.h
#define BOARD_TILE_W  2u
#define BOARD_TILE_H  2u
#define BOARD_GRID_TILES_PER_LETTER 4u
#define BOARD_GRID_VRAM_BYTES_PER_LETTER (BOARD_LETTERS_BYTES_PER_TILE * BOARD_GRID_TILES_PER_LETTER)
#define BOARD_GRID_TILES_PER_ROW    (BOARD_GRID_W * BOARD_GRID_TILES_PER_LETTER)
#define BOARD_GRID_TILE_W (BOARD_GRID_W * BOARD_TILE_W)
#define BOARD_GRID_TILE_H (BOARD_GRID_H * BOARD_TILE_H)

#define BOARD_TILE_X_START (BOARD_GRID_X)
#define BOARD_TILE_Y_START (BOARD_GRID_Y)


// ==== VRAM tile assignment for Gameplay Screen ====
#define BG_TILES_BLANK_START 0u
#define BG_TILES_BLANK_LEN   1u

#define BG_TILES_KEYBD_START  (BG_TILES_BLANK_START + BG_TILES_BLANK_LEN)
#define BG_TILES_KEYBD_LEN    (26u + 1u) // 26 letters + 1 for cedilla

#define BG_TILES_BOARD_LETTERS_START (BG_TILES_KEYBD_START + BG_TILES_KEYBD_LEN)
#define BG_TILES_BOARD_LETTERS_LEN   (BOARD_GRID_W * BOARD_GRID_H * BOARD_GRID_TILES_PER_LETTER)

#define BG_TILES_INTRO_DIALOG_START (BG_TILES_BOARD_LETTERS_START + BG_TILES_BOARD_LETTERS_LEN)
#define BG_TILES_INTRO_DIALOG_LEN   8u

// ==== FONT TILES ====

#define BG_TILES_FONT_START   (BG_TILES_INTRO_DIALOG_START + BG_TILES_INTRO_DIALOG_LEN)

        #define BG_TILES_FONT_CEDILLA (BG_TILES_FONT_START + 26u) // This is counted in the letters, but here for printing convenience

        #define BG_TILES_FONT_PERIOD  (BG_TILES_FONT_START + 27u)
        #define BG_TILES_FONT_EXCLAIM (BG_TILES_FONT_START + 28u)
        #define BG_TILES_FONT_QMARK   (BG_TILES_FONT_START + 29u)
        #define BG_TILES_FONT_COLON   (BG_TILES_FONT_START + 30u)
        #define BG_TILES_FONT_PLUS    (BG_TILES_FONT_START + 31u)
        #define BG_TILES_FONT_QUOTE   (BG_TILES_FONT_START + 32u)

        // Gamepad Buttons
        // #define BG_TILES_FONT_BUTTON_A (BG_TILES_FONT_START + 29u)
        // #define BG_TILES_FONT_BUTTON_B (BG_TILES_FONT_START + 30u)
        #define BG_TILES_FONT_BUTTON_U (BG_TILES_FONT_START + 33u) // '^' char
        #define BG_TILES_FONT_BUTTON_D (BG_TILES_FONT_START + 34u) // '/' char
        #define BG_TILES_FONT_BUTTON_L (BG_TILES_FONT_START + 35u) // '<' char
        #define BG_TILES_FONT_BUTTON_R (BG_TILES_FONT_START + 36u) // '>' char

        #define BG_TILES_FONT_CHECKBOX_OFF (BG_TILES_FONT_START + 37u) // not str printable
        #define BG_TILES_FONT_CHECKBOX_ON  (BG_TILES_FONT_START + 38u) // not str printable

    #define BG_TILES_LAST     (BG_TILES_FONT_CHECKBOX_ON)

#define BG_TILES_FONT_LEN     ((BG_TILES_LAST - BG_TILES_FONT_START) + 1u)

#define FONT_LETTERS_COUNT           (BG_TILES_FONT_LEN)
#define FONT_LETTERS_BYTES_PER_TILE   8u // 1bpp tiles = 8 bytes per 8x8 tile


#define BG_TILES_FONT_NUM_START   (BG_TILES_FONT_START + BG_TILES_FONT_LEN)
#define BG_TILES_FONT_NUM_LEN     10u // 10 digits

#define BG_TILES_HELP_LEGEND_START        (BG_TILES_FONT_NUM_START + BG_TILES_FONT_NUM_LEN)
#define BG_TILES_HELP_LEGEND_NUM_LETTERS  3 // No Match, Wrong Place, Matched
#define BG_TILES_HELP_LEGEND_START_LEN    (HELP_LEGEND_NUM_LETTERS * (BOARD_GRID_TILES_PER_LETTER))
#define BG_TILES_HELP_LEGEND_START_LAST   ((BG_TILES_HELP_LEGEND_START + BG_TILES_HELP_LEGEND_START_LEN) - 1)



#define DIALOG_TILE_UL   0u
#define DIALOG_TILE_TOP  1u
#define DIALOG_TILE_UR   2u
#define DIALOG_TILE_L    3u
#define DIALOG_TILE_R    6u



/*
// Board Letter box colors: Foreground, Background
// Standard color scheme
// Printing
#define SET_PRINT_COLOR_NORMAL       set_1bpp_colors(DMG_BLACK, DMG_WHITE)     // Full contrast text
// Board
#define SET_BOARD_COLOR_NORMAL       set_1bpp_colors(DMG_BLACK, DMG_WHITE)     // Full contrast text
#define SET_BOARD_COLOR_CONTAINS     set_1bpp_colors(DMG_BLACK, DMG_LITE_GRAY) // Filled in box text
#define SET_BOARD_COLOR_MATCHED      set_1bpp_colors(DMG_WHITE, DMG_BLACK)     // Inverted text
// #define SET_BOARD_COLOR_NOT_IN_WORD  set_1bpp_colors(DMG_LITE_GRAY, DMG_WHITE)  // Inverted text
// Keyboard
#define SET_KEYBD_COLOR_NORMAL       set_1bpp_colors(DMG_BLACK, DMG_WHITE)  // Faded text
#define SET_KEYBD_COLOR_CONTAINS     set_1bpp_colors(DMG_BLACK, DMG_LITE_GRAY)      // Full contrast text
#define SET_KEYBD_COLOR_MATCHED      set_1bpp_colors(DMG_WHITE, DMG_BLACK)  // Inverted text
#define SET_KEYBD_COLOR_NOT_IN_WORD  set_1bpp_colors(DMG_LITE_GRAY, DMG_WHITE)  // Inverted text
*/

/*
// Board Letter box colors: Foreground, Background
// Alt color scheme #1 (lighter board, less inversion)
// Printing
#define SET_PRINT_COLOR_NORMAL       set_1bpp_colors(DMG_BLACK, DMG_WHITE)     // Full contrast text
// Board
#define SET_BOARD_COLOR_NORMAL       set_1bpp_colors(DMG_LITE_GRAY, DMG_WHITE)  // Faded text
#define SET_BOARD_COLOR_CONTAINS     set_1bpp_colors(DMG_BLACK, DMG_WHITE)      // Full contrast text
#define SET_BOARD_COLOR_MATCHED      set_1bpp_colors(DMG_BLACK, DMG_LITE_GRAY)  // Inverted text
// #define SET_BOARD_COLOR_NOT_IN_WORD  set_1bpp_colors(DMG_WHITE, DMG_LITE_GRAY)  // Inverted text
// Keyboard
#define SET_KEYBD_COLOR_NORMAL       set_1bpp_colors(DMG_LITE_GRAY, DMG_WHITE)  // Faded text
#define SET_KEYBD_COLOR_CONTAINS     set_1bpp_colors(DMG_BLACK, DMG_WHITE)      // Full contrast text
#define SET_KEYBD_COLOR_MATCHED      set_1bpp_colors(DMG_BLACK, DMG_LITE_GRAY)  // Inverted text
#define SET_KEYBD_COLOR_NOT_IN_WORD  set_1bpp_colors(DMG_WHITE, DMG_LITE_GRAY)  // Inverted text
*/

// Board Letter box colors: Foreground, Background
// Alt color scheme #2 (lighter board, less inversion)
// Printing
#define SET_PRINT_COLOR_NORMAL       set_1bpp_colors(DMG_BLACK, DMG_WHITE)     // Full contrast text

// Board
#define BOARD_DMG_COLOR_NOT_IN_WORD  DMG_LITE_GRAY, DMG_WHITE
#define BOARD_DMG_COLOR_CONTAINS     DMG_WHITE,     DMG_DARK_GRAY
#define BOARD_DMG_COLOR_MATCHED      DMG_BLACK,     DMG_WHITE

#define BOARD_DMG_COLOR_BLANK        DMG_LITE_GRAY, DMG_WHITE
#define BOARD_DMG_COLOR_ENTRY        DMG_BLACK,     DMG_WHITE

    // Board
    #define SET_BOARD_COLOR_BLANK        set_1bpp_colors(BOARD_DMG_COLOR_BLANK)
    #define SET_BOARD_COLOR_ENTRY        set_1bpp_colors(BOARD_DMG_COLOR_ENTRY)
    #define SET_BOARD_COLOR_NOT_IN_WORD  set_1bpp_colors(BOARD_DMG_COLOR_NOT_IN_WORD)
    #define SET_BOARD_COLOR_CONTAINS     set_1bpp_colors(BOARD_DMG_COLOR_CONTAINS)
    #define SET_BOARD_COLOR_MATCHED      set_1bpp_colors(BOARD_DMG_COLOR_MATCHED)

    // CGB does all colorizing with palettes instead
    // Uses same color as DMG ENTRY mode
    #define SET_BOARD_COLOR_FOR_CGB      set_1bpp_colors(BOARD_DMG_COLOR_ENTRY)

// Keyboard
#define KEYBD_COLOR_NORMAL       DMG_BLACK, DMG_WHITE
#define KEYBD_COLOR_CONTAINS     DMG_WHITE, DMG_LITE_GRAY
#define KEYBD_COLOR_MATCHED      DMG_WHITE, DMG_BLACK
#define KEYBD_COLOR_NOT_IN_WORD  DMG_LITE_GRAY, DMG_WHITE

    #define SET_KEYBD_COLOR_NORMAL       set_1bpp_colors(KEYBD_COLOR_NORMAL)
    #define SET_KEYBD_COLOR_CONTAINS     set_1bpp_colors(KEYBD_COLOR_CONTAINS)
    #define SET_KEYBD_COLOR_MATCHED      set_1bpp_colors(KEYBD_COLOR_MATCHED)
    #define SET_KEYBD_COLOR_NOT_IN_WORD  set_1bpp_colors(KEYBD_COLOR_NOT_IN_WORD)

// SGB uses a slightly different keyboard coloring
// since its dark grey color (orange) has more contrast
// versus black
#define KEYBD_COLOR_NORMAL_SGB       DMG_BLACK, DMG_WHITE
#define KEYBD_COLOR_CONTAINS_SGB     DMG_WHITE, DMG_DARK_GRAY // <-- Main difference versus DMG
#define KEYBD_COLOR_MATCHED_SGB      DMG_WHITE, DMG_BLACK
#define KEYBD_COLOR_NOT_IN_WORD_SGB  DMG_LITE_GRAY, DMG_WHITE



/*
// Board Letter box colors: Foreground, Background
// Alt color scheme #3 (lighter board, less inversion)
// Printing
#define SET_PRINT_COLOR_NORMAL       set_1bpp_colors(DMG_BLACK, DMG_WHITE)     // Full contrast text
// Board
#define SET_BOARD_COLOR_NORMAL       set_1bpp_colors(DMG_BLACK, DMG_WHITE)  // Faded text
#define SET_BOARD_COLOR_CONTAINS     set_1bpp_colors(DMG_WHITE, DMG_LITE_GRAY)      // Full contrast text
#define SET_BOARD_COLOR_MATCHED      set_1bpp_colors(DMG_WHITE, DMG_BLACK)  // Inverted text
// #define SET_BOARD_COLOR_MATCHED      set_1bpp_colors(DMG_WHITE, DMG_BLACK)  // Inverted text
// #define SET_BOARD_COLOR_NOT_IN_WORD  set_1bpp_colors(DMG_WHITE, DMG_LITE_GRAY)  // Inverted text
// Keyboard
#define SET_KEYBD_COLOR_NORMAL       set_1bpp_colors(DMG_BLACK, DMG_WHITE)  // Faded text
#define SET_KEYBD_COLOR_CONTAINS     set_1bpp_colors(DMG_WHITE, DMG_LITE_GRAY)      // Full contrast text
//#define SET_KEYBD_COLOR_MATCHED      set_1bpp_colors(DMG_WHITE, DMG_BLACK)  // Inverted text
#define SET_KEYBD_COLOR_MATCHED      set_1bpp_colors(DMG_WHITE, DMG_BLACK)  // Inverted text
#define SET_KEYBD_COLOR_NOT_IN_WORD  set_1bpp_colors(DMG_LITE_GRAY, DMG_WHITE)  // Inverted text

*/


#endif