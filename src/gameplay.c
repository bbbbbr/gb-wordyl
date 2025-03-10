// gameplay.c

#include <gbdk/platform.h>
#include <gbdk/incbin.h>
#include <gb/gbdecompress.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <rand.h>

#include "decode.h"

#include "common.h"
#include "input.h"
#include "gfx_common.h"
#include "sfx_common.h"
#include "gameboy_color.h"

#include "keyboard.h"
#include "board.h"
#include "window.h"
#include "stats.h"

#include "settings_menu.h"
#include "help_info.h"

#include "gameplay.h"

#include <lang_text.h>
#include <cartsave.h>

#if (defined(MEGADUCK))
    #include "megaduck_laptop/megaduck_keyboard.h"
#endif

#define GAMEPLAY_SET_GAMEOVER  game_state = GAME_STATE_OVER

// Should not be const, gets modified
uint8_t game_won_str[] = __MESSAGE_GAME_WON_STR;

// Show a popup message: You Won
void show_win_message(uint8_t guess_count) {

    play_sfx(SFX_GAME_WON);

    // Patch the number of guesses into the string at char '^'
    uint8_t * p_str = game_won_str;
    while (*p_str != '^') p_str++;
    *p_str = guess_count + '0';

    win_dialog_show_message(WON_MESSAGE_DIALOG_WIN_Y, game_won_str, NULL);

    // Restore X marker char
    *p_str = '^';
}


// Show a popup message: You Lost
void show_lose_message(char *correct_word) {

    play_sfx(SFX_GAME_LOST);
    win_dialog_show_message(LOSE_MESSAGE_DIALOG_WIN_Y, __MESSAGE_LOSE_STR, correct_word);
}


// Called from Menu
void ask_forfeit_round(void) {
    // Forfeit Round
    // sets: GAMEPLAY_SET_GAMEOVER
    if (win_confirm_dialog(__CONFIRM_FORFEIT_STR)) {
        gameplay_handle_gameover(GAME_NOT_WON);
    }
}


// Called from Menu
void ask_stats_reset(void) {
    // Reset Stats
    if (win_confirm_dialog(__CONFIRM_STATS_RESET_STR)) {
        stats_reset();
        win_dialog_show_message(STATS_RESET_DIALOG_WIN_Y, __MESSAGE_STATS_RESET_STR ,NULL);
    }
}


// Called if:
// - The guess matched the word  : game_was_won == true
// - The last guess was wrong    : game_was_won == false && guess_num == MAX_GUESSES
// - The user forfeited the game : game_was_won == false
// sets: GAMEPLAY_SET_GAMEOVER
void gameplay_handle_gameover(bool game_was_won) {

    // Prevent the cursor from flashing between popups
    // (The window dialog will handle hiding them)
    WIN_DIALOG_RESTORE_SPRITES_AFTER_NO;

    if (game_was_won)
        show_win_message(guess_num);
    else
        show_lose_message(word);

    stats_update(game_was_won, guess_num);
    stats_show();

    // Restore normal sprite hiding behavior
    WIN_DIALOG_RESTORE_SPRITES_AFTER_YES;

    // gameplay_restart() will handle restoring the sprites
    // at the start of the next round

    GAMEPLAY_SET_GAMEOVER;
}


// Process a player guess for a word (including winning and losing)
//
// * If the player won or lost, then game state
//   will be changed to game over, causing an exit
//   from the main gameplay loop
void gameplay_handle_guess(void) {

    if (strlen(guess) != WORD_LENGTH) {
        // Insufficient length
        win_dialog_show_message(WORD_TOO_SHORT_DIALOG_WIN_Y, __MESSAGE_WORD_TOO_SHORT_STR, NULL);
    }
    else if (!filterWord(guess)) {

        // Word not in dictionary
        win_dialog_show_message(WORD_NOT_IN_DICT_DIALOG_WIN_Y, __MESSAGE_WORD_NOT_IN_DICT_STR, NULL);
    }
    else {

        // Validate hard mode
        if ((game_settings.opt_hard_mode_enabled) && (guess_num > 0) && (evaluate_guess_hard_mode(guess) == false)) {
            win_dialog_show_message(HARD_MODE_GUESS_NOT_VALID_WIN_Y, __MESSAGE_HARD_MODE_GUESS_NOT_VALID_STR, NULL);
        }
        else {

            // Otherwise process the guess word
            //
            // If it's reached this point, the guess is a valid dictionary word
            // and has passed hard-mode criteria (if needed)

            board_draw_word(guess_num, guess, BOARD_HIGHLIGHT_YES);
            guess_num += 1;
            keyboard_update_from_guess();

            // == Handle Game Over scenarios ==

            // Check for correct match
            bool game_was_won = (strcmp(word, guess) == 0);

            if (game_was_won || (guess_num == MAX_GUESSES)) {
                gameplay_handle_gameover(game_was_won);
            } else {
                guess_letter_cursor = LETTER_CURSOR_START; // reset letter cursor to start of row
                board_update_row_cursor();
                board_update_letter_cursor();
            }

            // Store guess / Eval results, for hard mode
            copy_or_reset_prev_guess(guess);

            // Reset guess to empty and prepare for next guess
            for (uint8_t c = 0; c < WORD_LENGTH; c++)
                guess[c] = 0;

            board_reset_autofilled_flags();
            // If requested, try to auto-fill next guess if game is not over
            if ((game_settings.opt_autofill_enabled) && (game_state != GAME_STATE_OVER))
                board_autofill_matched_letters();
        }
    }
}


// Choose an answer word for a new gameplay round
// Modifies global: word
void gameplay_init_answer_word(void) {

    uint16_t wordnum;

    // Prevent having the same word two times in a row
    // (unlikely, but possible)
    do {
        wordnum = randw() % NUM_ANSWERS;
    } while (wordnum == wordnum_last);

    wordnum_last = wordnum;

    #ifdef DEBUG_FORCE_WORD_BY_NUM
        wordnum = DEBUG_FORCE_WORD_BY_NUM;
    #endif

    getSpecialWord(wordnum, word);

    #ifdef DEBUG_FORCE_WORD
        strcpy(word, DEBUG_FORCE_WORD);
    #endif

    #ifdef DEBUG_REVEAL_WORD
        // Cheat Mode: Display answer word in debug mode
        print_gotoxy(0,0, PRINT_BKG);
        print_str(word);
    #endif
}


// Run once on startup to prepare gameplay board graphics maps
//
// This DOES NOT draw the actual graphics, just lays out maps
// so they can be drawn using draw-via-tiles instead of draw-via-tile-ids
void gameplay_init_maps(void) {

    // == Setup the game board & keyboard ==

    // Clear screen
    gfx_clear_bkg_blank_tile();
    keyboard_draw_map();

    // Draw stars
    // IMPORTANT: relies on stars already being in decompressed map
    //     gb_decompress(intro_stars_map, map_decomp_buf);
    set_bkg_based_tiles(0,0,  DEVICE_SCREEN_WIDTH - 2, DEVICE_SCREEN_HEIGHT - KEYBD_ROWS -2 , map_decomp_buf, BG_TILES_INTRO_DIALOG_START);

    // Set up Board Letter map in VRAM
    // (direct addressable for rewriting letters via changing tile contents)
    board_map_fill();
    // set_bkg_based_tiles(BOARD_TILE_X_START, BOARD_TILE_Y_START,
    //                     BOARD_GRID_TILE_W, BOARD_GRID_TILE_H,
    //                     board_map,
    //                     BG_TILES_BOARD_LETTERS_START);

    // Set up colors for board area
    if (IS_CGB)
        board_initgfx_cgb();
}


// Called at start of a game round as well as when
// restoring sprites after showing a popup window
void gameplay_restore_sprites(void) {

    board_update_row_cursor();
    board_update_letter_cursor();
    // This is
    keyboard_update_cursor();
}


// Runs on startup and before start of a new gameplay round
void gameplay_restart(void) {

    // Reset round level vars
    guess_num = GUESS_NUM_RESET;
    guess_letter_cursor = LETTER_CURSOR_START;

    // Reset guess and exact match arrays
    for (uint8_t c = 0; c < WORD_LENGTH; c++) {
        guess[c] = 0;
        exact_matches[c] = 0;
    }
    board_reset_autofilled_flags();

    // Draws initial empty board and keyboard
    board_redraw_clean();

    // Resets keyboard and it's cursor
    keyboard_reset();

    gameplay_restore_sprites();
}


// Runs the main gameplay: word entry, answer checking, messaging
void gameplay_run(void)
{
    bool keys_select_consumed = false;
    uint8_t menu_select_count = 0;

    // make sure all keys are released before starting gameplay
    waitpadreleased_lowcpu(J_ANY_KEY);

    while(game_state == GAME_STATE_RUNNING) {
        vsync();


        #if defined(MEGADUCK)
            // Poll for keyboard keys every other frame
            // (Polling intervals below 20ms may cause keyboard lockup)
            if ((sys_time & 0x01u) && (megaduck_laptop_detected)) {
                if (duck_io_poll_keyboard(&keydata)) {
                    duck_io_process_key_data(&keydata, megaduck_model);

                    switch (key_pressed) {
                        case NO_KEY: break;

                        // case KEY_ARROW_UP:    keyboard_move_cursor(J_UP);    break;
                        // case KEY_ARROW_DOWN:  keyboard_move_cursor(J_DOWN);  break;
                        // case KEY_ARROW_LEFT:  keyboard_move_cursor(J_LEFT);  break;
                        // case KEY_ARROW_RIGHT: keyboard_move_cursor(J_RIGHT); break;
                        // case KEY_SPACE:       // TODO: use to add tile from on-screen keyboard?
                        case KEY_HELP:        settings_menu_show();              break;
                        case KEY_ENTER:       gameplay_handle_guess();           break;
                        case KEY_ESCAPE:      board_autofill_matched_letters();  break;
                        case KEY_ARROW_LEFT:  // Move Tile Cursor Left
                                              if (guess_letter_cursor > LETTER_CURSOR_START) {
                                                  guess_letter_cursor--;
                                                  play_sfx(SFX_CURSOR_MOVE);
                                                  board_update_letter_cursor();
                                              }
                                              break;

                        case KEY_ARROW_RIGHT: // Move Tile Cursor Left
                                              if (guess_letter_cursor < LETTER_CURSOR_MAX) {
                                                  guess_letter_cursor++;
                                                  play_sfx(SFX_CURSOR_MOVE);
                                                  board_update_letter_cursor();
                                              }
                                              break;


                        case KEY_DELETE:      // Fall through, same as backspace
                        case KEY_BACKSPACE:   // Remove tile and move cursor to left
                                              play_sfx(SFX_TILE_REMOVE);
                                              board_remove_guess_letter();
                                              board_update_letter_cursor();
                                              break;

                        default: // Try to add a letter from the keyboard
                                 // Force a-z lowercase to upper-case
                                 if ((key_pressed >= 'a') && (key_pressed <= 'z'))
                                        key_pressed -= ('a' - 'A');

                                 // Only allow A-Z
                                 if ( (key_pressed >= 'A') && (key_pressed <= 'Z')) {
                                    play_sfx(SFX_TILE_ADD);
                                    board_add_guess_letter(key_pressed);
                                 }
                                 break;

                    }
                }
            }
        #endif

        UPDATE_KEYS();
        UPDATE_KEY_REPEAT(J_DPAD);

        // Handle D-Pad movement separate from other buttons
        // so that buttons kept held down don't lock out D-pad
        //  movement during keyboard entry
        if (KEY_PRESSED(J_DPAD)) {

            // Check for initial button press or repeat
            if ((key_repeat_count == KEY_REPEAT_START) ||
                (key_repeat_count >= KEY_REPEAT_DPAD_THRESHOLD)) {

                if (key_repeat_count >= KEY_REPEAT_DPAD_THRESHOLD)
                    RESET_KEY_REPEAT(KEY_REPEAT_DPAD_RELOAD_VAL);

                // Filter with D-Pad to allow movement while still pressing A / B
                keyboard_move_cursor(keys & J_DPAD);
            }
        }


        if (KEY_RELEASED(J_SELECT)) {
            switch(previous_keys & J_SELECT) {

                // Show Options menu
                case J_SELECT:
                    // Can set game_state to lost (exiting loop)
                    //
                    // * Select is also used as a modifier, so
                    //   only trigger if it wasn't used for that
                    if (!keys_select_consumed) {

                        // Require N select presses in a row to spawn menu
                        menu_select_count++;
                        if (menu_select_count >= MENU_SELECT_COUNT_TRIGGER) {
                            settings_menu_show();
                            waitpadreleased_lowcpu(J_SELECT);
                            menu_select_count = MENU_SELECT_COUNT_RESET;
                        }
                    }

                    // reset modified state
                    keys_select_consumed = false;
                    break;
            }
        }


        if (KEY_TICKED(J_A | J_B | J_START)) {
            switch(keys & (J_A | J_B | J_START)) {

                // Check a guess
                case J_START:
                    if (keys & J_SELECT) {
                        board_autofill_matched_letters();
                        keys_select_consumed = true;
                    } else
                        // Can set game_state to won / lost (exiting loop)
                        gameplay_handle_guess();
                    break;

                // Add/Remove letters from a guess
                case J_A:
                    if (keys & J_SELECT) {
                        if (guess_letter_cursor < LETTER_CURSOR_MAX) {
                            guess_letter_cursor++;
                            play_sfx(SFX_CURSOR_MOVE);
                        }

                        keys_select_consumed = true;
                    } else {
                        play_sfx(SFX_TILE_ADD);
                        board_add_guess_letter(LETTER_ADD_USE_ON_SCREEN_KEYBOARD);
                    }

                    board_update_letter_cursor();
                    break;

                case J_B:
                    if (keys & J_SELECT) {
                        if (guess_letter_cursor > LETTER_CURSOR_START) {
                            guess_letter_cursor--;
                            play_sfx(SFX_CURSOR_MOVE);
                        }

                        keys_select_consumed = true;
                    } else {
                        play_sfx(SFX_TILE_REMOVE);
                        board_remove_guess_letter();
                    }

                    board_update_letter_cursor();
                    break;
            }
        }
    }
}
