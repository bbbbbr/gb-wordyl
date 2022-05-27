// keyboard.h

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBD_ROWS 3

#define KEYBD_CURSOR_OFFSET_Y 4u

extern const char *kb[KEYBD_ROWS];
extern const int8_t kb_row_len[KEYBD_ROWS];
extern const int8_t kb_offsets[KEYBD_ROWS];

extern int8_t kb_x;
extern int8_t kb_y;

void keyboard_reset(void);

inline char keyboard_get_letter(void);

void keyboard_fill_letter_cgb_pal(uint8_t row, uint8_t col, uint8_t palnum);
void keyboard_set_color_for_letter(uint8_t row, uint8_t col, uint8_t match_type, uint8_t tile_id);
inline void keyboard_set_default_color_letter(uint8_t row, uint8_t col);

void keyboard_update_from_guess(void);
void keyboard_redraw_clean(void);
void keyboard_draw_map(void);

void keyboard_update_cursor(void);
void keyboard_move_cursor(uint8_t dpad_key);

#endif
