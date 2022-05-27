/*

CoffeeBaT-FX (CBT-FX)

Very simple sound effect """""Driver""""" by Coffee Bat

...And as the name implies it is very close to CBT to make the sfx...

Also thanks to bbbbbr for getting my code to ACTUALLY work

*/
#include <gb/gb.h>
#include "cbtfx.h"
// #include "hUGEDriver.h"
// #define MUSIC_DRIVER_CH2_ON hUGE_mute_channel(HT_CH2, 0);
// #define MUSIC_DRIVER_CH2_OFF hUGE_mute_channel(HT_CH2, 1);
// #define MUSIC_DRIVER_CH4_ON hUGE_mute_channel(HT_CH4, 0);
// #define MUSIC_DRIVER_CH4_OFF hUGE_mute_channel(HT_CH4, 1);

const unsigned char CBTFX_HEADER[] = "CBT-FX BY COFFEEBAT 2021-2022";
const uint8_t * CBTFX_pointer;
uint8_t CBTFX_size = 0;
uint8_t CBTFX_repeater = 0;
uint8_t CBTFX_panning = 0;
uint8_t CBTFX_priority = 0;
uint8_t CBTFX_ch_used = 0;

//Restart values and point to the new sfx
void CBTFX_init(const unsigned char * SFX) NONBANKED {
    if (*SFX & 0x0f < CBTFX_priority) return;
    // To avoid hanging notes
    if (CBTFX_ch_used & 128) NR21_REG = NR22_REG = NR23_REG = NR24_REG = 0;
    if (CBTFX_ch_used & 32) NR41_REG = NR42_REG = NR43_REG = NR44_REG = 0;
    CBTFX_priority = *SFX & 0x0f;
    CBTFX_ch_used = *SFX++;
    CBTFX_size = *SFX++;
    CBTFX_repeater = 0;
    CBTFX_pointer = SFX;
    // if (CBTFX_ch_used & 128) MUSIC_DRIVER_CH2_OFF;
    // if (CBTFX_ch_used & 32) MUSIC_DRIVER_CH4_OFF;
}


#ifndef CPTFX_UPDATE_ASM

void CBTFX_update(void) NONBANKED {
    if (CBTFX_size != 0){ // If we have an SFX to play...

    	if(CBTFX_repeater != 0){ // If we are still playing a frame
    		CBTFX_repeater--; // Remove one from the frame counter
    	}else{

        	CBTFX_repeater = *CBTFX_pointer++; // Load the frame's length

            uint8_t mask = 0; // Mask to avoid muting an unused channel
            if (CBTFX_ch_used & 128) mask |= 0x22;
            if (CBTFX_ch_used & 32) mask |= 0x88;
            NR51_REG &= ~mask; // Mask out the CH2 and CH4 pan values
            NR51_REG |= mask & *CBTFX_pointer++; // And write ours

            if (CBTFX_ch_used & 128){
                NR21_REG = *CBTFX_pointer++;
                NR22_REG = *CBTFX_pointer & 0xf0; // To assure the envelope is set to 0
            }

            if (CBTFX_ch_used & 32){
                NR42_REG = *CBTFX_pointer << 4; // Volume for the noise channel is the lower 4 bits of the same byte
            }

            CBTFX_pointer++;

            // If CH2 isn't used, we omit this data
            if (CBTFX_ch_used & 128) {
                NR23_REG = *CBTFX_pointer++; // The lower 8 bits of the frequency
                NR24_REG = *CBTFX_pointer++; // Higher 3 bits of the frequency + making sure the length isn't used and triggering the channel
            }

            // If CH4 isn't used, we omit this data
            if (CBTFX_ch_used & 32) {
                NR43_REG = *CBTFX_pointer++; // Noise freq
                NR44_REG = 0x80; // Trigger the noise channel
            }

            CBTFX_size--;

            if(CBTFX_size == 0){
           		CBTFX_priority = 0;
                if (CBTFX_ch_used & 128){
//                    MUSIC_DRIVER_CH2_ON;
                    NR21_REG = NR22_REG = NR23_REG = NR24_REG = 0;
                }
                if (CBTFX_ch_used & 32){
//                    MUSIC_DRIVER_CH4_ON;
                    NR41_REG = NR42_REG = NR43_REG = NR44_REG = 0;
                }
            }

        }
    }
}


#else // CPTFX_UPDATE_ASM


void CBTFX_update_isr(void) __interrupt __naked {
// void CBTFX_update_asm(void) NONBANKED {

    __asm \

        push af
        push hl
        push bc

        ; // if (CBTFX_size != 0){ // If we have an SFX to play...
        ld  a, (#_CBTFX_size)
        or  a, a
        jp  Z, .cbtfx_done

        ; // if(CBTFX_repeater != 0){ // If we are still playing a frame
        ld  a, (#_CBTFX_repeater)
        or  a, a
        jr  Z, .cbtfx_play_update

            ; // Still repeating, decrement and exit
            // CBTFX_repeater--; // Remove one from the frame counter
            dec a
            ld  (#_CBTFX_repeater), a
            jp  .cbtfx_done


        // Continue playing current FX
        .cbtfx_play_update:
            ; // Load _CBTFX_pointer into HL
            ; //
            ; // Load the frame's length
            ; // CBTFX_repeater = *CBTFX_pointer++;
            ld  a, (_CBTFX_pointer + 0)
            ld  l, a
            ld  a, (_CBTFX_pointer + 1)
            ld  h, a

            ld  a, (hl+)
            ld  (#_CBTFX_repeater),a

            ; // mask -> A
            ; //
            ; // Mask to avoid muting an unused channel
            ; // uint8_t mask = 0;
            ; // if (CBTFX_ch_used & 128) mask |= 0x22;
            ; // if (CBTFX_ch_used & 32) mask |= 0x88;
            ; // duplicate mask bits into LSBits
            ld  a, (#_CBTFX_ch_used)

            ;// This is a shorter version of code commented out below
            and  #(0x80 | 0x20)
            srl  a
            srl  a
            ld   b, a
            swap a
            or   b

                ;// ld  c, a
                ;// xor a
                ;//
                ;// bit 7, c
                ;//     jr  Z, .cbtfx_skip_ch2_skip_mask
                ;//     or  a, #0x22
                ;// .cbtfx_skip_ch2_skip_mask:

                ;// bit 5, c
                ;// jr  Z, .cbtfx_skip_ch4_skip_mask
                ;//     or  a, #0x88
                ;// .cbtfx_skip_ch4_skip_mask:

            ; // Mask out the CH2 and CH4 pan values
            ; // src/cbtfx.c:55: NR51_REG &= ~mask;
            ; //  mask -> C
            ; // (NR51_REG &= ~mask) -> B
            ld   c, a
            cpl
            ld   b, a
            ldh  a, (_NR51_REG + 0)
            and  a, b
            ld   b, a
                ; // TODO: does this have to be a separate write or can it be combined
                ; // ldh  (_NR51_REG + 0), a

            ; // And write ours
            ; //src/cbtfx.c:56: NR51_REG |= mask & *CBTFX_pointer++;
            ld   a, (hl+)
            and  a, c
            or   a, b
            ldh  (_NR51_REG + 0), a

            ; // if (CBTFX_ch_used & 128){
            ; //     NR21_REG = *CBTFX_pointer++;
            ; //     NR22_REG = *CBTFX_pointer & 0xf0; // To assure the envelope is set to 0
            ; // }
            ld  a, (#_CBTFX_ch_used)
            ld  c, a

            bit 7, c
            jr  Z, .cbtfx_skip_ch2_env_zero

                ld   a, (hl+)
                ldh  (_NR21_REG + 0), a

                ld   a, (hl)
                and  a, #0xF0
                ldh  (_NR22_REG + 0), a
            .cbtfx_skip_ch2_env_zero:

            ; // if (CBTFX_ch_used & 32){
            ; //     NR42_REG = *CBTFX_pointer << 4; // Volume for the noise channel is the lower 4 bits of the same byte
            ; // }
            bit 5, c
            jr  Z, .cbtfx_skip_ch4_vol

                ld   a, (hl)
                swap a
                and  #0xF0
                ldh  (_NR42_REG + 0), a
            .cbtfx_skip_ch4_vol:

            ; // CBTFX_pointer++;
            inc  hl

            ; // If CH2 isn't used, we omit this data
            ; // if (CBTFX_ch_used & 128) {
            ; //     NR23_REG = *CBTFX_pointer++; // The lower 8 bits of the frequency
            ; //     NR24_REG = *CBTFX_pointer++; // Higher 3 bits of the frequency + making sure the length isn't used and triggering the channel
            ; // }
            bit 7, c
            jr  Z, .cbtfx_skip_ch2_data

                ld   a, (hl+)
                ldh  (_NR23_REG + 0), a

                ld   a, (hl+)
                ldh  (_NR24_REG + 0), a
            .cbtfx_skip_ch2_data:

            ; // If CH4 isn't used, we omit this data
            ; // if (CBTFX_ch_used & 32) {
            ; //     NR43_REG = *CBTFX_pointer++; // Noise freq
            ; //     NR44_REG = 0x80; // Trigger the noise channel
            ; // }
            bit 5, c
            jr  Z, .cbtfx_skip_ch4_data

                ld   a, (hl+)
                ldh  (_NR43_REG + 0), a

                ld   a, #0x80
                ldh  (_NR44_REG + 0), a
            .cbtfx_skip_ch4_data:


            ; // CBTFX_size--;
            ; // if(CBTFX_size == 0){
            ld  a, (#_CBTFX_size)
            dec a
            ld  (#_CBTFX_size), a
            jp  NZ, .cbtfx_play_update_done

                xor a

                ; // if(CBTFX_size == 0){
                ; //     CBTFX_priority = 0;
                ld  (#_CBTFX_priority), a

                ; // if (CBTFX_ch_used & 128){
                ; //     NR21_REG = NR22_REG = NR23_REG = NR24_REG = 0;
                ; // }
                bit 7, c
                jr  Z, .cbtfx_skip_ch2_zero

                    ldh  (_NR21_REG + 0), a
                    ldh  (_NR22_REG + 0), a
                    ldh  (_NR23_REG + 0), a
                    ldh  (_NR24_REG + 0), a
                .cbtfx_skip_ch2_zero:

                ; // if (CBTFX_ch_used & 32){
                ; //     NR41_REG = NR42_REG = NR43_REG = NR44_REG = 0;
                ; // }
                bit 5, c
                jr  Z, .cbtfx_skip_ch4_zero

                    ldh  (_NR41_REG + 0), a
                    ldh  (_NR42_REG + 0), a
                    ldh  (_NR43_REG + 0), a
                    ldh  (_NR44_REG + 0), a
                .cbtfx_skip_ch4_zero:

            .cbtfx_play_update_done:
            ; // Store updated CBT pointer address
            ld  a, l
            ld  (_CBTFX_pointer + 0), a
            ld  a, h
            ld  (_CBTFX_pointer + 1), a

        .cbtfx_done:

        pop bc
        pop hl
        pop af

        ret
    __endasm;

}

#endif