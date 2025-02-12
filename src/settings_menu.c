// common.c

#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#include "stats.h"
#include "common.h"
#include "input.h"
#include "gfx_common.h"
#include "sfx_common.h"

#include "gameplay.h"
#include "settings.h"
#include "window.h"
#include "help_info.h"

#include "settings_menu.h"

#include <lang_text.h>
#include <cartsave.h>

#if (defined(MEGADUCK))
    #include "megaduck_laptop/megaduck_keyboard.h"
#endif


#define MENU_MOVE_UP   -1
#define MENU_MOVE_NONE  0
#define MENU_MOVE_DOWN  1



const uint8_t menu_cursor_y_ids[] = {
    MENU_Y_EXIT,
    MENU_Y_HARD_MODE,
    MENU_Y_AUTO_FILL,
    MENU_Y_SKIP_FILLED,
    MENU_Y_SOUND_FX,
    MENU_Y_STARTUP_HELP,
    MENU_Y_SHOW_STATS,
    MENU_Y_FORFEIT_ROUND,
    MENU_Y_RESET_STATS,
    MENU_Y_HELP
};

// Function pointer table to match each menu item entry
const void (*menu_funcs[])(void) = {
    NULL, // Exit menu, no function to call
    &hardmode_handle_change,
    NULL, // opt_autofill_enabled
    NULL, // opt_skip_autofilled
    NULL, // opt_sound_fx_enabled
    NULL, // opt_help_message_on_startup
    &stats_show,
    &ask_forfeit_round,
    &ask_stats_reset,
    &help_info_show
};

// Bool variable pointer table to match each menu item entry
uint8_t * const p_menu_vars[] = {
    NULL, // Exit menu, no function to call
    NULL, // &game_settings.opt_hard_mode_enabled,
    &game_settings.opt_autofill_enabled,
    &game_settings.opt_skip_autofilled,
    &game_settings.opt_sound_fx_enabled,
    &game_settings.opt_help_message_on_startup,
    NULL, // Show Stats
    NULL, // Skip Auto-filled
    NULL, // Reset Stats
    NULL, // Help
};


#define MENU_ROW_HEIGHT_PX         8u
#define MENU_CURSOR_X             (8u + DEVICE_SPRITE_PX_OFFSET_X)
#define MENU_CURSOR_Y_ADJUST       12u // compensate for
#define MENU_CURSOR_Y_BASELINE    ((1 * (MENU_ROW_HEIGHT_PX)) + MENU_CURSOR_Y_ADJUST)  // To factor in window border
#define MENU_CURSOR_Y_SPR2_OFFSET  7u   // y + 7 instead of +8 to make a pointy arrow since it's using y flipped sprite from top of cursor
#define MENU_CURSOR_Y_AT_ROW(row) (WY_REG + (menu_cursor_y_ids[row] * MENU_ROW_HEIGHT_PX) + MENU_CURSOR_Y_BASELINE) // + (MENU_CURSOR_Y_BASELINE - DEVICE_SPRITE_PX_OFFSET_Y))


static bool settings_save_required;
static uint8_t menu_idx;
static const uint8_t menu_str_template[] = __OPTIONS_MENU_STR;
static uint8_t menu_str[sizeof(menu_str_template)];


static void menu_update_cursor(int8_t);
static void settings_menu_render_text(void);


// Hide the menu cursor
inline void menu_hide_cursor(void) {

    hide_sprite(SP_ID_CURSOR_MENU_START);
    hide_sprite(SP_ID_CURSOR_MENU_START + 1u);
}


// Inverts a boolean variable passed in from the var pointer table
// Only handles bool pointer vars
inline void settings_menu_change_var(bool * p_setting) {

    // Queue up a cart save if needed
    settings_save_required = true;

    if (*p_setting) *p_setting = false;
    else *p_setting = true;

    play_sfx(SFX_CURSOR_MOVE );
    //play_sfx(SFX_MENU_ACTION_ACKNOWLEDGE);

    // Redraw the menu with updated settings
    settings_menu_render_text();
    print_gotoxy(POPUP_WIN_START_X,POPUP_WIN_START_Y, PRINT_WIN);
    print_str(menu_str);
}


// Display menu in a popup window with a cursor
void settings_menu_show(void) {

    settings_save_required = false;

    settings_menu_render_text();

    // Prevent the cursor from flashing between popups
    // (The window dialog will handle hiding them)
    WIN_DIALOG_RESTORE_SPRITES_AFTER_NO;

    WIN_DIALOG_SET_FUNC_RUN(&menu_run);
    win_dialog_show_message(OPTIONS_MENU_DIALOG_WIN_Y, menu_str, NULL);


    // SDCC BUG?
    // Function pointer var needs to be static local or global
    // Otherwise SDCC generates "jp NZ, (hl)"
    // And gets the error: ?ASlink-Warning-Undefined Global 'hl' referenced by module 'settings_menu'
    //
    // if (menu_funcs[menu_idx])
    //     (*(menu_funcs[menu_idx]))();
    //
    // void (*p_func)(void) = menu_funcs[menu_idx];
    //
    // Call selected menu item handler
    static void (*p_func)(void) = NULL;
    p_func = menu_funcs[menu_idx];

    if (p_func)
        p_func();

    #if defined(CART_31k_1kflash) || defined(CART_mbc5) || defined(CART_md0_sram)
        if (settings_save_required) {
            cartsave_save_data();
        }
    #endif

    // Restore normal sprite hiding behavior
    WIN_DIALOG_RESTORE_SPRITES_AFTER_YES;
    gameplay_restore_sprites();
}


// Move the cursor to highlight the current row
// move_dir should only every be: -1, 0, +1
static void menu_update_cursor(int8_t move_dir) {

    if (move_dir) {

        menu_idx += move_dir;

        // Handle wraparound
        // Top -> Bottom is via unsigned overflow
        // Bottom -> Top  is via max menu entry count
        if (menu_idx == 0xFFu)
            menu_idx = ARRAY_LEN(menu_cursor_y_ids) - 1;
        else if (menu_idx > (ARRAY_LEN(menu_cursor_y_ids) - 1))
            menu_idx = 0u;

        play_sfx(SFX_CURSOR_MOVE);
    }

    uint8_t y = MENU_CURSOR_Y_AT_ROW(menu_idx);
    move_sprite(SP_ID_CURSOR_MENU_START     , MENU_CURSOR_X, y);
    move_sprite(SP_ID_CURSOR_MENU_START + 1u, MENU_CURSOR_X, y + MENU_CURSOR_Y_SPR2_OFFSET);
}


// Prepares settings text for the popup
// Replaces "^" characters with on/off status checkboxes
static void settings_menu_render_text(void) {

    // Reset display template
    // memcpy(menu_str, menu_str_template, sizeof(menu_str_template));
    // About 1/2 the size of including memcpy()
    uint8_t * p_dest = menu_str;
    const uint8_t * p_src = menu_str_template;
    while (p_src != menu_str_template + sizeof(menu_str_template)) {
        *p_dest++ = *p_src++;
    }

    str_bool_checkbox_at_X(menu_str, game_settings.opt_hard_mode_enabled);
    str_bool_checkbox_at_X(menu_str, game_settings.opt_autofill_enabled);
    str_bool_checkbox_at_X(menu_str, game_settings.opt_skip_autofilled);
    str_bool_checkbox_at_X(menu_str, game_settings.opt_sound_fx_enabled);
    str_bool_checkbox_at_X(menu_str, game_settings.opt_help_message_on_startup);
    // str_bool_checkbox_at_X(menu_str, game_settings.opt_tile_flip_enabled);
}


// Small loop that runs from within the popup menu
// which moves the cursor around and accepts selections
void menu_run(void) {

    // Default starting cursor pos / doubles as menu global return value
    menu_idx = MENU_DEFAULT_EXIT;

    // Show the cursor
    menu_update_cursor(MENU_MOVE_NONE);

    while (1) {

        vsync();

        #if defined(MEGADUCK)
            // Poll for keyboard keys every other frame
            // (Polling intervals below 20ms may cause keyboard lockup)
            if ((sys_time & 0x01u) && (megaduck_laptop_detected)) {
                if (megaduck_keyboard_poll_keys()) {

                    megaduck_keyboard_process_keys();

                    switch (megaduck_key_pressed) {
                        case ' ':           // Fall through, same as Enter
                        case KEY_ENTER:     // Apply action for current menu item
                                            // If it has a variable, invert it and redraw
                                            if (p_menu_vars[menu_idx])
                                                settings_menu_change_var(p_menu_vars[menu_idx]);

                                            // If it has a function, exit the menu and call it
                                            if (menu_funcs[menu_idx] || (menu_idx == MENU_DEFAULT_EXIT)) {
                                                // Hide the cursor here so that it doesn't show
                                                // while the popup window is retracting
                                                menu_hide_cursor();
                                                return;
                                            }
                                            break;

                        case KEY_ARROW_UP:   menu_update_cursor(MENU_MOVE_UP);
                                             break;
                        case KEY_ARROW_DOWN: menu_update_cursor(MENU_MOVE_DOWN);
                                             break;


                        case KEY_ESCAPE:      // Fall through, same as Backspace
                        case KEY_BACKSPACE:   // Use B to exit the menu regardless of where the cursor is
                                              menu_idx = MENU_DEFAULT_EXIT;
                                              menu_hide_cursor();
                                              return;
                                              break; // redundant
                    }
                }
            }            
        #endif

        UPDATE_KEYS();
        UPDATE_KEYS_TICKED();
        switch (KEYS_GET_TICKED()) {

            case J_UP: menu_update_cursor(MENU_MOVE_UP);
                       break;

            case J_DOWN: menu_update_cursor(MENU_MOVE_DOWN);
                         break;

                      // Use B to exit the menu regardless of where the cursor is
            case J_B: menu_idx = MENU_DEFAULT_EXIT;
                      menu_hide_cursor();
                      return;
                      break; // redundant

            case J_START: // Fall through to exit below
            case J_A:
                        // If it has a variable, invert it and redraw
                        if (p_menu_vars[menu_idx])
                            settings_menu_change_var(p_menu_vars[menu_idx]);

                        // If it has a function, exit the menu and call it
                        if (menu_funcs[menu_idx] || (menu_idx == MENU_DEFAULT_EXIT)) {
                            // Hide the cursor here so that it doesn't show
                            // while the popup window is retracting
                            menu_hide_cursor();
                            return;
                        }
                        break;
        }
    }
}



