// lang_text.h

#ifndef _LANG_TEXT_H
#define _LANG_TEXT_H


// == This has default strings
#include "../lang_text_defaults.inc"


// == #define overrides for language strings go here

// Brazilian Portuguese: Translation by Daniel Tolentino

#undef INTRO_DIALOG_WIN_Y
#define INTRO_DIALOG_WIN_Y DIALOG_TEXT_HEIGHT_LINES(15)
#undef __INTRO_MESSAGE_STR
#define __INTRO_MESSAGE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "IDIOMA ... " STR(LANG_CODE) "\n" \
    "  A..Z  APENAS\n" \
    "  SEM CAR. ESPEC.\n" \
    "\n" \
    "ABRIR MENU:\n" \
    "  3 x SELECT\n" \
    "\n" \
    "CURSOR DOS BLOCOS:\n" \
    "  SELECT + B A\n" \
    "\n" \
    "PREENCHER:\n" \
    "  SELECT + START\n" \
    "\n" \
    "APERTE QUALQUER\n" \
    "  TECLA"


// #undef HELP_LEGEND_MAP_Y
// #undef HELP_LEGEND_MAP_Y_GAP
// #define HELP_LEGEND_MAP_Y (2u)     // Start Win Y for color key board letters
// #define HELP_LEGEND_MAP_Y_GAP (0u) // Gap between color key board letters in case more space is needed        
#undef HELP_LEGEND_DIALOG_WIN_Y
#define HELP_LEGEND_DIALOG_WIN_Y  DIALOG_TEXT_HEIGHT_LINES(7)
#undef __HELP_LEGEND_DIALOG_STR
#define __HELP_LEGEND_DIALOG_STR \
/*  |----------------XX| Available space (16 chars wide, 2 reserved for letter legend tiles) */\
    "TECLA DE CORES:\n" \
    "\n" \
    "NO CORRESPONDER.\n" \
    "\n" \
    "LUGAR ERRADO ...\n" \
    "\n" \
    "CORRESPONDER ..."


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
    "  ... OPCOES ...\n" \
    "\n" \
    " SAIR DO MENU\n" \
    "\n" \
    " MODO DIFICIL .. ^\n" \
    " AUTOPREENCHER . ^\n" \
    " SALTAR\n" \
    "   AUTOPREENCHER ^\n" \
    " SONOROS ....... ^\n" \
    "\n" \
    " VER STATS\n" \
    " SAIR\n" \
    " APAGAR STATS\n" \
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
    "CANCELAR: OUTRA\n"\
    "..................\n" \
    "\n" \

#undef __CONFIRM_FORFEIT_STR
#define __CONFIRM_FORFEIT_STR     "SAIR?"

#undef __CONFIRM_STATS_RESET_STR
#define __CONFIRM_STATS_RESET_STR "APAGAR STATS?"


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
    "MUDA APENAS EM\n" \
    "NOVA RODADA"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_HARD_MODE_GUESS_NOT_VALID_STR
#define __MESSAGE_HARD_MODE_GUESS_NOT_VALID_STR \
/*  |------------------| Available space (18 chars wide) */\
    "MODO DIFICIL:\n" \
    "DEVE INCLUIR\n" \
    "DICAS REVELADAS"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_LOSE_STR
#define __MESSAGE_LOSE_STR \
/*  |------------------| Available space (18 chars wide) */\
    "Perdeu!\n" \
    "\n" \
    "Resposta: "    // <-- Needs 5 chars left for answer word!


// NOTE! ^ gets replaced with guess 5 letter word num
//
// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_GAME_WON_STR
#define __MESSAGE_GAME_WON_STR \
/*  |------------------| Available space (18 chars wide) */\
    "Ganhou!\n" \
    "\n" \
    "Tentativa ^ DE 6"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_WORD_TOO_SHORT_STR
#define __MESSAGE_WORD_TOO_SHORT_STR \
/*  |------------------| Available space (18 chars wide) */\
    "Termo muito curto!\n" \
    "\n" \
    "Min. de 5 Letras"


// Avoid increasing this popup height so it doesn't obscure the board
#undef __MESSAGE_WORD_NOT_IN_DICT_STR
#define __MESSAGE_WORD_NOT_IN_DICT_STR \
/*  |------------------| Available space (18 chars wide) */\
    "Palava nao esta no\n" \
    "\n" \
    "dicionario!"


#undef __MESSAGE_STATS_RESET_STR
#define __MESSAGE_STATS_RESET_STR \
/*  |------------------| Available space (18 chars wide) */\
    "STATS APAGADOS!"


#undef __STATS_TEMPLATE_STR
#define __STATS_TEMPLATE_STR \
/*  |------------------| Available space (18 chars wide) */\
    " Jogadas ... ____^\n" \
    " Seguida ... ____^\n" \
    " Vitoria ... ____^\n" \
    " Perdidas .. ____^\n" \
    "\n" \
    "Numero de Jogadas\n" \
    " Minimo  ... ____^\n" \
    " Maximo  ... ____^\n" \
    " Media...... ____^\n"   // <-- All "____^" get replaced by numbers

// Should be no more than 18 chars
#undef __INTRO_PRESS_START_STR
#undef __INTRO_PRESS_START_STR_BLANK
#define __INTRO_PRESS_START_STR       "APERTE START"
#define __INTRO_PRESS_START_STR_BLANK "            "


// X, Y controls upper-left corner coordinates
#undef __INTRO_CREDITS_STR
#define __INTRO_CREDITS_STR \
/*  |--------------------| */\
    "OBRIGADO a:\n" \
    "TOXA\n" \
    "DAEO\n" \
    "ARPRUSS\n" \
    "COFFEE 'VALEN' BAT\n" \
    "STACKSMASHING\n" \
    "ZERAPHY\n" \
    "\n" \
    "GBDK2020\n" \
    "EMULICIOUS\n" \
    "FERRANTE CRAFTS"


#endif
