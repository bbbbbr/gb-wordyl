#ifndef __SGB_BORDER_H_INCLUDE
#define __SGB_BORDER_H_INCLUDE

#include <gb/gb.h>
#include <stdint.h>
#include <stdbool.h>

extern bool sgb_enabled;

#define SNES_RGB(R,G,B) (uint16_t)((B) << 10 | (G) << 5 | (R))

#define SGB_IS_ENABLED (sgb_enabled == true)

void sgb_border_try_loading(void);

#endif