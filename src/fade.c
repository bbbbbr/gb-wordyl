#include <gbdk/platform.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "gameboy_color.h"
#include "fade.h"


const uint16_t FADE_LUT[] = {
           0b0111111111111111,  // Allow all source color through
           0b0011110111101111,
           0b0001110011100111,
           0b0000110001100011,
           0b0000010000100001,
           0b0000000000000000}; // Allow no source color through (black)

#define FADE_MAX    (ARRAY_LEN(FADE_LUT) - 1)

// #define CGB_PAL_COUNT   5 // from gameboy_color.h
#define CGB_COL_PER_PAL 4

#define FADE_IN_START  5
#define FADE_OUT_START 0
#define FADE_STEP_MAX  (FADE_IN_START + 1)
#define FADE_STEP_MIN  (FADE_OUT_START - 1)


static uint16_t cgb_fade_pal[CGB_COL_PER_PAL * CGB_PAL_COUNT];


const uint8_t fade_steps_dmg_normal[] = {
    DMG_PALETTE(DMG_WHITE,     DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK),
    DMG_PALETTE(DMG_LITE_GRAY, DMG_DARK_GRAY, DMG_BLACK,     DMG_BLACK),
    DMG_PALETTE(DMG_DARK_GRAY, DMG_BLACK,     DMG_BLACK,     DMG_BLACK),
    DMG_PALETTE(DMG_BLACK,     DMG_BLACK,     DMG_BLACK,     DMG_BLACK)
};

// Used by intro splash
const uint8_t fade_steps_dmg_invert[] = {
    DMG_PALETTE(DMG_BLACK,     DMG_DARK_GRAY, DMG_LITE_GRAY, DMG_WHITE),
    DMG_PALETTE(DMG_BLACK,     DMG_BLACK,     DMG_DARK_GRAY, DMG_LITE_GRAY),
    DMG_PALETTE(DMG_BLACK,     DMG_BLACK,     DMG_BLACK,     DMG_DARK_GRAY),
    DMG_PALETTE(DMG_BLACK,     DMG_BLACK,     DMG_BLACK,     DMG_BLACK)
};



const uint8_t * fade_steps_dmg = fade_steps_dmg_normal;


// Fade out to black
void fade_out(void) {
    if (IS_CGB)
        fade_cgb(FADE_OUT);
    else {
        for (uint8_t c = 0; c < ARRAY_LEN(fade_steps_dmg_normal); c++) {
            wait_vbl_done();
            BGP_REG = fade_steps_dmg[c];
            OBP0_REG = OBP1_REG = fade_steps_dmg[c];
            delay(70);
        }
    }
}

// Fade in from black
void fade_in(void) {
    if (IS_CGB)
        fade_cgb(FADE_IN);
    else {
        for (uint8_t c = 0; c < ARRAY_LEN(fade_steps_dmg_normal); c++) {
            wait_vbl_done();
            BGP_REG = fade_steps_dmg[ARRAY_LEN(fade_steps_dmg_normal) - 1 - c];
            OBP0_REG = OBP1_REG= fade_steps_dmg[ARRAY_LEN(fade_steps_dmg_normal) - 1 - c];
            delay(70);
        }
        // TODO: FIXME: Slight hack, OBP1 has a different palette setup, modify after fade in (uses less code for now)
        OBP1_REG = DMG_PALETTE(DMG_WHITE, DMG_WHITE, DMG_DARK_GRAY, DMG_BLACK);
    }
}


// Fade in/out from Black
void fade_cgb(int8_t fade_dir) {

    uint8_t pal;
    // Static for size opt
    static int8_t i;

    i = (fade_dir == FADE_OUT) ? FADE_OUT_START : FADE_IN_START; // Set loop start (5 for fade in, 0 for fade out)

    while ((i != FADE_STEP_MIN) && (i != FADE_STEP_MAX)) {

        // Loop through all palettes and their colors
        for(pal = 0; pal < (CGB_COL_PER_PAL * CGB_PAL_COUNT); pal++) {

            // Load color and mask it with
            cgb_fade_pal[pal] = cgb_palettes[pal] & FADE_LUT[i];
        }
        // Same palette used for background and sprites
        wait_vbl_done();
        set_bkg_palette(0, CGB_PAL_COUNT, cgb_fade_pal);
        set_sprite_palette(0, CGB_PAL_COUNT, cgb_fade_pal);

        delay(100);
        i += fade_dir;
    }
}
