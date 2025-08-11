#include <gbdk/platform.h>
#include <gb/gbdecompress.h>

#include <stdint.h>

#include <gb/sgb.h>
#include <string.h>

#include "../common.h"
#include "sgb_border.h"

#include "sgb_border_data.h"
#include "sgb_border_tiles.gbcomp.h"
#include "sgb_border_map.gbcomp.h"

// This gets set if sgb_check() succeeds
// Use it to select some alternate tile coloring
bool sgb_enabled = false;

#define TILEDATA_SIZE (sgb_border_tiles_sz_decomp)
#define TILEMAP_SIZE (sgb_border_map_sz_decomp)


// Command code + data to set BGP / OBP GB Pals on the SNES/SGB
// "Transmit color data for SGB palette 0, color 0-3, and for SGB palette 1, color 1-3 (without separate color 0)."
const uint8_t sgb_cmd_gb_screen_area_palette[] = {
    (SGB_PAL_01 << 3) | 1,
    0xBB, 0x7F, // Light Blue  0xDEEEFD -> 0xBB7F
    0xB5, 0x56, // Med Grey    0xAFAFAF -> B556
    0xF9, 0x25, // Med Orange  0xc97f48 -> 0xF925   // 0xF8, 0x01, // Med-Dark Orange  0xC17C00 -> 0xF801
    0x00, 0x00  // Black
};

#define SGB_CHR_BLOCK0 0
#define SGB_CHR_BLOCK1 1

#define SGB_SCR_FREEZE 1
#define SGB_SCR_UNFREEZE 0


// Buffer used for sending SGB transfers
uint8_t sgb_buf[20];

// sgb_border_tiles_sz_decomp will be the largest size, use it for both
uint8_t sgb_decomp_buf[sgb_border_tiles_sz_decomp];
uint8_t * p_sgb_tiledata;


void sgb_transfer_buf(uint8_t a, uint8_t b) {
	sgb_buf[0]=a;
	sgb_buf[1]=b;
	sgb_transfer(sgb_buf);
}


void sgb_border_try_loading(void) {

    // Wait 4 frames
    // For SGB on PAL SNES this delay is required on startup, otherwise borders don't show up
    for (uint8_t i = 4; i != 0; i--) vsync();

    DISPLAY_ON;

    if (sgb_check()) {

        sgb_enabled = true;

		for (uint8_t c = 0; c <=19; c++)
			sgb_buf[c] = 0;
        // memset(sgb_buf, 0, sizeof(sgb_buf));

        sgb_transfer_buf((SGB_MASK_EN << 3) | 1, SGB_SCR_FREEZE);

        BGP_REG = OBP0_REG = OBP1_REG = 0xE4U;
        SCX_REG = SCY_REG = 0U;

        uint8_t tmp_lcdc = LCDC_REG;

        HIDE_SPRITES, HIDE_WIN, SHOW_BKG;
        DISPLAY_ON;


        // Decompress tile data
        gb_decompress(sgb_border_tiles, sgb_decomp_buf);
        p_sgb_tiledata = sgb_decomp_buf;

        // Prepare tilemap for SGB_BORDER_CHR_TRN (should display all 256 tiles)
		// Fills map with incrementing counter
		uint8_t idx = 0;
        for (uint8_t y = 0u; y <= 13u; y++)
            for (uint8_t x = 0u; x <= 19u; x++)
				set_bkg_tile_xy(x,y, idx++);

        // transfer tile data
        uint8_t ntiles = (TILEDATA_SIZE > 256 * 32) ? 0 : TILEDATA_SIZE >> 5;
        if ((!ntiles) || (ntiles > 128U)) {

            set_bkg_data(0, 0, p_sgb_tiledata);
			sgb_transfer_buf((SGB_CHR_TRN << 3) | 1, SGB_CHR_BLOCK0);

            if (ntiles) ntiles -= 128U;
            p_sgb_tiledata += (128 * 32);

            set_bkg_data(0, ntiles << 1, p_sgb_tiledata);
			sgb_transfer_buf((SGB_CHR_TRN << 3) | 1, SGB_CHR_BLOCK1);
        } else {
            set_bkg_data(0, ntiles << 1, p_sgb_tiledata);
			sgb_transfer_buf((SGB_CHR_TRN << 3) | 1, SGB_CHR_BLOCK0);
        }

        // Decompress map data
        gb_decompress(sgb_border_map, sgb_decomp_buf);

        // transfer map and palettes
        set_bkg_data(0, (uint8_t)(TILEMAP_SIZE >> 4), sgb_decomp_buf);
        set_bkg_data(128, (uint8_t)(sizeof(sgb_border_palettes) >> 4), sgb_border_palettes);
        sgb_transfer_buf((SGB_PCT_TRN << 3) | 1, 0);

        // Set the palettes used by the game boy screen area (OBP/BGP)
        // set_bkg_data(0, (uint8_t)(sizeof(sgb_gb_screen_area_palette) >> 4), sgb_gb_screen_area_palette);
        sgb_transfer(sgb_cmd_gb_screen_area_palette);


        LCDC_REG = tmp_lcdc;

        // clear SCREEN
        fill_bkg_rect(0, 0, 20, 18, 0);

        sgb_transfer_buf((SGB_MASK_EN << 3) | 1, SGB_SCR_UNFREEZE);
    }
}
