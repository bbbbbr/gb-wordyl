// gameplay.c

#include <gbdk/platform.h>
#include <gb/gbdecompress.h>

#include <stdint.h>
#include <stdbool.h>
#include <rand.h>

#include "common.h"
#include "input.h"
#include "gfx_common.h"
#include "sfx_common.h"
#include "gameboy_color.h"
#include "fade.h"

#include "board.h"
#include "intro_splash.h"

#include <lang_text.h>

#if (defined(MEGADUCK))
    #include "megaduck_laptop/megaduck_keyboard.h"
#endif


void splash_run(void) {

    DMG_PAL_INVERT_ON;

    // == INITIAL CREDITS SCREEN ==
    gfx_clear_bkg_blank_tile();
    fade_in();

    print_gotoxy(__INTRO_CREDITS_X, __INTRO_CREDITS_Y, PRINT_BKG);
    print_str(__INTRO_CREDITS_STR);
    if (IS_CGB) delay(4000);
    else        delay(2000);
    fade_out();


    // == MAIN SPLASH SCREEN ==

    // Scroll up one tile row to move the board down a little
    move_bkg(0, (uint8_t)-8); // TODO: macro?

    // Change board rendering layout to splash screen style
    // See: board_row_ranges_splash[] in board.c
    BOARD_SET_LAYOUT_SPLASH;
    splash_init_maps();

    // Use an inverted pallette for splash screen
    fade_in();

    splash_animate_title();

    // Display "Press Start" after intro tile flip is complete or is skipped
    print_gotoxy((DEVICE_SCREEN_WIDTH - (sizeof(__INTRO_PRESS_START_STR) - 1)) / 2u, DEVICE_SCREEN_HEIGHT - 4u, PRINT_BKG);
    print_str(__INTRO_PRESS_START_STR);

    waitpadticked_lowcpu(J_START);

    // Don't use play_sfx() since that checks if sound is enabled
    // 1) Don't want to gate by that, 2) options haven't yet been loaded from Flash ROM / SRAM
    CBTFX_init(SFX_list[(SFX_EXIT_SPLASH)]);

    // A little delay for the sfx to play before fade-out.
    // Flash the text so that it doesn't just look stuck if volume is off
    for (uint8_t c = 0; c < 5; c++) {
        if (IS_CGB) delay(200);
        else        delay(100);

        print_gotoxy((DEVICE_SCREEN_WIDTH - (sizeof(__INTRO_PRESS_START_STR) - 1)) / 2u, DEVICE_SCREEN_HEIGHT - 4u, PRINT_BKG);
        if (c & 0x01)
            print_str(__INTRO_PRESS_START_STR);
        else
            print_str(__INTRO_PRESS_START_STR_BLANK);
    }


    fade_out();
    // Revert to normal palette
    DMG_PAL_INVERT_OFF;

    move_bkg(252, 0); // TODO: macro
}


// Run once on startup to prepare gameplay board graphics
void splash_init_maps(void) {

    // IMPORTANT: Expects dialog border map to already be
    // decompressed and ready
    //     gb_decompress(intro_dialog_map, map_decomp_buf);
    set_bkg_based_tiles(0,31, DEVICE_SCREEN_WIDTH, DEVICE_SCREEN_HEIGHT, map_decomp_buf, BG_TILES_INTRO_DIALOG_START);

    // Decompress stars map
    gb_decompress(intro_stars_map, map_decomp_buf);
    set_bkg_based_tiles(1,0,  DEVICE_SCREEN_WIDTH - 2, DEVICE_SCREEN_HEIGHT - 1, map_decomp_buf, BG_TILES_INTRO_DIALOG_START);

    // Set up Board Letter map in VRAM
    // (direct addressable for rewriting letters via changing tile contents)
    board_map_fill();
    // set_bkg_based_tiles(BOARD_TILE_X_START, BOARD_TILE_Y_START,
    //                     BOARD_GRID_TILE_W, BOARD_GRID_TILE_H,
    //                     board_map,
    //                     BG_TILES_BOARD_LETTERS_START);


    // IMPORTANT: Needs to happen before board_redraw_clean() so it doesn't wipe out those attributes
    if (IS_CGB)
        splash_initgfx_cgb();

    board_redraw_clean();


    // print_gotoxy((DEVICE_SCREEN_WIDTH - (sizeof("BBBBBR 2022") - 1)) / 2u, DEVICE_SCREEN_HEIGHT - 2u, PRINT_BKG);
    print_gotoxy(2u, DEVICE_SCREEN_HEIGHT - 2u, PRINT_BKG);
    print_str("BBBBBR");
    print_gotoxy(DEVICE_SCREEN_WIDTH - 5u - 1u, DEVICE_SCREEN_HEIGHT - 2u, PRINT_BKG);
    print_str("2025");
}

#define CHR_NUM(c) (c - 'A')

// 31k + 1k cart release gets separate naming for GB port
#if defined(CART_31k_1kflash)
    #ifdef MEGADUCK
        // "DUCK DUCK WORDYL";
        const uint8_t splash_text[] = {
                CHR_NUM('D'),
                CHR_NUM('U'),
                CHR_NUM('C'),
                CHR_NUM('K'),
                CHR_NUM('D'),
                CHR_NUM('U'),
                CHR_NUM('C'),
                CHR_NUM('K'),
                BOARD_LETTERS_SPACE_CHAR,
                CHR_NUM('W'),
                CHR_NUM('O'),
                CHR_NUM('R'),
                CHR_NUM('D'),
                BOARD_LETTERS_SPACE_CHAR,
                BOARD_LETTERS_SPACE_CHAR,
                BOARD_LETTERS_SPACE_CHAR,
                CHR_NUM('Y'),
                CHR_NUM('L'),};

        const uint8_t splash_text_color[] = {
            CGB_PAL_GREEN, // GAME
            CGB_PAL_GREEN,
            CGB_PAL_GREEN,
            CGB_PAL_GREEN,
            CGB_PAL_GREEN,
            CGB_PAL_GREEN, // DUCK
            CGB_PAL_GREEN,
            CGB_PAL_GREEN,

            CGB_PAL_WHITE, // SPACE

            CGB_PAL_BLUE, // WORD
            CGB_PAL_BLUE,
            CGB_PAL_BLUE,
            CGB_PAL_BLUE,

            CGB_PAL_WHITE, // SPACE
            CGB_PAL_WHITE, // SPACE
            CGB_PAL_WHITE, // SPACE

            CGB_PAL_BLUE,  // YL
            CGB_PAL_BLUE};
    #else
        // "  GBBOY WORDYL"; (For Ferrante Crafts 31k+1k release)
        const uint8_t splash_text[] = {
                BOARD_LETTERS_SPACE_CHAR,
                CHR_NUM('G'),
                CHR_NUM('B'),
                BOARD_LETTERS_SPACE_CHAR,

                BOARD_LETTERS_SPACE_CHAR,
                BOARD_LETTERS_SPACE_CHAR,
                BOARD_LETTERS_SPACE_CHAR,
                // CHR_NUM('B'),
                // CHR_NUM('O'),
                // CHR_NUM('Y'),
                BOARD_LETTERS_SPACE_CHAR,
                CHR_NUM('W'),
                CHR_NUM('O'),
                CHR_NUM('R'),
                CHR_NUM('D'),
                BOARD_LETTERS_SPACE_CHAR,
                BOARD_LETTERS_SPACE_CHAR,
                BOARD_LETTERS_SPACE_CHAR,
                CHR_NUM('Y'),
                CHR_NUM('L'),};

        const uint8_t splash_text_color[] = {
            CGB_PAL_WHITE, // SPACE
            CGB_PAL_GREEN, // GB
            CGB_PAL_GREEN,
            CGB_PAL_WHITE, // SPACE

            CGB_PAL_WHITE, // SPACE
            CGB_PAL_WHITE, // SPACE
            CGB_PAL_WHITE, // SPACE
            // CGB_PAL_GREEN, // BOY
            // CGB_PAL_GREEN,
            // CGB_PAL_GREEN,
            CGB_PAL_WHITE, // SPACE

            CGB_PAL_BLUE, // WORD
            CGB_PAL_BLUE,
            CGB_PAL_BLUE,
            CGB_PAL_BLUE,

            CGB_PAL_WHITE, // SPACE
            CGB_PAL_WHITE, // SPACE
            CGB_PAL_WHITE, // SPACE

            CGB_PAL_BLUE,  // YL
            CGB_PAL_BLUE};
    #endif
#else
    #ifdef MEGADUCK
        // "DUCK DUCK WORDYL";
        const uint8_t splash_text[] = {
                CHR_NUM('D'),
                CHR_NUM('U'),
                CHR_NUM('C'),
                CHR_NUM('K'),
                CHR_NUM('D'),
                CHR_NUM('U'),
                CHR_NUM('C'),
                CHR_NUM('K'),
                BOARD_LETTERS_SPACE_CHAR,
                CHR_NUM('W'),
                CHR_NUM('O'),
                CHR_NUM('R'),
                CHR_NUM('D'),
                BOARD_LETTERS_SPACE_CHAR,
                BOARD_LETTERS_SPACE_CHAR,
                BOARD_LETTERS_SPACE_CHAR,
                CHR_NUM('Y'),
                CHR_NUM('L'),};

        const uint8_t splash_text_color[] = {
            CGB_PAL_GREEN, // GAME
            CGB_PAL_GREEN,
            CGB_PAL_GREEN,
            CGB_PAL_GREEN,
            CGB_PAL_GREEN,
            CGB_PAL_GREEN, // DUCK
            CGB_PAL_GREEN,
            CGB_PAL_GREEN,

            CGB_PAL_WHITE, // SPACE

            CGB_PAL_BLUE, // WORD
            CGB_PAL_BLUE,
            CGB_PAL_BLUE,
            CGB_PAL_BLUE,

            CGB_PAL_WHITE, // SPACE
            CGB_PAL_WHITE, // SPACE
            CGB_PAL_WHITE, // SPACE

            CGB_PAL_BLUE,  // YL
            CGB_PAL_BLUE};
    #else
        // "GAMEBOY WORDYL";
        const uint8_t splash_text[] = {
                CHR_NUM('G'),
                CHR_NUM('A'),
                CHR_NUM('M'),
                CHR_NUM('E'),
                CHR_NUM('B'),
                CHR_NUM('O'),
                CHR_NUM('Y'),
                BOARD_LETTERS_SPACE_CHAR,
                CHR_NUM('W'),
                CHR_NUM('O'),
                CHR_NUM('R'),
                CHR_NUM('D'),
                BOARD_LETTERS_SPACE_CHAR,
                BOARD_LETTERS_SPACE_CHAR,
                BOARD_LETTERS_SPACE_CHAR,
                CHR_NUM('Y'),
                CHR_NUM('L'),};

        const uint8_t splash_text_color[] = {
            CGB_PAL_GREEN, // GAME
            CGB_PAL_GREEN,
            CGB_PAL_GREEN,
            CGB_PAL_GREEN,
            CGB_PAL_GREEN, // BOY
            CGB_PAL_GREEN,
            CGB_PAL_GREEN,

            CGB_PAL_WHITE, // SPACE

            CGB_PAL_BLUE, // WORD
            CGB_PAL_BLUE,
            CGB_PAL_BLUE,
            CGB_PAL_BLUE,

            CGB_PAL_WHITE, // SPACE
            CGB_PAL_WHITE, // SPACE
            CGB_PAL_WHITE, // SPACE

            CGB_PAL_BLUE,  // YL
            CGB_PAL_BLUE};
    #endif
#endif


void splash_animate_title(void) {

    uint8_t row = 0;
    uint8_t col = 0;
    bool skip_anim = false;
    bool keyboard_press_occurred = false;

    // BOARD_SET_FLIP_SPEED(BOARD_TILE_FLIP_FAST);
    BOARD_SET_FLIP_SPEED(BOARD_TILE_FLIP_SLOW);

    for (uint8_t c = 0; c < ARRAY_LEN(splash_text); c++) {

        // Allow user to slip through animation
        UPDATE_KEYS();
        if (KEY_TICKED(J_ANY_KEY)) {
            skip_anim = true;
            // Don't use play_sfx() since that checks if sound is enabled
            // 1) Don't want to gate by that, 2) options haven't yet been loaded from Flash ROM / SRAM
            CBTFX_init(SFX_list[(SFX_TILE_REVEAL_RESULT)]);
        }

        #if defined(MEGADUCK)
            // Poll for keyboard keys every other frame
            // (Polling intervals below 20ms may cause keyboard lockup)
            // The usual (sys_time & 0x01u) doesn't work here due to some of the loop timing
            // so use 2 x vsync to ensure enough time has elapsed instead
            vsync();
            vsync();
            if (megaduck_laptop_detected) {

                if (megaduck_keyboard_poll_keys()) {
                    megaduck_keyboard_process_keys();

                    // Prevent passing through any key press by flagging the press
                    // and then returning once no keys are pressed
                    if (megaduck_key_pressed) {
                        skip_anim = true;
                        // Don't use play_sfx() since that checks if sound is enabled
                        // 1) Don't want to gate by that, 2) options haven't yet been loaded from Flash ROM / SRAM
                        CBTFX_init(SFX_list[(SFX_TILE_REVEAL_RESULT)]);
                    }
                }
            }
        #endif

        // Don't show letter flip animation for space chars
        // and don't change their board tile color
        if (splash_text[c] != BOARD_LETTERS_SPACE_CHAR) {
            if (!skip_anim)
                board_draw_tile_flip_anim(row, col);

            if (IS_CGB) {
                SET_BOARD_COLOR_FOR_CGB;
                board_fill_letter_cgb_pal(row, col, splash_text_color[c]);
            } else {
                SET_BOARD_COLOR_MATCHED;
            }

            board_draw_letter_bits(row, col, splash_text[c]);
        }

        // Simulate transition between board rows as
        // letters increment, to re-use board drawing logic
        row++;
        if (row = BOARD_GRID_W) {
            row = 0;
            col++;
        }
    }

}
