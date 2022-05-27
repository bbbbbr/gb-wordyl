// fade.h

#ifndef _FADE_H
#define _FADE_H

#include "common.h"

extern const uint8_t fade_steps_dmg_normal[];
extern const uint8_t fade_steps_dmg_invert[];
extern const uint8_t * fade_steps_dmg;

#define FADE_OUT 1
#define FADE_IN -1

void fade_in(void);
void fade_out(void);
void fade_cgb(int8_t fade_dir);

#define DMG_PAL_INVERT_ON  fade_steps_dmg = fade_steps_dmg_invert
#define DMG_PAL_INVERT_OFF fade_steps_dmg = fade_steps_dmg_normal

#endif
