// gfx_common.c

#include <gbdk/platform.h>
#include <gbdk/incbin.h>
#include <gb/gbdecompress.h>

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "common.h"
#include "gfx_common.h"
#include "gameboy_color.h"

#include "window.h"

#include "lang_text.h"

// Printing cursor position, updated by print_gotoxy
uint8_t * print_vram_addr = NULL;

uint8_t map_decomp_buf[DEVICE_SCREEN_WIDTH * DEVICE_SCREEN_HEIGHT];

uint8_t board_letters_decomp_buf[BOARD_LETTERS_COUNT * BOARD_LETTERS_TILES_PER * BOARD_LETTERS_BYTES_PER_TILE];
uint8_t font_letters_decomp_buf[FONT_LETTERS_COUNT * FONT_LETTERS_BYTES_PER_TILE];

// Cursor sprite flipping flags to allow use of same tile for all 4 corners
// const uint8_t sp_cursor_kbd_props[] = { 0x00, S_FLIPX, S_FLIPY, S_FLIPX | S_FLIPY };
// const uint8_t sp_cursor_offset_x[] = { 0, 8, 0, 8 };
// const uint8_t sp_cursor_offset_y[] = { 0, 0, 8, 8 };


void gfx_clear_bkg_blank_tile(void) {
    fill_bkg_rect(0, 0, DEVICE_SCREEN_WIDTH, DEVICE_SCREEN_HEIGHT, BG_TILES_BLANK_START);
}

void sprites_hide_all_offscreen(void) {
    for (uint8_t c = 0u; c < SP_ID_COUNT_TOTAL; c++)
        hide_sprite(c);
}


// Blank tile, could be optimized
const uint8_t tile_blank[] = {0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00};


// Move printing cursor on screen
void print_gotoxy(uint8_t x, uint8_t y, uint8_t target) {

    if (target == PRINT_BKG)
        print_vram_addr = get_bkg_xy_addr(x,y);
    else
        print_vram_addr = get_win_xy_addr(x,y);
}


// Print a string on background or window at location set by print_gotoxy()
void print_str(const char * txt) {

    uint8_t src_chr;
    uint8_t letter;
    uint8_t line_wrap_x_addr = (((uint8_t)print_vram_addr) & 0x1Fu);// (uint8_t)((uint16_t)print_vram_addr & 0x001Fu);    

    // src_chr = *txt++; // Next character
    // while(src_chr) {
    // SIZE: this loop structure saves about 60 bytes vs above
    while (1) {
        src_chr = *txt++; // Next character
        if (!src_chr) return;

        if (src_chr == '\n') {
            // Mask out X location, reset it to line wrap location, move to next line
            print_vram_addr = (uint16_t *)(((uint16_t)print_vram_addr & 0xFFE0u) + (line_wrap_x_addr + 0x20u));
            // src_chr = *txt++; // Next character
            continue;
        }
        else if (src_chr >= 'A' && src_chr <= 'Z') {
            letter = BG_TILES_FONT_START + (unsigned char)(src_chr - 'A');
        } else if (src_chr >= 'a' && src_chr <= 'z') {
            letter = BG_TILES_FONT_START + (unsigned char)(src_chr - 'a');
        } else if (src_chr >= '0' && src_chr <= '9') {
            letter = BG_TILES_FONT_NUM_START + (unsigned char)(src_chr - '0');
        } else {
            switch (src_chr) {
                // The top two don't work due to signed char
                // case 'ç': // Fall Through to single quote char
                // case 'Ç': // Fall Through to single quote char
                case CEDILLA_MAPPED_CHR: letter = BG_TILES_FONT_CEDILLA; break;
                case '\"': // Fall Through to single quote char
                case '\'': letter = BG_TILES_FONT_QUOTE; break;
                case '.': letter = BG_TILES_FONT_PERIOD; break;
                case '!': letter = BG_TILES_FONT_EXCLAIM; break;
                case '?': letter = BG_TILES_FONT_QMARK; break;
                case ':': letter = BG_TILES_FONT_COLON; break;
                case '+': letter = BG_TILES_FONT_PLUS; break;
                case '<': letter = BG_TILES_FONT_BUTTON_L; break;
                case '>': letter = BG_TILES_FONT_BUTTON_R; break;
                case '^': letter = BG_TILES_FONT_BUTTON_U; break;
                case '/': letter = BG_TILES_FONT_BUTTON_D; break;

                case CHECKBOX_ON_CHR:  letter = BG_TILES_FONT_CHECKBOX_ON; break;
                case CHECKBOX_OFF_CHR: letter = BG_TILES_FONT_CHECKBOX_OFF; break;

                // Default is blank tile for Space or any other unknown chars
                default:  letter = BG_TILES_BLANK_START; break;
            }
        }

        set_vram_byte(print_vram_addr++, letter);
        // src_chr = *txt++; // Next character
    }
}


// Draw hard mode status indicator if enabled, otherwise overwrite it with spaces
void opt_hardmode_display(void) {

    print_gotoxy(OPT_HARDMODE_DISPLAY_X, OPT_HARDMODE_DISPLAY_Y, PRINT_BKG);
    print_str(game_settings.opt_hard_mode_enabled ? __STR_HARD_MODE_DISPLAY : __STR_HARD_MODE_HIDE);
}


// Direct render a keyboard letter to VRAM to fill a keyboard entry
// Previous calls to set_1bpp_colors() will affect colors produced here
// Keyboard entries are 1 byte per entry on the map
void draw_letter_to_tileid(uint8_t letter, uint8_t index) {

    // if ((letter == ' ') || (letter == 0x00)) letter = BOARD_LETTERS_SPACE_CHAR;
    if (letter >= 'a') letter -= 'a';
    else if (letter >= 'A') letter -= 'A';

    // Draw letter into VRAM address for desired keyboard tile
    set_bkg_1bpp_data(index, 1, font_letters_decomp_buf + (letter * FONT_LETTERS_BYTES_PER_TILE));
}


// Set up graphics
void gfx_load(void) {

    uint8_t c;

    // Map Data
    SET_PRINT_COLOR_NORMAL;

    // Load 2bpp blank tile
    set_bkg_data((BG_TILES_BLANK_START), 1u, tile_blank);


    // == Font Numbers ==
    //
    // ** borrows font_letters_decomp_buf temporarily before it's actual use **
    //
    // Load 1bpp font num tiles
    gb_decompress(font_num_tiles, font_letters_decomp_buf);
    // Load tiles into vram for font printing
    set_bkg_1bpp_data(BG_TILES_FONT_NUM_START, BG_TILES_FONT_NUM_LEN, font_letters_decomp_buf);


    // == Dialog Window Tiles / Squares ==
    //
    // ** borrows board_letters_decomp_buf temporarily before it's actual use **
    //
    // Load 1bpp intro / window dialog tiles
    gb_decompress(intro_dialog_tiles, board_letters_decomp_buf);
    // Load tiles into vram
    set_bkg_1bpp_data(BG_TILES_INTRO_DIALOG_START, BG_TILES_INTRO_DIALOG_LEN, board_letters_decomp_buf);
    // Initialize Window dialog box
    gb_decompress(intro_dialog_map, map_decomp_buf);
    win_dialog_draw();


    // == Font Letters ==
    // Load 1bpp font tiles (used by both keyboard for VRAM drawing and print as a VRAM tileset)
    gb_decompress(font_tiles, font_letters_decomp_buf);
    // Load tiles into vram for font printing
    set_bkg_1bpp_data(BG_TILES_FONT_START, BG_TILES_FONT_LEN, font_letters_decomp_buf);

    // == Game Board Letter Tiles / Squares ==
    // Decompress board letter tiles into a buffer, they get written to VRAM later as needed
    gb_decompress(board_letter_tiles, board_letters_decomp_buf);



    // ==== Cursors ====

    // Shared Cursor Sprite Tile Data
    // Load 4bpp gbcompressed sprite cursor data
    gb_decompress_sprite_data((SP_TILES_CURSOR_START), cursor_tiles);
    // set_sprite_data((SP_TILES_CURSOR_START), SP_TILES_CURSOR_COUNT_TOTAL, cursor_tiles);

    SPRITES_8x8;

    // Keyboard Cursor
    set_sprite_tile(SP_ID_CURSOR_KBD_START + 0u, SP_TILES_CURSOR_KBD_START);
    set_sprite_tile(SP_ID_CURSOR_KBD_START + 1u, SP_TILES_CURSOR_KBD_START);
    set_sprite_tile(SP_ID_CURSOR_KBD_START + 2u, SP_TILES_CURSOR_KBD_START);
    set_sprite_tile(SP_ID_CURSOR_KBD_START + 3u, SP_TILES_CURSOR_KBD_START);

    set_sprite_prop(SP_ID_CURSOR_KBD_START + 0u, 0x00u);
    set_sprite_prop(SP_ID_CURSOR_KBD_START + 1u, S_FLIPX);
    set_sprite_prop(SP_ID_CURSOR_KBD_START + 2u, S_FLIPY);
    set_sprite_prop(SP_ID_CURSOR_KBD_START + 3u, S_FLIPX | S_FLIPY);

    // Board Row Cursor
    c = SP_TILES_CURSOR_BOARD_START;
    for (uint8_t i = SP_ID_CURSOR_BOARD_START; i < (SP_ID_CURSOR_BOARD_START + SP_ID_CURSOR_BOARD_LEN); i++) {
        set_sprite_tile(i, c++);
        set_sprite_prop(i, CGB_PAL_3);
    }

    // Letter cursor (as dotted frame)
    set_sprite_tile(SP_ID_CURSOR_LETTER_START + 0u, SP_TILES_CURSOR_LETTER_START);
    set_sprite_tile(SP_ID_CURSOR_LETTER_START + 1u, SP_TILES_CURSOR_LETTER_START + 1u);
    set_sprite_tile(SP_ID_CURSOR_LETTER_START + 2u, SP_TILES_CURSOR_LETTER_START + 1u);
    set_sprite_tile(SP_ID_CURSOR_LETTER_START + 3u, SP_TILES_CURSOR_LETTER_START);

    set_sprite_prop(SP_ID_CURSOR_LETTER_START + 0u, OAMF_PAL1 | CGB_PAL_WHITE_2);
    set_sprite_prop(SP_ID_CURSOR_LETTER_START + 1u, OAMF_PAL1 | CGB_PAL_WHITE_2); // | S_FLIPX);
    set_sprite_prop(SP_ID_CURSOR_LETTER_START + 2u, OAMF_PAL1 | CGB_PAL_WHITE_2 | S_FLIPX | S_FLIPY); //| S_FLIPY);
    set_sprite_prop(SP_ID_CURSOR_LETTER_START + 3u, OAMF_PAL1 | CGB_PAL_WHITE_2 | S_FLIPX | S_FLIPY);


    // Menu Cursor
    set_sprite_tile(SP_ID_CURSOR_MENU_START + 0u, SP_TILES_CURSOR_MENU_START);
    set_sprite_tile(SP_ID_CURSOR_MENU_START + 1u, SP_TILES_CURSOR_MENU_START);

    set_sprite_prop(SP_ID_CURSOR_MENU_START + 0u, 0x00u);
    set_sprite_prop(SP_ID_CURSOR_MENU_START + 1u, S_FLIPY);


    // Moved this to Main so it could happen before 31k+1k logo splash
    //
    // // Clear window and move it offscreen at the bottom
    // move_win(0 + WIN_X_OFFSET, DEVICE_SCREEN_PX_HEIGHT); // Window is offscreen by default

    // Center screen by scrolling slightly to the left
    // move_bkg(252, 252);
    move_bkg(252, 0);
}
