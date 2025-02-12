/* Word game for Game Boy.
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

#if (defined(GAMEBOY) || defined(ANALOGUEPOCKET))
    #include "sgb/sgb_border.h"
#endif
#if (defined(MEGADUCK))
    #include "megaduck_laptop/megaduck_laptop_io.h"
    #include "megaduck_laptop/megaduck_model.h"
#endif

#include "decode.h"

#include "help_info.h"

#include "intro_splash.h"
#include "board.h"
#include "gameplay.h"
#include "stats.h"

#ifdef TEST_DICT_ENABLED
    #include "decode.h"
#endif

#if defined(CART_31k_1kflash)
    #include "cart_31k_1kflash/logo_splash_ferrante_skullcat.h"
#elif defined(CART_md0_sram)
    volatile uint8_t __at (0x1000) rMD0_BANK_REG;
    #define SWITCH_BANKS_ROM_RAM_MD0(ROMB, RAMB) (rMD0_BANK_REG = (RAMB << 4) | ROMB)
#endif

bool is_first_run = true;

fixed rand_seed = {.w = 0x0000u};


void main(void) {

    #if defined(CART_mbc5)
        // Initialize MBC bank defaults
        // Upper ROM bank to 1, And SRAM/XRAM bank to 0
        SWITCH_ROM_MBC5(1);
        SWITCH_RAM(0);
        DISABLE_RAM_MBC5;
    #elif defined(CART_md0_sram)
        // Make sure MD0 ROM and SRAM banks are both set to 0, only those are used
        // There is no (known) enable/disable for the SRAM, it's just always on
        SWITCH_BANKS_ROM_RAM_MD0(0,0);
    #endif

    #if defined(GAMEBOY) || defined(ANALOGUEPOCKET)
    // Call before gfx_load() since it will overwrite other tile data
    sgb_border_try_loading();
    #endif

    #if defined(MEGADUCK)
        megaduck_laptop_check_model_vram_on_startup();  // This must be called before any vram tiles are loaded
        megaduck_laptop_detected = megaduck_laptop_init();
    #else
        megaduck_laptop_detected = false;
    #endif

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
    NR52_REG = AUDENA_ON;
    NR51_REG = AUDTERM_ALL_LEFT | AUDTERM_ALL_RIGHT;
    NR50_REG = 0x77u;


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

                    // Note: This needs to get called AFTER the flag has been
                    //       loaded by settings_load() (in main())
                    if (game_settings.opt_help_message_on_startup)
                        help_info_show();
                    else
                        stats_show();

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
