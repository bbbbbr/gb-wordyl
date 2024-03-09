// lang_text.h

#ifndef _LANG_TEXT_H
#define _LANG_TEXT_H

// == This has default strings
#include "../lang_text_defaults.inc"


// == #define overrides for language strings go here

// German: Translation by skite2001 ^_-


// DE Keyboard Map Layout (Z & Y transposed)
#undef KEYBD_ROW_1
#undef KEYBD_ROW_2
#undef KEYBD_ROW_3
#define KEYBD_ROW_1 "QWERTZUIOP"
#define KEYBD_ROW_2  "ASDFGHJKL"
#define KEYBD_ROW_3   "YXCVBNM"


#undef INTRO_DIALOG_WIN_Y
#define INTRO_DIALOG_WIN_Y DIALOG_TEXT_HEIGHT_LINES(13)
#undef __INTRO_MESSAGE_STR
#define __INTRO_MESSAGE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "SPRACHE ....... " STR(LANG_CODE) "\n" \
    "  NUR A..Z OHNE\n" \
    "  SONDERZEICHEN\n" \
    "\n" \
    "OPTIONEN:3x SELECT\n" \
    "\n" \
    "STELLENAUSWAHL:\n" \
    "  SELECT + B A\n" \
    "\n" \
    "AUTOEINGABE:\n" \
    "  SELECT + START\n" \
    "\n" \
    "TASTE DRUECKEN"


#undef __INTRO_MEGADUCK_LAPTOP_MESSAGE_STR
#define __INTRO_MEGADUCK_LAPTOP_MESSAGE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "MEGADUCK LAPTOP\n" \
    "\n" \
    "OPTIONEN:\n" \
    "  HELP.AYUDA.HILFE\n" \
    "\n" \
    "STELLENAUSWAHL:\n" \
    "  < > ^ /\n" \
    "\n" \
    "AUTOEINGABE:\n" \
    "  ESC.SALDIA\n" \
    "\n" \
    "TASTE DRUECKEN"


// #undef HELP_LEGEND_MAP_Y
// #undef HELP_LEGEND_MAP_Y_GAP
// #define HELP_LEGEND_MAP_Y (2u)     // Start Win Y for color key board letters
// #define HELP_LEGEND_MAP_Y_GAP (0u) // Gap between color key board letters in case more space is needed        
#undef HELP_LEGEND_DIALOG_WIN_Y
#define HELP_LEGEND_DIALOG_WIN_Y  DIALOG_TEXT_HEIGHT_LINES(7)
#undef __HELP_LEGEND_DIALOG_STR
#define __HELP_LEGEND_DIALOG_STR \
/*  |----------------XX| Available space (16 chars wide, 2 reserved for letter legend tiles) */\
    "FARBCODES:\n" \
    "\n" \
    "NICHT DABEI ....\n" \
    "\n" \
    "FALSCHE STELLE..\n" \
    "\n" \
    "RICHTIG ........\n"


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
#define OPTIONS_MENU_DIALOG_WIN_Y  DIALOG_TEXT_HEIGHT_LINES(15)
#undef __OPTIONS_MENU_STR
#define __OPTIONS_MENU_STR \
/*  |------------------| Available space (18 chars wide) */\
    "  .. OPTIONEN ..\n" \
    "\n" \
    " WEITERSPIELEN\n" \
    "\n" \
    " PROFI MODUS ... ^\n" \
    " AUTOEINGABE ... ^\n" \
    " UEBERSPRINGE OK ^\n" \
    " SOUND FX  ..... ^\n" \
    " EINSTEIGERHILFE ^\n" \
    "\n" \
    " ZEIGE STATS\n" \
    " RUNDE AUFGEBEN\n" \
    " STATS ERNEUERN\n" \
    "\n" \
    " HILFE"

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
// space                     9
#define MENU_Y_SHOW_STATS    10
#define MENU_Y_FORFEIT_ROUND 11
#define MENU_Y_RESET_STATS   12
// space                     13
#define MENU_Y_HELP          14



#define CONFIRM_DIALOG_WIN_Y  DIALOG_TEXT_HEIGHT_LINES(6)
#undef __CONFIRM_DIALOG_STR
#define __CONFIRM_DIALOG_STR \
/*  |------------------| Available space (18 chars wide) */ \
    "WEITER:^\n" \
    "STOP: ANDERE TASTE\n"\
    "..................\n" \
    "\n" \

#undef __CONFIRM_FORFEIT_STR
#define __CONFIRM_FORFEIT_STR     "RUNDE AUFGEBEN?"

#undef __CONFIRM_STATS_RESET_STR
#define __CONFIRM_STATS_RESET_STR "STATS ERNEUERN?"


// Display hard mode indicator just above keyboard on right
#undef OPT_HARDMODE_DISPLAY_X
#define OPT_HARDMODE_DISPLAY_X 14
#undef __STR_HARD_MODE_DISPLAY
#undef __STR_HARD_MODE_HIDE
#define __STR_HARD_MODE_DISPLAY "PROFI" // 5 chars max at this location
#define __STR_HARD_MODE_HIDE    "     "


#undef __MESSAGE_HARD_MODE_CANT_CHANGE_STR
#define __MESSAGE_HARD_MODE_CANT_CHANGE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "PROFI MODUS: NUR\n" \
    "BEI NEUER RUNDE\n" \
    "WECHSELBAR"


#undef __MESSAGE_HARD_MODE_GUESS_NOT_VALID_STR
#define __MESSAGE_HARD_MODE_GUESS_NOT_VALID_STR \
/*  |------------------| Available space (18 chars wide) */\
    "PROFI MODUS:\n" \
    "RICHTIGE HINWEISE\n" \
    "SIND BINDEND!"



#undef __MESSAGE_LOSE_STR
#define __MESSAGE_LOSE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "DU VERLIERST!\n" \
    "\n" \
    "ANTWORT IST: "    // <-- Needs 5 chars left for answer word


// NOTE! ^ gets replaced with guess 5 letter word num
//
// Intentionally not const! ^ gets replaced with guess num
#undef __MESSAGE_GAME_WON_STR
#define __MESSAGE_GAME_WON_STR \
/*  |------------------| Available space (18 chars wide) */\
    "DU HAST GEWONNEN!\n" \
    "\n" \
    "VERSUCH ^ von 5" // 6"


#undef __MESSAGE_WORD_TOO_SHORT_STR
#define __MESSAGE_WORD_TOO_SHORT_STR \
/*  |------------------| Available space (18 chars wide) */\
    "WORT IST ZU KURZ!\n" \
    "\n" \
    "5 BUCHSTABEN BITTE"

#undef __MESSAGE_WORD_NOT_IN_DICT_STR
#define __MESSAGE_WORD_NOT_IN_DICT_STR \
/*  |------------------| Available space (18 chars wide) */\
    "WORT IST NICHT\n" \
    "\n" \
    "IN DER DATENBANK!"


#undef __MESSAGE_STATS_RESET_STR
#define __MESSAGE_STATS_RESET_STR \
/*  |------------------| Available space (18 chars wide) */\
    "STATS ERNEUERT!"


#undef __STATS_TEMPLATE_STR
#define __STATS_TEMPLATE_STR \
/*  |------------------| Available space (18 chars wide) */\
    " RUNDEN   ____^\n" \
    " SERIE    ____^\n" \
    " GEWONNEN ____^\n" \
    " VERLOREN ____^\n" \
    "\n" \
    "GEWINNVERSUCHE\n" \
    " MIN      ____^\n" \
    " MAX      ____^\n" \
    " MITTE    ____^\n"   // <-- All "____^" get replaced by numbers


// Should be no more than 18 chars
#undef __INTRO_PRESS_START_STR
#undef __INTRO_PRESS_START_STR_BLANK
#define __INTRO_PRESS_START_STR       "DRUECKE START"
#define __INTRO_PRESS_START_STR_BLANK "             "


// X, Y controls upper-left corner coordinates
#undef __INTRO_CREDITS_STR
#define __INTRO_CREDITS_STR \
/*  |--------------------| */\
    "DANKE AN:\n" \
    "TOXA\n" \
    "DAEO\n" \
    "ARPRUSS\n" \
    "COFFEE 'VALEN' BAT\n" \
    "STACKSMASHING\n" \
    "EUCAL.BB\n" \
    "SKITE2001\n" \
    "\n" \
    "GBDK2020\n" \
    "EMULICIOUS\n" \
    "FERRANTE CRAFTS"


#endif
