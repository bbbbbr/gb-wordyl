// lang_text.h

#ifndef _LANG_TEXT_H
#define _LANG_TEXT_H

// == This has default strings
#include "../lang_text_defaults.inc"


// == #define overrides for language strings go here

// Kernewek (Cornish Language): Translation sjd_sjd_
// Meur ras dhe Niwlen Ster, Kamm Cam ha Dyski Kernowek Discord

#undef INTRO_DIALOG_WIN_Y
#define INTRO_DIALOG_WIN_Y DIALOG_TEXT_HEIGHT_LINES(15)
#undef __INTRO_MESSAGE_STR
#define __INTRO_MESSAGE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "YETH ... " STR(LANG_CODE) "\n" \
    "  A..Z  HEPKEN\n" \
    "\n" \
    "ROL Y'N GWARI:\n" \
    "  3 x SELECT\n" \
    "\n" \
    "RESELL BORDH:\n" \
    "  SELECT + B A\n" \
    "\n" \
    "OMLENWEL:\n" \
    "  SELECT + START\n" \
    "\n" \
    "GWASK BOTON\n"\
    "HA DALLETH"\


#undef __INTRO_MEGADUCK_LAPTOP_MESSAGE_STR
#define __INTRO_MEGADUCK_LAPTOP_MESSAGE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "MEGADUCK LAPTOP\n" \
    "\n" \
    "ROL Y'N GWARI:\n" \
    "  HELP.AYUDA.HILFE\n" \
    "\n" \
    "RESELL BORDH:\n" \
    "  < > ^ /\n" \
    "\n" \
    "OMLENWEL:\n" \
    "  ESC.SALDIA\n" \
    "\n" \
    "DRUCK.PRINT SCRN\n" \
    "\n" \
    "GWASK BOTON\n"\
    "HA DALLETH"\

// #undef HELP_LEGEND_MAP_Y
// #undef HELP_LEGEND_MAP_Y_GAP
// #define HELP_LEGEND_MAP_Y (2u)     // Start Win Y for color key board letters
// #define HELP_LEGEND_MAP_Y_GAP (0u) // Gap between color key board letters in case more space is needed        
#undef HELP_LEGEND_DIALOG_WIN_Y
#define HELP_LEGEND_DIALOG_WIN_Y  DIALOG_TEXT_HEIGHT_LINES(7)
#undef __HELP_LEGEND_DIALOG_STR
#define __HELP_LEGEND_DIALOG_STR \
/*  |----------------XX| Available space (16 chars wide, 2 reserved for letter legend tiles) */\
    "ALHWEDH:\n" \
    "\n" \
    "LE VYTH ........\n" \
    "\n" \
    "LE KAMM ........\n" \
    "\n" \
    "LE EWN  ........\n"


#undef MENU_Y_EXIT
#undef MENU_Y_HARD_MODE
#undef MENU_Y_AUTO_FILL
#undef MENU_Y_SKIP_FILLED
#undef MENU_Y_SOUND_FX
#undef MENU_Y_STARTUP_HELP
#undef MENU_Y_SHOW_STATS
#undef MENU_Y_FORFEIT_ROUND
#undef MENU_Y_RESET_STATS
#undef MENU_Y_HELP
// "^" char gets replaced with a UI checkboxes reflecting option on/off state
// Leading space on each line is room for Cursor
// MAX Height is 16
#undef OPTIONS_MENU_DIALOG_WIN_Y
#define OPTIONS_MENU_DIALOG_WIN_Y  DIALOG_TEXT_HEIGHT_LINES(16)
#undef __OPTIONS_MENU_STR
#define __OPTIONS_MENU_STR \
/*  |------------------| Available space (18 chars wide) */\
    " ... DEWISYOW ...\n" \
    "\n" \
    " KWYTYA ROL\n" \
    "\n" \
    " NIVEL KALES ... ^\n" \
    " OMLENWEL ...... ^\n" \
    " TERLEMMEL LENWYS^\n" \
    " SONYOW ........ ^\n" \
    " GWERES ORTH\n" \
    "   DALLETHANS .. ^\n" \
    "\n" \
    " DISKWEDHES STATS\n" \
    " KWYTYA TRO\n" \
    " DASSETTYA STATS\n" \
    "\n" \
    " GWERES"

// "SKIP FILLED" is ok for "SKIP AUTOFILLED"

// These need to exactly match the menu order & formating above
// Line 0 is the first line of text
// menu header               0
// space                     1
#define MENU_Y_EXIT          2
// space                     3
#define MENU_Y_HARD_MODE     4
#define MENU_Y_AUTO_FILL     5
#define MENU_Y_SKIP_FILLED   6
#define MENU_Y_SOUND_FX      7
#define MENU_Y_STARTUP_HELP  8
// space                     10
#define MENU_Y_SHOW_STATS    11
#define MENU_Y_FORFEIT_ROUND 12
#define MENU_Y_RESET_STATS   13
// space                     14
#define MENU_Y_HELP          15



#define CONFIRM_DIALOG_WIN_Y  DIALOG_TEXT_HEIGHT_LINES(6)
#undef __CONFIRM_DIALOG_STR
#define __CONFIRM_DIALOG_STR \
/*  |------------------| Available space (18 chars wide) */ \
    "AFYDHYA:         ^\n" \
    "HEDHI:  BOTON ARAL\n"\
    "..................\n" \
    "\n" \

#undef __CONFIRM_FORFEIT_STR
#define __CONFIRM_FORFEIT_STR     "KWYTYA TRO?"

#undef __CONFIRM_STATS_RESET_STR
#define __CONFIRM_STATS_RESET_STR "DASSETTYA STATS?"


// Display hard mode indicator at top of screen
#undef OPT_HARDMODE_DISPLAY_X
#undef OPT_HARDMODE_DISPLAY_Y
#define OPT_HARDMODE_DISPLAY_X 5
#define OPT_HARDMODE_DISPLAY_Y 0

#undef __STR_HARD_MODE_DISPLAY
#undef __STR_HARD_MODE_HIDE
#define __STR_HARD_MODE_DISPLAY "KALES"
#define __STR_HARD_MODE_HIDE    "       "


#undef __MESSAGE_HARD_MODE_CANT_CHANGE_STR
#define __MESSAGE_HARD_MODE_CANT_CHANGE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "N.KALES:RES YW BOS\n" \
    "AN CHANJ GWRYS\n" \
    "ORTH PENN AN DRO"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_HARD_MODE_GUESS_NOT_VALID_STR
#define __MESSAGE_HARD_MODE_GUESS_NOT_VALID_STR \
/*  |------------------| Available space (18 chars wide) */\
    "NIVEL KALES: RES \n" \
    "YW USYA PUB HYNT\n" \
    "Y'N DISMYK"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_LOSE_STR
#define __MESSAGE_LOSE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "TY A WRUG FYLLEL!\n" \
    "\n" \
    "GORTHYP: "    // <-- Needs 5 chars left for answer word!


// NOTE! ^ gets replaced with guess 5 letter word num
//
// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_GAME_WON_STR
#define __MESSAGE_GAME_WON_STR \
/*  |------------------| Available space (18 chars wide) */\
    "TY A WRUG GWAYNYA!\n" \
    "\n" \
    "DISMYGOW: ^"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_WORD_TOO_SHORT_STR
#define __MESSAGE_WORD_TOO_SHORT_STR \
/*  |------------------| Available space (18 chars wide) */\
    "RE VERR YW AN GER!\n" \
    "RES YW BOS 5\n" \
    "LYTHEREN YNNO"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_WORD_NOT_IN_DICT_STR
#define __MESSAGE_WORD_NOT_IN_DICT_STR \
/*  |------------------| Available space (18 chars wide) */\
    "GER ANKOTH\n"


#undef __MESSAGE_STATS_RESET_STR
#define __MESSAGE_STATS_RESET_STR \
/*  |------------------| Available space (18 chars wide) */\
    "DASSETTYA STATS!"


#undef __STATS_TEMPLATE_STR
#define __STATS_TEMPLATE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "GWARIOW\n" \
    "  YN SOMM... ____^\n" \
    " KADON   ... ____^\n" \
    " GWAYNYOW... ____^\n" \
    " FALLOW  ... ____^\n" \
    "LESRANNANS DISMYK\n" \
    " LYHA    ... ____^\n" \
    " MOYHA   ... ____^\n" \
    " KRESEK  ... ____^\n"   // <-- All "____^" get replaced by numbers

// Should be no more than 18 chars
#undef __INTRO_PRESS_START_STR
#undef __INTRO_PRESS_START_STR_BLANK
#define __INTRO_PRESS_START_STR       "GWASK START"
#define __INTRO_PRESS_START_STR_BLANK "           "


// X, Y controls upper-left corner coordinates
#undef __INTRO_CREDITS_STR
#define __INTRO_CREDITS_STR \
/*  |--------------------| */\
    "MEUR RAS DHE :\n" \
    "TOXA\n" \
    "DAEO\n" \
    "ARPRUSS\n" \
    "COFFEE 'VALEN' BAT\n" \
    "STACKSMASHING\n" \
    "EUCAL.BB\n" \
    "SJD\n" \
    "\n" \
    "GBDK2020\n" \
    "EMULICIOUS\n" \
    "FERRANTE CRAFTS"


#endif
