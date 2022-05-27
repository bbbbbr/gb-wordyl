// sfx_common.c

#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "settings.h"

#include "sfx_common.h"

// SFX_list array should be in identical order
#include "sfx/tile_add_sfx.h"      // SFX_0A[];
#include "sfx/tile_remove_sfx.h"   // SFX_0B[];
#include "sfx/word_reveal_sfx.h"   // SFX_11[];
#include "sfx/cursor_move_sfx.h"   // SFX_0C[];
#include "sfx/game_won_sfx.h"      // SFX_0E[];
#include "sfx/game_lost_sfx.h"     // SFX_0D[];
#include "sfx/exit_splash_sfx.h"   // SFX_10[];
#include "sfx/action_change_sfx.h" // SFX_0F[];


const unsigned char * SFX_list[] = {
    &SFX_0A[0], // tile_add_sfx.h
    &SFX_0B[0], // tile_remove_sfx.h
    &SFX_11[0], // word_reveal_sfx.h
    &SFX_0C[0], // cursor_move_sfx.h
    &SFX_0E[0], // game_won_sfx.h
    &SFX_0D[0], // game_lost_sfx.h
    &SFX_10[0], // exit_splash_sfx.h
    &SFX_0F[0] // action_change_sfx.h
};


void play_sfx(uint8_t sfx_num) {

    if (game_settings.opt_sound_fx_enabled == true)
        CBTFX_init(SFX_list[(sfx_num)]);
}