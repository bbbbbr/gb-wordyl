#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#include "usb_keyboard.h"

// For use with Inside Gadgets USB HID keyboard to link port adapter

#if !defined(CART_31k_1kflash)

uint8_t key_pressed = NO_KEY;
uint8_t key_previous = NO_KEY;

#define SERIAL_TRANSFER_TIMEOUT        0x0800u
#define REPEAT_OFF                     0u
#define REPEAT_FIRST_THRESHOLD         8u
#define REPEAT_CONTINUE_THRESHOLD      4u

static uint8_t key_repeat_timeout;
static uint8_t key_repeat_previous;

static uint8_t insidegadgets_serial_exchange(void) {

    uint16_t timeout = SERIAL_TRANSFER_TIMEOUT;

    SB_REG = 0x00u;
    SC_REG = SIOF_CLOCK_INT | SIOF_XFER_START;


    while (timeout && (SC_REG & SIOF_XFER_START))
        timeout--;

    if (!timeout) {
        SC_REG = 0x00u;
        return 0xFFu;
    }

    return SB_REG;
}

// Keys on the IG usb keyboard adapter take the USB HID key and add 93.
// So to look up a key subtract 93 and check the (hex) chart here:
// https://usb.org/sites/default/files/hut1_21.pdf#chapter.10
static uint8_t insidegadgets_translate_key(uint8_t raw_key) {

    if (((raw_key >= 'a') && (raw_key <= 'z')) ||
        ((raw_key >= 'A') && (raw_key <= 'Z')))
        return raw_key;

    switch (raw_key) {
        case 133u: return KEY_ENTER;
        case 134u: return KEY_ESCAPE;
        case 135u: return KEY_BACKSPACE;
        case 137u: return ' ';
        case 140u:
        case 172u: return KEY_ARROW_RIGHT;
        case 141u:
        case 173u: return KEY_ARROW_LEFT;
        case 142u:
        case 174u: return KEY_ARROW_DOWN;
        case 143u:
        case 175u: return KEY_ARROW_UP;
        case 194u:                        // Dedicated Menu key (right side, by ctrl, etc)
        case 151u: return KEY_HELP;       // F1 Key
        case 163u: return KEY_PRINTSCREEN;
        case 169u: return KEY_DELETE;
        default:   return NO_KEY;
    }
}


static void insidegadgets_process_key(uint8_t key_value) {

    key_previous = key_pressed;

    if (!key_value) {
        key_pressed = NO_KEY;
        key_repeat_timeout = REPEAT_OFF;
        key_repeat_previous = NO_KEY;
        return;
    }

    if (key_value == key_repeat_previous) {

        key_pressed = NO_KEY;

        if (key_repeat_timeout)
            key_repeat_timeout--;
        else {
            key_pressed = key_repeat_previous;
            key_repeat_timeout = REPEAT_CONTINUE_THRESHOLD;
        }
    }
    else {
        key_pressed = key_value;
        key_repeat_previous = key_value;
        key_repeat_timeout = REPEAT_FIRST_THRESHOLD;
    }
}


bool usb_keyboard_init(void) {

    key_pressed = NO_KEY;
    key_previous = NO_KEY;
    key_repeat_timeout = REPEAT_OFF;
    key_repeat_previous = NO_KEY;

    return true;
}


bool usb_keyboard_poll(void) {

    uint8_t raw_key = insidegadgets_serial_exchange();
    uint8_t key_value = insidegadgets_translate_key(raw_key);

    insidegadgets_process_key(key_value);
    return true;
}


#endif // !defined(CART_31k_1kflash)
