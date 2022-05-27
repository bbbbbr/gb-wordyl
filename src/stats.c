// stats.c

#include <gbdk/platform.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "window.h"
#include <lang_text.h>

#include "stats.h"

#include <cartsave.h>


// Zeros out stats and if supported saves the change
void stats_reset(void) {

    game_settings.save_check0 = STATS_SIG_CHECK_0;
    game_settings.save_check1 = STATS_SIG_CHECK_1;

    game_settings.games_count = 0;
    game_settings.games_won = 0;
    game_settings.games_lost = 0;
    game_settings.games_streak = 0;

    game_settings.guesses_min = MAX_GUESSES;
    game_settings.guesses_max = 0;
    game_settings.guesses_sum = 0;

    // For relevant carts, save the reset stats
    #if defined(CART_31k_1kflash) || defined(CART_mbc5)
        cartsave_save_data();
    #endif
}


// Called after end of a game round and before stats_show
void stats_update(bool game_won, uint8_t guess_num) {

    game_settings.games_count++;

    if (game_won) {
        game_settings.games_won++;
        game_settings.games_streak++;

        if (guess_num < game_settings.guesses_min)
            game_settings.guesses_min = guess_num;
        if (guess_num > game_settings.guesses_max)
            game_settings.guesses_max = guess_num;

        game_settings.guesses_sum += guess_num;
    }
    else {
        game_settings.games_lost++;
        game_settings.games_streak = 0;
    }

    // For relevant carts, save the reset stats
    #if defined(CART_31k_1kflash) || defined(CART_mbc5)
        cartsave_save_data();
    #endif
}


const uint8_t stats_str_template[] = __STATS_TEMPLATE_STR;
uint8_t stats_str[sizeof(stats_str_template)];

// Display current stats in a popup window at end of a game round
void stats_show(void) {

    // Reset display template
    // memcpy(stats_str, stats_str_template, sizeof(stats_str_template));
    // About 1/2 the size of including memcpy()
    uint8_t * p_dest = stats_str;
    const uint8_t * p_src = stats_str_template;
    while (p_src != stats_str_template + sizeof(stats_str_template)) {
        *p_dest++ = *p_src++;
    }


    str_u16_left_at_X(stats_str, game_settings.games_count);
    str_u16_left_at_X(stats_str, game_settings.games_streak);
    str_u16_left_at_X(stats_str, game_settings.games_won);
    str_u16_left_at_X(stats_str, game_settings.games_lost);

    str_u16_left_at_X(stats_str, game_settings.guesses_min);
    str_u16_left_at_X(stats_str, game_settings.guesses_max);
    str_u16_left_at_X(stats_str, game_settings.guesses_sum / game_settings.games_won);

    win_dialog_show_message(STATS_DIALOG_WIN_Y, stats_str, NULL);
}

