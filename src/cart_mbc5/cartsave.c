#include <gbdk/platform.h>
#include <stdint.h>
#include <string.h>

#include "../common.h"


// Map a secondary stats struct to the beginning of SRAM (0xA000) when using MBC5
settings_rec __at (0xA000) sram_stats;


void cartsave_restore_data(void) {

    ENABLE_RAM_MBC5;
    memcpy((void *)&game_settings, (void *)&sram_stats, sizeof(game_settings));
    DISABLE_RAM_MBC5;
}


// TODO: warning on failure to save?
void cartsave_save_data(void) {

    ENABLE_RAM_MBC5;
    memcpy((void *)&sram_stats, (void *)&game_settings, sizeof(game_settings));
    DISABLE_RAM_MBC5;
}



