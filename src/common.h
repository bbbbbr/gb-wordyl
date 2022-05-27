// common.h

#ifndef _COMMON_H
#define _COMMON_H

#include <stdbool.h>

// Controls whether debug test code is compiled
// It dumps dictionary testing output into the emu debug console
// Takes up about 1300-1400K, so keep it turned off unless needed
//
// #define TEST_DICT_ENABLED

// Selects debug test mode if enabled
// These are gated by TEST_DICT_ENABLED
//
#define TEST_ALLWORDS
// #define TEST_BENCHMARK
// #define TEST_ANSWERWORDS

// ==== Cheats for testing ====
// #define DEBUG_REVEAL_WHILE_TYPE
// #define DEBUG_FORCE_WORD "AMPLE"
// #define DEBUG_FORCE_WORD "HUMAN"
// #define DEBUG_FORCE_WORD "ACAÇA"
// #define DEBUG_REVEAL_WORD
// #define DEBUG_FORCE_WORD_BY_NUM  39u
// #define DEBUG_FORCE_WORD_BY_NUM  2294u //2200u // 2200u // 2295u // 2200u // 0u // 100u // 1351u
// TODO: show answer word number

#define _STR(s) #s
#define STR(s) _STR(s)

#define ARRAY_LEN(A)  sizeof(A) / sizeof(A[0])

#define CEDILLA_MAPPED_CHR_STRQUOTE    "["
#define CEDILLA_MAPPED_CHR   '['
#define CHECKBOX_ON_CHR      '|'
#define CHECKBOX_OFF_CHR     '}'


// Game flow control
#define GAME_STATE_INTRO      0
#define GAME_STATE_FIRSTSTART 1
#define GAME_STATE_RESTART    2
#define GAME_STATE_RUNNING    3
#define GAME_STATE_OVER       4


// Controls board size and many other things
#define GUESS_NUM_RESET 0
#define GUESS_NUM_FIRST (GUESS_NUM_RESET)
#define WORD_LENGTH 5
#define MAX_GUESSES 6

#define LETTER_CURSOR_START 0
#define LETTER_CURSOR_MAX   (WORD_LENGTH - 1)


#define BOARD_HIGHLIGHT_NO  false
#define BOARD_HIGHLIGHT_YES true

// IMPORANT: Order of values is important and relied on for sorting, DO NOT CHANGE!
//           Also used for palette array lookup, help color key, etc
// Used :
// * Printing color lookup table
// * Letter matching evaluation ranking
// * Ensuring keyboard highlighting only gets upgraded and not downgraded with progressive guesses
#define LETTER_BEING_ENTERED 4u
#define LETTER_RIGHT_PLACE 3u
#define LETTER_WRONG_PLACE 2u
#define LETTER_NOT_MATCHED 1u
#define LETTER_NOT_SET     0u

#define STATUS_GAME_WON      2u
#define STATUS_GAME_LOST     1u
#define STATUS_GAME_CONTINUE 0u

#define GUESS_LETTER_EMPTY   0

extern uint16_t wordnum_last;
extern uint8_t game_state;

extern uint8_t guess_num;
extern uint8_t guess_letter_cursor;

extern uint8_t prev_guess_eval[WORD_LENGTH];
extern uint8_t guess_eval[WORD_LENGTH];

extern bool guess_auto_filled[WORD_LENGTH+1];
extern char exact_matches[WORD_LENGTH+1];
extern char prev_guess[WORD_LENGTH+1];
extern char guess[WORD_LENGTH+1];
extern char word[WORD_LENGTH+1];


// Save record signature check
#define STATS_SIG_CHECK_0 0xA50Fu
#define STATS_SIG_CHECK_1 0x1E78u

// Struct containing the save game
// For MBC5 will be mapped to the SRAM through a pointer
typedef struct settings_rec {

    // Don't change order, it will scramble cart saves

    // Stats
    uint16_t save_check0;
    uint16_t save_check1;
    uint16_t games_count;
    uint16_t games_won;
    uint16_t games_lost;
    uint16_t games_streak;

    uint16_t guesses_min;
    uint16_t guesses_max;
    uint16_t guesses_sum;

    // Options
    bool opt_hard_mode_enabled;
    bool opt_autofill_enabled;
    bool opt_sound_fx_enabled;
    bool opt_skip_autofilled;
    bool intro_message_displayed;
        // Not implemented, set to zero on first run
        uint8_t opt_blank_1;
        uint8_t opt_blank_2;
        uint8_t opt_blank_3;
        uint8_t opt_blank_4;
        uint8_t opt_blank_5;

} settings_rec;

extern settings_rec game_settings;


void settings_load(void);

void str_bool_checkbox_at_X(uint8_t *, bool);
void str_u16_left_at_X(uint8_t * p_str, uint16_t num);
// uint8_t * str_u16_left_at_X(uint8_t * p_str, uint16_t num);

void copy_or_reset_prev_guess(char* guess);

// uint8_t contains(char *str, char c); // TODO: DEAD CODE
void evaluate_letters(char * guess);
bool evaluate_guess_hard_mode(char * guess);

#endif