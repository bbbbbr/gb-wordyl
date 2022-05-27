// stats.h

#ifndef _STATS_H
#define _STATS_H

#define STATS_GAME_WON  true
#define STATS_GAME_LOST false


void stats_reset(void);
void stats_update(bool game_won, uint8_t guess_count);
void stats_show(void);


#endif // _STATS_H


