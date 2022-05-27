// lang_text.h

#ifndef _LANG_TEXT_H
#define _LANG_TEXT_H

// == This has default strings
#include "../lang_text_defaults.inc"


// == #define overrides for language strings go here

// Catalan: Not yet translated

// Use '[' for 'Ç'

#undef INTRO_DIALOG_WIN_Y
#define INTRO_DIALOG_WIN_Y DIALOG_TEXT_HEIGHT_LINES(14)
#undef __INTRO_MESSAGE_STR
#define __INTRO_MESSAGE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "IDIOMA ...... " STR(LANG_CODE) "\n" \
    "  NOMES A..Z\n" \
    "  SENSE SIMBOLS\n" \
    "\n" \
    "MENU DE JOC:\n" \
    "  3 x SELECT\n" \
    "\n" \
    "CURSOR DE PARAULA:\n" \
    "  SELECT + B A\n" \
    "\n" \
    "AUTOEMPLENAT:\n" \
    "  SELECT + START\n" \
    "\n" \
    "PREMEU UNA TECLA"


// #undef HELP_LEGEND_MAP_Y
#undef HELP_LEGEND_MAP_Y_GAP
// #define HELP_LEGEND_MAP_Y (2u)     // Start Win Y for color key board letters
#define HELP_LEGEND_MAP_Y_GAP (1u) // Gap between color key board letters in case more space is needed        
#undef HELP_LEGEND_DIALOG_WIN_Y
#define HELP_LEGEND_DIALOG_WIN_Y  DIALOG_TEXT_HEIGHT_LINES(9)
#undef __HELP_LEGEND_DIALOG_STR
#define __HELP_LEGEND_DIALOG_STR \
/*  |----------------XX| Available space (16 chars wide, 2 reserved for letter legend tiles) */\
    "LLEGENDA:\n" \
    "\n" \
    "NO COINCIDEIX ..\n" \
    "\n" \
    "\n" \
    "LLOC ERRONI ....\n" \
    "\n" \
    "\n" \
    "ENCERT..........\n"


#undef MENU_Y_EXIT
#undef MENU_Y_HARD_MODE
#undef MENU_Y_AUTO_FILL
#undef MENU_Y_SKIP_FILLED
#undef MENU_Y_SOUND_FX
#undef MENU_Y_SHOW_STATS
#undef MENU_Y_FORFEIT_ROUND
#undef MENU_Y_RESET_STATS
#undef MENU_Y_HELP
// "^" char gets replaced with a UI checkboxes reflecting option on/off state
// Leading space on each line is room for Cursor
// MAX Height is 16
#undef OPTIONS_MENU_DIALOG_WIN_Y
#define OPTIONS_MENU_DIALOG_WIN_Y  DIALOG_TEXT_HEIGHT_LINES(14)
#undef __OPTIONS_MENU_STR
#define __OPTIONS_MENU_STR \
/*  |------------------| Available space (18 chars wide) */\
    " ... OPCIONS ...\n" \
    "\n" \
    " SURT DEL MENU\n" \
    "\n" \
    " MODE DIFICIL .. ^\n" \
    " AUTOEMPLENAT  . ^\n" \
    " SALTA EMPLENATS ^\n" \
    " EFECTES DE SO . ^\n" \
    "\n" \
    " ESTADISTIQUES\n" \
    " RENDIR-SE\n" \
    " ESBORRA ESTAD.\n" \
    "\n" \
    " AJUDA"

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
// space                     8
#define MENU_Y_SHOW_STATS    9
#define MENU_Y_FORFEIT_ROUND 10
#define MENU_Y_RESET_STATS   11
// space                     12
#define MENU_Y_HELP          13



#define CONFIRM_DIALOG_WIN_Y  DIALOG_TEXT_HEIGHT_LINES(6)
#undef __CONFIRM_DIALOG_STR
#define __CONFIRM_DIALOG_STR \
/*  |------------------| Available space (18 chars wide) */ \
    "CONFIRMACIO:^\n" \
    "ENRERE: QUALSEVOL\n"\
    "..................\n" \
    "\n" \

#undef __CONFIRM_FORFEIT_STR
#define __CONFIRM_FORFEIT_STR     "RENDIR-SE?"

#undef __CONFIRM_STATS_RESET_STR
#define __CONFIRM_STATS_RESET_STR "REINICIAR-LES?"


// Display hard mode indicator at top of screen
#undef OPT_HARDMODE_DISPLAY_X
#undef OPT_HARDMODE_DISPLAY_Y
#define OPT_HARDMODE_DISPLAY_X 5
#define OPT_HARDMODE_DISPLAY_Y 0

#undef __STR_HARD_MODE_DISPLAY
#undef __STR_HARD_MODE_HIDE
#define __STR_HARD_MODE_DISPLAY "DIFICIL"
#define __STR_HARD_MODE_HIDE    "    "


#undef __MESSAGE_HARD_MODE_CANT_CHANGE_STR
#define __MESSAGE_HARD_MODE_CANT_CHANGE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "MODE DIFICIL:\n" \
    "NOMES CANVIABLE\n" \
    "AL PRINCIPI"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_HARD_MODE_GUESS_NOT_VALID_STR
#define __MESSAGE_HARD_MODE_GUESS_NOT_VALID_STR \
/*  |------------------| Available space (18 chars wide) */\
    "MODE DIFICIL: CAL\n" \
    "USAR TOTES\n" \
    "LES PISTES"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_LOSE_STR
#define __MESSAGE_LOSE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "Oh! Has perdut\n" \
    "\n" \
    "La resposta: "    // <-- Needs 5 chars left for answer word!


// NOTE! ^ gets replaced with guess 5 letter word num
//
// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_GAME_WON_STR
#define __MESSAGE_GAME_WON_STR \
/*  |------------------| Available space (18 chars wide) */\
    "Visca! Has guanyat\n" \
    "\n" \
    "En ^ de 6 intents"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_WORD_TOO_SHORT_STR
#define __MESSAGE_WORD_TOO_SHORT_STR \
/*  |------------------| Available space (18 chars wide) */\
    "Massa curt!\n" \
    "\n" \
    "Minim 5 lletres"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_WORD_NOT_IN_DICT_STR
#define __MESSAGE_WORD_NOT_IN_DICT_STR \
/*  |------------------| Available space (18 chars wide) */\
    "La paraula no es\n" \
    "\n" \
    "al diccionari!"


#undef __MESSAGE_STATS_RESET_STR
#define __MESSAGE_STATS_RESET_STR \
/*  |------------------| Available space (18 chars wide) */\
    "TOT LLEST!"


#undef __STATS_TEMPLATE_STR
#define __STATS_TEMPLATE_STR \
/*  |------------------| Available space (18 chars wide) */\
    " Partides  ____^\n" \
    " Ratxa     ____^\n" \
    " Guanyades ____^\n" \
    " Perdudes  ____^\n" \
    "\n" \
    "Intents\n" \
    " Minims   ____^\n" \
    " Maxims   ____^\n" \
    " Mitjana  ____^\n"   // <-- All "____^" get replaced by numbers

// Should be no more than 18 chars
#undef __INTRO_PRESS_START_STR
#undef __INTRO_PRESS_START_STR_BLANK
#define __INTRO_PRESS_START_STR       "PREMEU START"
#define __INTRO_PRESS_START_STR_BLANK "            "

// X, Y controls upper-left corner coordinates
#undef __INTRO_CREDITS_STR
#define __INTRO_CREDITS_STR \
/*  |--------------------| */\
    "GRACIES A:\n" \
    "TOXA\n" \
    "DAEO\n" \
    "ARPRUSS\n" \
    "COFFEE 'VALEN' BAT\n" \
    "STACKSMASHING\n" \
    "\n" \
    "GBDK2020\n" \
    "EMULICIOUS\n" \
    "FERRANTE CRAFTS"

#endif

