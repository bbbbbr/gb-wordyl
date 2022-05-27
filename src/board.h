// board.h

#ifndef _BOARD_H
#define _BOARD_H

#define BOARD_TILE_FLIP_SLOW    2u
#define BOARD_TILE_FLIP_FAST    1u
#define BOARD_TILE_FLIP_NONE    0u

extern uint8_t g_board_tile_flip_speed;


// Range for consecutive board rows: start offset, length
// Board layouts
extern const uint8_t board_row_ranges_game[];
extern const uint8_t board_row_ranges_splash[];
extern const uint8_t * p_board_layout;

// Also used by help_info
extern const uint8_t board_map_letter[];

#define BOARD_SET_LAYOUT_SPLASH (p_board_layout = board_row_ranges_splash)
#define BOARD_SET_LAYOUT_GAME   (p_board_layout = board_row_ranges_game)

// Turns board tile flipping on/off and sets speed in board_draw_letter()
// Use with BOARD_TILE_FLIP_*
#define BOARD_SET_FLIP_SPEED(speed) g_board_tile_flip_speed = (speed)


void board_map_fill();

void board_reset_autofilled_flags(void);
void board_autofill_matched_letters(void);

void board_update_row_cursor(void);
void board_update_letter_cursor(void);

void board_hide_letter_cursor(void);

// TODO: delete if no longer used
// void board_hide_row_cursor(void);

void board_redraw_clean(void);
void board_render_guess_letter_at_cursor(void);
void board_add_guess_letter(void);
void board_remove_guess_letter(void);

void board_draw_letter_bits(uint8_t row, uint8_t col, uint8_t letter);
void board_draw_tile_flip_anim(uint8_t row, uint8_t col);
void board_draw_letter(uint8_t row, uint8_t col, uint8_t letter, bool do_highlight);
void board_draw_word(uint8_t row, uint8_t * p_guess, bool do_highlight);

#endif
