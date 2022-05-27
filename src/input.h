// input.h

#ifndef INPUT_H
#define INPUT_H

#define J_ANY_KEY           0xFFu
#define J_WAIT_ALL_RELEASED 0xFFu
#define J_WAIT_ANY_PRESSED  0x00u
#define J_DPAD              (J_LEFT | J_RIGHT | J_UP | J_DOWN)

// Units are number of frames (~60 / sec)
#define KEY_REPEAT_START                0u
#define KEY_REPEAT_DPAD_INITIAL_DELAY   20u // 40u is slower, better accessibility // 20u is nice, might be too fast for some
#define KEY_REPEAT_DPAD_REPEAT_DELAY    7u
#define KEY_REPEAT_DPAD_THRESHOLD       (KEY_REPEAT_DPAD_INITIAL_DELAY)
#define KEY_REPEAT_DPAD_RELOAD_VAL      (KEY_REPEAT_DPAD_THRESHOLD - KEY_REPEAT_DPAD_REPEAT_DELAY)


#define UPDATE_KEYS() previous_keys = keys; keys = joypad(); // keys_changed = (keys ^ previous_keys)
#define UPDATE_KEY_REPEAT(MASK) if ((MASK) & previous_keys & keys) { key_repeat_count++; } else { key_repeat_count=KEY_REPEAT_START; }
#define RESET_KEY_REPEAT(NEWVAL) key_repeat_count = NEWVAL

#define KEY_PRESSED(MASK)  (keys & (MASK))
#define KEY_TICKED(MASK)   ((keys & (MASK)) && !(previous_keys & (MASK)))
#define KEY_RELEASED(MASK) (!(keys & (MASK)) && (previous_keys & (MASK)))
//#define KEY_TICKED(MASK)   (keys & keys_changed & (MASK))
//#define KEY_RELEASED(MASK) (~keys & keys_changed & (MASK))

#define UPDATE_KEYS_TICKED() (keys_ticked = (keys ^ (keys & previous_keys)))
#define KEYS_GET_TICKED() (keys_ticked)

#define ANY_KEY_PRESSED (keys)

void waitpadreleased_lowcpu(uint8_t button_mask);
void waitpadticked_lowcpu(uint8_t button_mask);
// void waitpadticked_lowcpu(uint8_t button_mask, volatile uint8_t * loop_control);

extern uint8_t keys;
extern uint8_t previous_keys;
extern uint8_t keys_ticked;
// extern uint8_t keys_changed;
extern uint8_t key_repeat_count;

#endif // INPUT_H


