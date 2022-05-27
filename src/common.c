// common.c

#include <gbdk/platform.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "gfx_common.h"

uint16_t wordnum_last = 0;
uint8_t game_state;

// Options and stats
settings_rec game_settings;

uint8_t prev_guess_eval[WORD_LENGTH];
uint8_t guess_eval[WORD_LENGTH];

bool answer_letter_used[WORD_LENGTH];
bool guess_letter_used[WORD_LENGTH];

uint8_t guess_num;
uint8_t guess_letter_cursor;

bool guess_auto_filled[WORD_LENGTH+1];
char exact_matches[WORD_LENGTH+1]; // Used for auto-fill of previous exact matched letters
char prev_guess[WORD_LENGTH+1]; // Used for hard-mode enforcement
char guess[WORD_LENGTH+1]; // Current guess
char word[WORD_LENGTH+1];  // TODO: rename this to "answer_word"


// Patch a bool checkbox indicator into the string at char '^'
// * Warning: Safetys are turned off, lots can go wrong here
void str_bool_checkbox_at_X(uint8_t * p_str, bool is_enabled) {

    // Search for start position
    while ((*p_str != '^') && (*p_str)) p_str++;

    if (is_enabled)
        *p_str = CHECKBOX_ON_CHR; // Checkbox ON swap char
    else
        *p_str = CHECKBOX_OFF_CHR; // Checkbox OFF swap char
}


#define PRINT_MAX_DIGITS  5

// Patch a number into the string at char '^'
// * uitoa() puts in leading zeros and a tailing str terminator, so it's not suitable
// * Warning: Safetys are turned off, lots can go wrong here
//
// TODO: remove old declaration if not used anymore
// uint8_t * str_u16_left_at_X(uint8_t * p_str, uint16_t num) {
void str_u16_left_at_X(uint8_t * p_str, uint16_t num) {

    uint8_t index = PRINT_MAX_DIGITS;

    // if (p_str == NULL) return NULL;

    // Search for start position
    while ((*p_str != '^') && (*p_str)) p_str++;

    // Store individual digits of n in reverse order
    // Starting at the END of the array and working forward
    // (using do-while to handle when initial value == 0)
    do {
        // decrement the counter first, so it finishes as pointing to the current digit in the array
        index--;
        *p_str-- = (num % 10) + '0';
        num = num / 10;
    } while (num != 0);

    // Fill remaining characters with space chars
    while (index > 0) {
        index--;
        *p_str-- = ' ';
    }

    // // Move pointer past end of print area
    // return (p_str + PRINT_MAX_DIGITS + 1);
}



// Check a guess word for letter matches and marks
// them as LETTER_NOT_IN_WORD, LETTER_WRONG_PLACE or LETTER_RIGHT_PLACE
// Original: arpruss. Optimized some
//
// Called after a user has submitted a guess (which validated as a dictionary word)
void evaluate_letters(char* guess) {

    // First scan word for exact letter matches
    for (uint8_t i=0; i < WORD_LENGTH; i++) {

        if (guess[i] == word[i]) {
            // Store all exact matches (across multiple guesses) for auto-fill
            exact_matches[i] = word[i];
            guess_eval[i] = LETTER_RIGHT_PLACE;
            answer_letter_used[i] = true;
        }
        else {
            guess_eval[i] = LETTER_NOT_MATCHED;
            answer_letter_used[i] = false;
        }
    }

    // Next for each guess char check RIGHT Letter at WRONG location
    for (uint8_t i=0; i < WORD_LENGTH; i++) {

        if (guess_eval[i] == LETTER_NOT_MATCHED) {

            // Scan through unused answer word letters
            for (uint8_t j=0; j < WORD_LENGTH;j++) {

                if ((answer_letter_used[j] == false) && (guess[i] == word[j])) {

                    // Found a match at WRONG location, mark both and
                    // break out for next char in guess
                    answer_letter_used[j] = true;
                    guess_eval[i] = LETTER_WRONG_PLACE;
                    break;
                }
            }
        }
    }
}


// Used for hard mode evaluation
void copy_or_reset_prev_guess(char* guess) {

    for (uint8_t i=0; i < WORD_LENGTH; i++) {
        prev_guess[i] = guess[i];
        prev_guess_eval[i] = guess_eval[i];
    }
}


// Validate hard mode
// Returns false if criteria not met
//   - Green: must be in exact space
//   - Yellow: must be used
bool evaluate_guess_hard_mode(char* guess) {

    static uint8_t idx_p;

    // First scan guess for exact letter matches
    // Fail if any exact matches aren't present
    for (uint8_t i=0; i < WORD_LENGTH; i++) {

        // Reset current guess letter status
        guess_letter_used[i] = false;

        if (prev_guess_eval[i] == LETTER_RIGHT_PLACE) {

            if (guess[i] == prev_guess[i])
                guess_letter_used[i] = true;
            else
                return false;
//                return prev_guess_eval[i]; // Could missing return letter
        }
    }

    // Then scan for right letter wrong place matches
    // Fail if any of those can't be found
    for (idx_p=0; idx_p < WORD_LENGTH; idx_p++) {

        if (prev_guess_eval[idx_p] == LETTER_WRONG_PLACE) {

            bool found_prev_guess_letter = false;

            // Scan through unused guess word letters
            for (uint8_t idx_g=0; idx_g < WORD_LENGTH;idx_g++) {

                if ((guess_letter_used[idx_g] == false) &&
                    (prev_guess[idx_p] == guess[idx_g])) {

                    // Found a match, mark char as used and break out
                    found_prev_guess_letter = true;
                    guess_letter_used[idx_g] = true;
                    break;
                }
            }

            if (found_prev_guess_letter == false)
                return false;
//                return prev_guess_eval[idx_p]; // Could missing return letter
        }
    }

    // If nothing failed up to this point then hard validation passed, turn success
//    return '\0'; // Return NULL if letter not found
    return true;
}
