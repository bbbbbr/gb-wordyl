/* Wordle clone for Game Boy.
 * A weekend project by stacksmashing.
 * Contact: twitter.com/ghidraninja
 *          code@stacksmashing.net
 *
 * Dictionary improvements by zeta_two
 * Further dictionary improvements and highlighting fixes by arpruss
 * Major rework by bbbbbr
 */

#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>
#include <rand.h>

#include "common.h"
#include "settings.h"
#include "input.h"
#include "gfx_common.h"
#include "sfx_common.h"
#include "gameboy_color.h"
#include "fade.h"

#include "sgb_border.h"

#include "decode.h"

#include "intro_splash.h"
#include "board.h"
#include "gameplay.h"
#include "stats.h"

#ifdef TEST_DICT_ENABLED
    #include "decode.h"
#endif

#if defined(CART_31k_1kflash)
    #include "cart_31k_1kflash/logo_splash_ferrante_skullcat.h"
#endif

bool is_first_run = true;

fixed rand_seed = {.w = 0x0000u};


void main() {

    // Call before gfx_load() since it will overwrite other tile data
    sgb_border_try_loading();

    fade_out();
    move_win(0 + WIN_X_OFFSET, DEVICE_SCREEN_PX_HEIGHT); // Window is offscreen by default
    SHOW_WIN;
    SHOW_BKG;
    DISPLAY_ON;

    cgb_check_and_init();

    #if defined(CART_31k_1kflash)
        // Call before gfx_load() since it will overwrite other tile data
        logo_splash_ferrante_skullcat_show();
    #endif

    gfx_load();

    initDict();
    game_state = GAME_STATE_INTRO;

    // Enable audio output
    NR52_REG = 0x80;
    NR51_REG = 0xFF;
    NR50_REG = 0x77;


    __critical {
        #ifdef CPTFX_UPDATE_ASM
                add_VBL(CBTFX_update_isr);
        #else
               add_VBL(CBTFX_update);
        #endif
    }

    while(1) {
        switch (game_state) {

            case GAME_STATE_INTRO:
                splash_run();
                // First half of random init (button press to exit splash)
                rand_seed.h = DIV_REG;
                // screen now: faded out
                BOARD_SET_LAYOUT_GAME;
                gameplay_init_maps();
                game_state = GAME_STATE_RESTART;
                break;

            case GAME_STATE_RESTART:
                gameplay_restart();
                fade_in();
                if (is_first_run) {
                    // Load or reset stats depending on cart type
                    // Call this before show_intro_message(), it uses a loaded flag
                    settings_load();

                    show_intro_message();
                    is_first_run = false;
                    // Second half of random init (after button press to exit welcome dialog)
                    rand_seed.l = DIV_REG;
                    initrand(rand_seed.w);

                    // Wait to turn on all sprites until after first popup intro message
                    SHOW_SPRITES;

                    #ifdef TEST_DICT_ENABLED
                        dumpTestToEmuConsole();
                    #endif
                }
                gameplay_init_answer_word();
                game_state = GAME_STATE_RUNNING;
                break;

            case GAME_STATE_RUNNING:
                gameplay_run();
                break;

            case GAME_STATE_OVER:
                fade_out();
                game_state = GAME_STATE_RESTART;
                break;
        }
    }
}
