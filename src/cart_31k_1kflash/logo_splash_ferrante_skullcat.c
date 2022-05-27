#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#include <gb/gbdecompress.h>

#include "../gfx_common.h"
#include "../gameboy_color.h"
#include "../fade.h"

#include "logo_splash_map_ferrante_text_skullcat.h"
#include "logo_splash_map.gbcomp.h"
#include "logo_splash_tiles_ferrante_text_skullcat.h"

void logo_splash_ferrante_skullcat_show(void) {

    // This works before gfx_load() because the splash logo tile 0 is also blank
    gfx_clear_bkg_blank_tile();

    // ** Borrows board_letters_decomp_buf temporarily before its actual use **
    gb_decompress(logo_splash_tiles, board_letters_decomp_buf);
    set_bkg_1bpp_data(0, 44u, board_letters_decomp_buf);

    // Load compressed map
    //    ../../tools/gbcompress -v --varname=logo_splash_mapPLN0 --cin --cout logo_splash_map_ferrante_text_skullcat.c logo_splash_map.gbcomp.c
    //        Compressed: 96 bytes -> 71 bytes (%73.96)
    // ** Borrows board_letters_decomp_buf temporarily before its actual use **
    gb_decompress(logo_splash_map, board_letters_decomp_buf);
    set_bkg_tiles((DEVICE_SCREEN_WIDTH - logo_splash_mapWidth) / 2u,
                  (DEVICE_SCREEN_HEIGHT - logo_splash_mapHeight) / 2u - 1u, // -1u to better center, up 1 row
                  logo_splash_mapWidth, logo_splash_mapHeight,
                  board_letters_decomp_buf);

    // Version for uncompressed map
    // set_bkg_tiles((DEVICE_SCREEN_WIDTH - logo_splash_mapWidth) / 2u,
    //               (((DEVICE_SCREEN_HEIGHT - logo_splash_mapHeight) / 2u) - 1u),
    //               logo_splash_mapWidth, logo_splash_mapHeight,
    //               logo_splash_mapPLN0);

    fade_in();

    if (IS_CGB) delay(2750);
    else        delay(1500);

    fade_out();

    // Clear screen so next
    gfx_clear_bkg_blank_tile();
}
