// sfx_common.h

#ifndef _SFX_COMMON_H
#define _SFX_COMMON_H

#include "cbtfx.h"


#define SFX_TILE_ADD           0x00
#define SFX_TILE_REMOVE        0x01
#define SFX_TILE_REVEAL_RESULT 0x02
#define SFX_CURSOR_MOVE        0x03
#define SFX_GAME_WON           0x04
#define SFX_GAME_LOST          0x05
#define SFX_GAME_FORFEIT       0x05 // maybe just same as SFX_GAME_LOST
#define SFX_EXIT_SPLASH        0x06
#define SFX_MENU_ACTION_ACKNOWLEDGE 0x07


extern const unsigned char * SFX_list[];

void play_sfx(uint8_t sfx_num);

#endif