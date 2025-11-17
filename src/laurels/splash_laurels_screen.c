#include <gbdk/platform.h>
#include <stdint.h>
#include <stdbool.h>

#include <gb/gbdecompress.h>

#include "../gfx_common.h"
#include "../gameboy_color.h"
#include "../fade.h"

#include "splash_laurels_screen.h"
#include "splash_laurels_1k_map.h"
#include "splash_laurels_1k_map.comp.h"
#include "splash_laurels_1k_tiles.comp.h"


/*
$GBDK_HOME/bin/png2asset resources/splash_laurels_1k.png -map -noflip -no_palettes -bin -maps_only -o splash_laurels_1k_map.bin
$GBDK_HOME/bin/gbcompress -v --cout --varname=splash_laurels_1k_map splash_laurels_1k_map_map.bin splash_laurels_1k_map.comp.c

$GBDK_HOME/bin/png2asset resources/splash_laurels_1k.png -map -noflip -no_palettes -bin -tiles_only -o splash_laurels_1k_tiles.bin
$GBDK_HOME/bin/gbcompress -v --cout --varname=splash_laurels_1k_tiles splash_laurels_1k_tiles_tiles.bin splash_laurels_1k_tiles.comp.c
*/

void splash_laurels_1k_show(void) {

    // This works before gfx_load() because the splash logo tile 0 is also blank
    gfx_clear_bkg_blank_tile();

 	gb_decompress_bkg_data(0u, splash_laurels_1k_tiles);

    // Load compressed map
    //    ../../tools/gbcompress -v --varname=logo_splash_mapPLN0 --cin --cout logo_splash_map_ferrante_text_skullcat.c logo_splash_map.gbcomp.c
    //        Compressed: 96 bytes -> 71 bytes (%73.96)
    // ** Borrows board_letters_decomp_buf temporarily before its actual use **
    gb_decompress(splash_laurels_1k_map, board_letters_decomp_buf);
    set_bkg_tiles((DEVICE_SCREEN_WIDTH - (splash_laurels_1k_map_WIDTH / splash_laurels_1k_map_TILE_W)) / 2u,
                  (DEVICE_SCREEN_HEIGHT - (splash_laurels_1k_map_HEIGHT / splash_laurels_1k_map_TILE_H)) / 2u,
                  (splash_laurels_1k_map_WIDTH / splash_laurels_1k_map_TILE_W),
                  (splash_laurels_1k_map_HEIGHT / splash_laurels_1k_map_TILE_H),
                  board_letters_decomp_buf);


    fade_in();

    if (IS_CGB) delay(2750);
    else        delay(1500);

    fade_out();

    // Clear screen so next
    gfx_clear_bkg_blank_tile();
}
