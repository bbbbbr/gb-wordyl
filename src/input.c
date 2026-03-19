// input.c

#include <gbdk/platform.h>
#include <stdbool.h>

#include "input.h"
#include "common.h"

#if (defined(MEGADUCK))
    #include <duck/laptop_io.h>
    #include <duck/laptop_keycodes.h>
    #include "megaduck_laptop/megaduck_keyboard.h"
    #include "megaduck_laptop/megaduck_printer.h"
    #include "megaduck_laptop/megaduck_printscreen.h"

    duck_keyboard_data_t keydata;
#endif

uint8_t keys = 0x00;
uint8_t previous_keys = 0x00;
uint8_t keys_ticked;
// uint8_t keys_changed = 0x00;
uint8_t key_repeat_count = 0x00;


#if defined(MEGADUCK)

    static void waitpadreleased_lowcpu_megaduck_keyboard(uint8_t button_mask) {

       do {
            UPDATE_KEYS();
            vsync();

            // Poll for keyboard keys every other frame
            // (Polling intervals below 20ms may cause keyboard lockup)
            if ((sys_time & 0x01u) && (megaduck_laptop_detected)) {
                if (duck_io_poll_keyboard(&keydata)) {
                    duck_io_process_key_data(&keydata, megaduck_model);

                    if (key_pressed == KEY_PRINTSCREEN) {
                        duck_print_screen();
                        continue;
                    }

                    if ((!key_pressed) &&
                        (!KEY_PRESSED(button_mask))) break;
                }
            }
        } while (KEY_PRESSED(button_mask));
    }


    static bool waitpadticked_lowcpu_megaduck_keyboard(bool * p_keyboard_checked) {
        // Poll for keyboard keys every other frame
        // (Polling intervals below 20ms may cause keyboard lockup)
        if ((sys_time & 0x01u) && (megaduck_laptop_detected)) {

            if (duck_io_poll_keyboard(&keydata)) {
                duck_io_process_key_data(&keydata, megaduck_model);

                if (key_pressed == KEY_PRINTSCREEN) {
                    duck_print_screen();
                    return false; // No exit
                }

                // Prevent passing through any key press by flagging the press
                // and then returning once no keys are pressed
                if (*p_keyboard_checked) {
                    if ((key_pressed) && (!key_previous))
                        return true;  // Yes exit
                }

                // Only register a non-pressed key if key repeat is not active)
                // (otherwise the gaps between repeated keys might look like non-keypresses
                if (!(keydata.flags & DUCK_IO_KEY_FLAG_KEY_REPEAT)) {
                    *p_keyboard_checked = true;
                }
            }
        }
        return false;  // No exit
    }

#endif // #if defined(MEGADUCK)


#if (defined(GAMEBOY) || defined(ANALOGUEPOCKET)) && !defined(CART_31k_1kflash)

    static void waitpadreleased_lowcpu_usb_keyboard(uint8_t button_mask) {

        bool keyboard_pressed = false;
        do {
            UPDATE_KEYS();
            vsync();

            keyboard_pressed = (key_pressed != NO_KEY);
            if (platform_keyboard_poll()) {
                keyboard_pressed = (key_pressed != NO_KEY);
            }
        } while (KEY_PRESSED(button_mask) || keyboard_pressed);
    }


    static bool waitpadticked_lowcpu_usb_keyboard(bool * p_keyboard_checked) {

        if (platform_keyboard_poll()) {
            // Prevent passing through any key press by flagging the press
            // and then returning once a new keyboard key is pressed.
            if (*p_keyboard_checked) {
                if ((key_pressed) && (!key_previous))
                    return true;  // Yes exit
            }

            *p_keyboard_checked = true;
        }

        return false; // No exit
    }

#endif // #if (defined(GAMEBOY) || defined(ANALOGUEPOCKET)) && !defined(CART_31k_1kflash)


// Reduce CPU usage by only checking once per frame
// Allows a loop control to be passed in
void waitpadreleased_lowcpu(uint8_t button_mask) {

    #if defined(MEGADUCK)
        waitpadreleased_lowcpu_megaduck_keyboard(button_mask);
    #elif (defined(GAMEBOY) || defined(ANALOGUEPOCKET)) && !defined(CART_31k_1kflash)
        waitpadreleased_lowcpu_usb_keyboard(button_mask);
    #else
        while (joypad() & button_mask) {
            vsync();
        }
    #endif

    UPDATE_KEYS_TICKED();
    UPDATE_KEYS();
}


// Reduce CPU usage by only checking once per frame
// Allows a loop control to be passed in
void waitpadticked_lowcpu(uint8_t button_mask) {

    bool keyboard_checked = false;

    while (1) {

        vsync(); // yield CPU
        UPDATE_KEYS();
        if (KEY_TICKED(button_mask)) {

            // Store key(s) ticked to exist this menu
            // in case they need to be retrieved
            UPDATE_KEYS_TICKED();

            // Prevent passing through any key ticked
            // event that may have just happened
            UPDATE_KEYS();

            return;
        }

        #if defined(MEGADUCK)
            if (waitpadticked_lowcpu_megaduck_keyboard(&keyboard_checked)) return;
        #elif (defined(GAMEBOY) || defined(ANALOGUEPOCKET)) && !defined(CART_31k_1kflash)
            if (waitpadticked_lowcpu_usb_keyboard(&keyboard_checked)) return;
        #endif
    };
}
