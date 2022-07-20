// gameplay.h

#ifndef _GAMEPLAY_H
#define _GAMEPLAY_H

#define GAME_NOT_WON false
#define GAME_WAS_WON true


// Number of consecutive button presses to trigger menu
#define MENU_SELECT_COUNT_TRIGGER 3u
#define MENU_SELECT_COUNT_RESET   0u

void gameplay_init_answer_word(void);

void show_win_message(uint8_t guess_count);
void show_lose_message(char *correct_word);

void ask_forfeit_round(void);
void ask_stats_reset(void);

void gameplay_handle_gameover(bool);
void gameplay_handle_guess(void);

void gameplay_restore_sprites(void);

// void gameplay_init_turn_gfx_on(void);
void gameplay_init_maps(void);
void gameplay_restart(void);
void gameplay_run(void);

#endif
