// input.c

#include <gbdk/platform.h>

#include "input.h"

uint8_t keys = 0x00;
uint8_t previous_keys = 0x00;
uint8_t keys_ticked;
// uint8_t keys_changed = 0x00;
uint8_t key_repeat_count = 0x00;


// Reduce CPU usage by only checking once per frame
// Allows a loop control to be passed in
void waitpadreleased_lowcpu(uint8_t button_mask) {

    while (joypad() & button_mask) { wait_vbl_done(); }

    UPDATE_KEYS_TICKED();
    UPDATE_KEYS();
}

// Reduce CPU usage by only checking once per frame
// Allows a loop control to be passed in
void waitpadticked_lowcpu(uint8_t button_mask) {

    while (1) {

        wait_vbl_done(); // yield CPU
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
    };
}


/*// Reduce CPU usage by only checking once per frame
// Allows a loop control to be passed in
void waitpadticked_lowcpu(uint8_t button_mask, volatile uint8_t * loop_control) {

    uint8_t always_loop = 1;

    // If no loop control var specified, use a placeholder
    if (loop_control == NULL)
        loop_control = &always_loop;

    while (*loop_control) {

        wait_vbl_done(); // yield CPU
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
    };
}
*/
