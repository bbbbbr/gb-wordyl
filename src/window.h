// window.h

#ifndef _WINDOW_H
#define _WINDOW_H

#define POPUP_WIN_START_X 1u
#define POPUP_WIN_START_Y 1u

#define WIN_DIALOG_SET_FUNC_RUN(p_func) (p_win_func_run = p_func)
#define WIN_DIALOG_CLEAR_FUNC_RUN() (p_win_func_run = NULL)
extern void (*p_win_func_run)(void);

// Make sure to manually call gameplay_restore_sprites() if below is used
// Helps prevent board sprite flashing between subsequent menu popups
#define WIN_DIALOG_RESTORE_SPRITES_AFTER_NO  (win_restore_sprites_after = false)
#define WIN_DIALOG_RESTORE_SPRITES_AFTER_YES (win_restore_sprites_after = true)
extern bool win_restore_sprites_after;

void win_dialog_draw(void);
bool win_confirm_dialog(char * str_message);
uint8_t win_dialog_show_message(uint8_t win_y_moveto, uint8_t * str_1, uint8_t * str_2);

#define PX_PER_TILE        8
#define DIALOG_BORDER_SIZE (1 * PX_PER_TILE)

// Adds one extra line at bottom for padding
// MAXIMUM input value is 16
#define DIALOG_TEXT_HEIGHT_LINES(LINES)  (DEVICE_SCREEN_PX_HEIGHT - (((LINES + 1) * PX_PER_TILE) + DIALOG_BORDER_SIZE))

#endif
