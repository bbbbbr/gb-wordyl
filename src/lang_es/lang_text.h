// lang_text.h

#ifndef _LANG_TEXT_H
#define _LANG_TEXT_H

// == This has default strings
#include "../lang_text_defaults.inc"


// == #define overrides for language strings go here

// Spanish: Translation by Adamantibus

#undef INTRO_DIALOG_WIN_Y
#define INTRO_DIALOG_WIN_Y DIALOG_TEXT_HEIGHT_LINES(14)
#undef __INTRO_MESSAGE_STR
#define __INTRO_MESSAGE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "IDIOMA ...... " STR(LANG_CODE) "\n" \
    "  SOLO A..Z\n" \
    "  NO CAR. ESPEC.\n" \
    "\n" \
    "VER MENU:\n" \
    " SELECT x 3\n" \
    "\n" \
    "MOVER CURSOR:\n" \
    "  SELECT + B A\n" \
    "\n" \
    "AUTORELLENAR:\n" \
    "  SELECT + START\n" \
    "\n" \
    "PULSA OTRA TECLA"


// #undef HELP_LEGEND_MAP_Y
#undef HELP_LEGEND_MAP_Y_GAP
// #define HELP_LEGEND_MAP_Y (2u)     // Start Win Y for color key board letters
#define HELP_LEGEND_MAP_Y_GAP (1u) // Gap between color key board letters in case more space is needed        
#undef HELP_LEGEND_DIALOG_WIN_Y
#define HELP_LEGEND_DIALOG_WIN_Y  DIALOG_TEXT_HEIGHT_LINES(9)
#undef __HELP_LEGEND_DIALOG_STR
#define __HELP_LEGEND_DIALOG_STR \
/*  |----------------XX| Available space (16 chars wide, 2 reserved for letter legend tiles) */\
    "CLAVE:\n" \
    "\n" \
    "FALLO ..........\n" \
    "\n" \
    "POSICION\n"\
    "  INCORRECTA ...\n" \
    "\n" \
    "ACIERTO ........"


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
#define OPTIONS_MENU_DIALOG_WIN_Y  DIALOG_TEXT_HEIGHT_LINES(15)
#undef __OPTIONS_MENU_STR
#define __OPTIONS_MENU_STR \
/*  |------------------| Available space (18 chars wide) */\
    " ... OPCIONES ...\n" \
    "\n" \
    " SALIR DEL MENU\n" \
    "\n" \
    " MODO DIFICIL .. ^\n" \
    " AUTORELLENAR .. ^\n" \
    " SALTAR\n" \
    "   COMPLETADAS . ^\n" \
    " SONIDO FX ..... ^\n" \
    "\n" \
    " VER STATS\n" \
    " ABANDONAR\n" \
    " BORRAR STATS\n" \
    "\n" \
    " AYUDA"

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
#define MENU_Y_SOUND_FX      8
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
    "CONFIRMAR:^\n" \
    "CANCELAR: OTRO\n"\
    "..................\n" \
    "\n" \

#undef __CONFIRM_FORFEIT_STR
#define __CONFIRM_FORFEIT_STR     "ABANDONAR?"

#undef __CONFIRM_STATS_RESET_STR
#define __CONFIRM_STATS_RESET_STR "BORRAR STATS?"


// Display hard mode indicator at top of screen
#undef OPT_HARDMODE_DISPLAY_X
#undef OPT_HARDMODE_DISPLAY_Y
#define OPT_HARDMODE_DISPLAY_X 5
#define OPT_HARDMODE_DISPLAY_Y 0

#undef __STR_HARD_MODE_DISPLAY
#undef __STR_HARD_MODE_HIDE
#define __STR_HARD_MODE_DISPLAY "DIFICIL"
#define __STR_HARD_MODE_HIDE    "       "


#undef __MESSAGE_HARD_MODE_CANT_CHANGE_STR
#define __MESSAGE_HARD_MODE_CANT_CHANGE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "MODO DIFICIL: SOLO\n" \
    "DISPONIBLE AL\n" \
    "COMENZAR RONDA"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_HARD_MODE_GUESS_NOT_VALID_STR
#define __MESSAGE_HARD_MODE_GUESS_NOT_VALID_STR \
/*  |------------------| Available space (18 chars wide) */\
    "MODO DIFICIL:\n" \
    "DEBES INCLUIR\n" \
    "TODAS LAS PISTAS"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_LOSE_STR
#define __MESSAGE_LOSE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "Pierdes!\n" \
    "\n" \
    "Solucion: "    // <-- Needs 5 chars left for answer word!


// NOTE! ^ gets replaced with guess 5 letter word num
//
// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_GAME_WON_STR
#define __MESSAGE_GAME_WON_STR \
/*  |------------------| Available space (18 chars wide) */\
    "GANAS!\n" \
    "\n" \
    "INTENTO ^ DE 6"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_WORD_TOO_SHORT_STR
#define __MESSAGE_WORD_TOO_SHORT_STR \
/*  |------------------| Available space (18 chars wide) */\
    "Demasiado corta!\n" \
    "\n" \
    "5 Letras Min."


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_WORD_NOT_IN_DICT_STR
#define __MESSAGE_WORD_NOT_IN_DICT_STR \
/*  |------------------| Available space (18 chars wide) */\
    "No existe en el\n" \
    "\n" \
    "diccionario!"


#undef __MESSAGE_STATS_RESET_STR
#define __MESSAGE_STATS_RESET_STR \
/*  |------------------| Available space (18 chars wide) */\
    "STATS BORRADAS!"


#undef __STATS_TEMPLATE_STR
#define __STATS_TEMPLATE_STR \
/*  |------------------| Available space (18 chars wide) */\
    " Jugadas ... ____^\n" \
    " Racha ..... ____^\n" \
    " Ganadas ... ____^\n" \
    " Perdidas .. ____^\n" \
    "\n" \
    "Numero de Intentos\n" \
    " Minimo  ... ____^\n" \
    " Maximo  ... ____^\n" \
    " Media...... ____^\n"   // <-- All "____^" get replaced by numbers

// Should be no more than 18 chars
#undef __INTRO_PRESS_START_STR
#undef __INTRO_PRESS_START_STR_BLANK
#define __INTRO_PRESS_START_STR       "PULSA START"
#define __INTRO_PRESS_START_STR_BLANK "           "

// X, Y controls upper-left corner coordinates
#undef __INTRO_CREDITS_STR
#define __INTRO_CREDITS_STR \
/*  |--------------------| */\
    "GRACIAS:\n" \
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

