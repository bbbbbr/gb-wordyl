#ifndef PLATFORM_KEYBOARD_H
#define PLATFORM_KEYBOARD_H

#include <stdint.h>
#include <stdbool.h>

#define NO_KEY        0u

// Arrow keys are arbitrarily chosen to not clash with common
// ascii chars, though they do match the GBDK ibm_fixed font
#define KEY_ARROW_UP    1u
#define KEY_ARROW_DOWN  2u
#define KEY_ARROW_RIGHT 3u
#define KEY_ARROW_LEFT  4u
#define KEY_HELP        5u
#define KEY_PRINTSCREEN 6u

// Use ascii values for these keys
#define KEY_BACKSPACE   8u
#define KEY_ENTER       13u
#define KEY_ESCAPE      27u
#define KEY_DELETE      127u

extern uint8_t platform_key_pressed;
extern uint8_t platform_key_previous;

#define key_pressed  platform_key_pressed
#define key_previous platform_key_previous

bool platform_keyboard_init(void);
bool platform_keyboard_poll(void);
void platform_keyboard_printscreen(void);

#endif // PLATFORM_KEYBOARD_H
