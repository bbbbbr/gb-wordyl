#include <stdint.h>
#include <stdbool.h>
#include <stdbool.h>

#include <asm/types.h>
#include "common.h"

#include "decode.h"

#include <gbdk/gbdecompress.h>
#include "answerbitmap_rle.h"
#include "encoded.h"
#include "sizes.h"

#define CEDILLA_CHR_NUM 26 // a = 0, z = 25, ç = 26 ('{', the letter after lowercase 'z')

//  ====== ENCODE / DECODE Settings ======

#define _ASM_GETSPECIALWORD

// #define _ASM_UPDATEWORD_7BIT_VARINT
#define _UPDATEWORD_3BIT_VARINT

// #define ALPHABET_REMAP
// #define WORD_LETTERS_REVERSED

// #define NO_ZERO_DELTA_SUBTRACT
#define YES_ZERO_DELTA_SUBTRACT

// #define _3BIT_VARINT_LSBITS_FIRST
#define _3BIT_VARINT_MSBITS_FIRST

//  ======================================

uint8_t answerBitmap[rle_answerBitmapIndex_sz_decomp];

void initDict(void) {
    gb_decompress(rle_answerBitmapIndex, answerBitmap);
}


#define DICT_BUCKET_EOF  0xFFu

static const dictIndexBucket_t * p_dictIndex;
static uint16_t bucketWordCount;
static uint16_t bucketStart_BlobOffset;
static uint32_t bucketStart_currentWord;

static uint32_t       currentWord;
static const uint8_t* blobPtr;
static char  *        str_return_buffer;

static bool dict_two_nybbles_queued;
static uint8_t dict_cur_byte;


#ifdef TEST_DICT_ENABLED

#include <gbdk/emu_debug.h> // Use this include to add the Emu debug functions

    void dumpTestToEmuConsole(void) {
        static char tempStr[10];
        bool validated;

        #if defined(TEST_ANSWERWORDS)

            // Decode all answer words words and log them to emu debug console.
            // Validate each word against the dictionary and warn if it fails
            for (uint16_t c = 0; c < NUM_ANSWERS; c++) {
                //str_return_buffer = tempStr;

                getSpecialWord(c, tempStr);
                EMU_printf("%s", tempStr);

                validated = filterWord(tempStr);
                if (!validated)
                    EMU_MESSAGE("Rejected by filterWord()");
            }

        #elif defined (TEST_ALLWORDS)
            // Decode all dictionary words and log them to emu debug console.
            // Validate each word against the dictionary and warn if it fails
            for (uint16_t c = 0; c < NUM_WORDS; c++) {
                str_return_buffer = tempStr;

                getWord(c);
                EMU_printf("%s", tempStr);

                validated = filterWord(tempStr);
                if (!validated)
                    EMU_MESSAGE("Rejected by filterWord()");
            }

        #elif defined (TEST_BENCHMARK)
            // Benchmark retreival and validation for each word in the dictionary
            for (uint16_t c = 0; c < NUM_WORDS; c++) {
                str_return_buffer = tempStr;

                EMU_MESSAGE("%ZEROCLKS%");
                getWord(c);
                EMU_MESSAGE("%-8+LASTCLKS%");
                EMU_printf("%s", tempStr);

                EMU_MESSAGE("%ZEROCLKS%");
                validated = filterWord(tempStr);
                EMU_MESSAGE("%-8+LASTCLKS%");
                if (!validated)
                    EMU_MESSAGE("Rejected by filterWord()");
            }
        #endif

    }
#endif



#ifdef _ASM_UPDATEWORD_7BIT_VARINT

// high byte of currentWord is always zero
void updateWord_7bit_varint(void) OLDCALL {
__asm
    ld a,(#_blobPtr)
    ld l,a
    ld a,(#_blobPtr+1)
    ld h,a
    ld a,(hl+)
    bit 7,a
    jr z,0001$
    and #127
    ld b,a
    xor a
    ld d,a
0005$:
    ld e,a
0003$:
;; inc edb
    ld a,b
    add #1
    ld b,a
    ld a,d
    adc #0
    ld d,a
    ld a,e
    adc #0
    ld e,a
;; add edb to currentWord
    ld a,(#_currentWord)
    add b
    ld (#_currentWord),a
    ld a,(#_currentWord+1)
    adc d
    ld (#_currentWord+1),a
    ld a,(#_currentWord+2)
    adc e
    ld (#_currentWord+2),a
;; update blobPtr
    ld a,l
    ld (#_blobPtr),a
    ld a,h
    ld (#_blobPtr+1),a
    ret
0001$:
    ld b,a
    ld a,(hl+)
    ld c,a
    and #127
    ld d,a
    srl d
    sla a
    sla a
    sla a
    sla a
    sla a
    sla a
    sla a
    or b
    ld b,a
    bit 7,c
    jr z,0002$
    xor a
    jr 0005$
0002$:
    ld a,(hl+)
    ld e,a
    srl e
    srl e
    sla a
    sla a
    sla a
    sla a
    sla a
    sla a
    or d
    ld d,a
    jr 0003$
__endasm ;
}

#elif defined (_UPDATEWORD_3BIT_VARINT)


// V4 - Expects HIGHEST bits (chunks) to be packed first, highest last
uint32_t update_v;

// dict_two_nybbles_queued and dict_cur_byte need to be
// primed once at the start of the decode loop
void updateWord_3bit_varint(void) OLDCALL {

    update_v = 0;
    bool first_pass = true;
    bool loop_done = false;

    // Merge in 3 bit varints until the "more nybbles" flag is not set
    do {
        if (!first_pass)
            update_v <<= 3;
        else
            first_pass = false;

        // Merge in new bits to lowest byte of u32 only
        *(((uint8_t *)(&update_v)) + 0) |= (dict_cur_byte & 0x07);

        // Exit loop (after loading next byte/nybble)
        // if this is the last varint nybble for the word
        if ((dict_cur_byte & 0x08) == 0)
            loop_done = true;

        // If a nybble is queued then move it into the lower 4 bits
        // Otherwise read a new byte
        if (dict_two_nybbles_queued)
            dict_cur_byte >>= 4;
        else
            dict_cur_byte = *++blobPtr;

        dict_two_nybbles_queued = !dict_two_nybbles_queued;
    // };
    } while (!loop_done);

    // Add 1 since all words are encoded as (value - 1)
    #ifdef YES_ZERO_DELTA_SUBTRACT
        currentWord += update_v + 1;
    #else
        currentWord += update_v;
    #endif
}


#else // end #elif defined (_UPDATEWORD_3BIT_VARINT)

void updateWord_7bit_varint(void) OLDCALL {
    uint8_t b = *blobPtr++;
    uint32_t v;
    v = b & 0x7F;
    if (0 == (b & 0x80)) {
        b = *blobPtr++;
        v |= (uint32_t)(b & 0x7F) << 7;
        if (0 == (b&0x80)) {
            v |= (uint32_t)*blobPtr << 14;
            blobPtr++;
        }
    }
    currentWord += v+1;
}

#endif // if _ASM_UPDATEWORD_7BIT_VARINT -> elif ->  else


#ifdef ALPHABET_REMAP
    const char  alpha_unmap[] = "AEIOUSTRYHKBCDFGJLMNPQVWXZ";
    //                             [ ^un-map dir ]
    //                           ABCDEFGHIJKLMNOPQRSTUVWXYZ
    //                             [ map dir \/ ]
    const char alpha_newmap[] = "ALMNBOPJCQKRSTDUVHFGEWXYIZ";
#endif


// Applies alphabet remapping to a character
inline char check_alpha_remap_char(char c) {
    #ifdef ALPHABET_REMAP
        if (c == ('A' + CEDILLA_CHR_NUM))
            return c;
        else
            return alpha_newmap[c - 'A'];
    #else
        return c;
    #endif
}

// Reverse alphabet remapping that was previously applied to a character
inline char check_alpha_unmap_char(char c) {
    #ifdef ALPHABET_REMAP
        if (c == ('A' + CEDILLA_CHR_NUM))
            return c;
        else
            return alpha_unmap[c - 'A'];
    #else
        return c;
    #endif
}


// Converts a word from a dictionary bucket # and numeric value to a string
void decodeWord(uint8_t start, uint32_t nextFour, char* buffer) {

    #ifdef WORD_LETTERS_REVERSED
        // Words encoded with letters in reverse order
        *(buffer+5) = 0;
        // Set last letter from letter bucket index (a-z)
        *(buffer+4) = check_alpha_unmap_char(start + 'A');
        // Decode remaining 4 letters
        for(uint8_t i=1;i<5;i++) {
            *buffer++ = check_alpha_unmap_char((nextFour & 0x1F) + 'A');
            nextFour >>= 5;
        }
    #else
        // Words encoded with normal letter order
        *buffer = check_alpha_unmap_char(start + 'A');
        buffer += 5;
        *buffer-- = 0;
        for(uint8_t i=1;i<5;i++) {
            *buffer-- = check_alpha_unmap_char((nextFour & 0x1F) + 'A');
            nextFour >>= 5;
        }
    #endif
}


#ifdef _UPDATEWORD_3BIT_VARINT



void loadDictBucketInitVals(void) {
    // Note: For each initial alpha letter bucket the delta gets reset to ~(first word val - 1)
    currentWord = p_dictIndex->wordVal;
    blobPtr = wordBlob + p_dictIndex->blobOffset;

    // For 3-bit encoding the first nybble in a given bucket
    // is padded to always align in the low nybble of the first byte
    dict_cur_byte = *blobPtr; // ++;
    dict_two_nybbles_queued = true;
}


// Gets an answer word (from the dictionary) by index number in
// the answer list
// Needs to look up words by: Word index in full answer list
void getWord(uint16_t dictWordNum) {

    // Default to failed to find the requested word
    *str_return_buffer = 0;

    // Validate requested word is within range
    if (dictWordNum > (NUM_WORDS - 1))
        return; // failed: str_return_buffer will be NULLED

    dictWordNum++; // make the word number 1 based instead of zero based

    // Use RAM bucket index to fast-forward closer to desired answer word
    p_dictIndex = dictIndexes;

    while (dictWordNum > p_dictIndex->wordCount) {
        dictWordNum -= p_dictIndex->wordCount;
        p_dictIndex++;

        if (p_dictIndex->firstLetter == (DICT_BUCKET_EOF))
            return; // failed: str_return_buffer will be NULLED
    }

    loadDictBucketInitVals();

    for (uint16_t j = dictWordNum; j; j--) {
        updateWord_3bit_varint();
    }
    decodeWord(p_dictIndex->firstLetter, currentWord, str_return_buffer);
}


// Encodes guess word to 5-bits per letter, then seeks through
// dictionary looking for the numeric match
// Returns true if matched or false (0) if no match was found
// Needs to look up words by: Calculated word value
uint8_t filterWord(char* s) {
    // because the input system uses an on-screen keyboard with A-Z only, no need to sanitize
    // for (i=0; i<5; i++)
    //    if (s[i] < 'A' || s[i] > 'Z')
    //        return 0;
    uint32_t userWordVal = 0;

    #ifdef WORD_LETTERS_REVERSED
        int8_t letter;
        // Words encoded with letters in reverse order
        for (letter=3;letter>=0;letter--)
            userWordVal = (userWordVal << 5) | (check_alpha_remap_char(s[letter])-'A');

        uint8_t first_letter = check_alpha_remap_char(s[4])-'A';
    #else
        uint8_t letter;
        // Words encoded with normal letter order
        for (letter=1;letter<5;letter++)
            userWordVal = (userWordVal << 5) | (check_alpha_remap_char(s[letter])-'A');

        uint8_t first_letter = check_alpha_remap_char(s[0])-'A';
    #endif


    // Use RAM bucket index to fast-forward closer to desired answer word
    p_dictIndex = dictIndexes;

    // Scan for first bucket with matching initial letter
    while (first_letter != p_dictIndex->firstLetter) {
        p_dictIndex++;

        if (p_dictIndex->firstLetter == (DICT_BUCKET_EOF))
            return 0;
    }

    // Loop through buckets until either the first letter doesn't match
    // or the user wordVal is less than the bucket start wordVal
    // Note: For each initial alpha bucket the wordVal resets to 0
    while ((first_letter == p_dictIndex->firstLetter) &&
           (userWordVal > p_dictIndex->wordVal)) {

        p_dictIndex++;
    }
    // Now back up one since the above loop is designed to overshoot by +1
    // since it can't check if a wordVal is within a single bucket since
    // buckets don't store a "maxWordValue"
    p_dictIndex--;

    loadDictBucketInitVals();

    // Scan through entire bucket decoding words to see if there is a match
    for (uint16_t j = p_dictIndex->wordCount; j; j--) {
        updateWord_3bit_varint();
        if (currentWord >= userWordVal)
            return currentWord == userWordVal;
    }

    return 0;
}


#endif // _UPDATEWORD_3BIT_VARINT



#ifndef _ASM_GETSPECIALWORD

// C version is about 70-80 bytes larger than ASM version right now,
// speed comparable once bucket size is increased to 50
void getSpecialWord(uint16_t _n, char* buffer) OLDCALL {

    str_return_buffer = buffer;
    static uint16_t w;
    w = 0;
    static uint16_t n;
    n = _n;

    const AnswerBucket_t* bucket = answerBuckets;
    const uint8_t* b = answersBitmap;

    // Use bucket index to fast-forward closer to desired answer word
    while (bucket->numWords <= n) {
        n -= bucket->numWords;
        b += bucket->byteOffsetDelta;
        bucket++;
    }

    while(1) {
        static uint8_t c;
        c = *b++;
        if (c) {
            static uint8_t mask;
            for (mask = 1 ; mask ; mask <<= 1) {
                if (c & mask) {
                    if (n == 0) {
                        // Calculate word number "w" using pointer vs base address
                        w = (uint16_t)(b - answerBitmap) << 3;
                        // Then Subtract out unused bits for this byte (rewind mask 1 step) to get final value
                        mask >>= 1;
                        while (mask <<= 1)
                            w--;
                        getWord(w);
                        return;
                    }
                    n--;
                }
            }
        }
    }
}

#else


// TODO: OLDCALL as precaution against upcoming SDCC calling convention change
void getSpecialWord(uint16_t special_word_num, char* str_buffer) OLDCALL {
    __asm \

    push AF
    push HL
    push BC
    push DE

    // word number (counter):            bc   :n
    // bit index counter & bitmask loop: de   :w
    // answer array index:               hl   :answer
    // packed-bits:                      a    :c

    // Load str_buffer into global
    ldhl  sp, #12
    ld    a, (hl+)
    ld    (#_str_return_buffer), a
    ld    a, (hl)
    ld    (#_str_return_buffer + 1), a

    // Load _n (arg word number) to bc
    ldhl  sp, #10
    ld    a, (hl+)
    ld    c, a
    ld    a, (hl)
    ld    b, a


    // const AnswerBucket_t* bucket = answerBuckets;
    ld    de, #_answerBitmap
    ld    hl, #_answerBuckets
    .preload_loop:

        push  de
        // while (answer_word_num >= bucket->numWords)
        // bc: answer_word_num, hl: answerBuckets->numWords
        // save result in e to avoid re-subtracting if applicable
        ld    a, c
        sub   a, (hl)
        ld    e, a
        ld    a, b
        sbc   a, #0x00
        // If answer_word_num was smaller, exit loop
        jr    c, .preload_loop_done
        // True: (answer_word_num >= bucket->numWords)
        // save result from above: answer_word_num -= bucket->numWords;
        ld    b, a
        ld    c, e  // low byte of result was saved in e
        inc   hl

        // hl: answerBuckets->byteOffsetDelta;
        // b_start += bucket->byteOffsetDelta;
        pop   de
        ld    a, e
        add   a, (hl)
        ld    e, a
        ld    a, d
        adc   a, #0x00
        ld    d, a
        inc   hl

    jr    .preload_loop

    .preload_loop_done:
    // Restore answer array index pointer
    // Was pushed as de
    pop   hl

    // Counter offsets for word number exit test
    inc   b
    inc   c


    // Pre-load loop counter (e gets reloaded from d)
    ld    d, #0x08

    // Load first byte of bit-packed array into a, increment array pointer
    .lookup_loop:

        // c = *b++;
        ld   a, (hl+)

        // Special handling when value is zero
        // Less cycles when not taken
        // if (c == 0) {
        or    a, a
        jr    z, .lookup_loop


        // for (mask = 1 ; mask ; mask <<= 1) {
        // e is loop control
        ld     e, d
        .bitmask_loop:

            // if (c & mask) {
            // Downshift A into carry
            rrca
            // Skip answer word decrement if lowest bit was zero
            jr   nc, .bitmask_loop_check_exit

            // Otherwise decrement word number counter
            // if (n == 0) return
            dec  c
            jr   nz, .bitmask_loop_check_exit

            dec  b
            jr   z, .lookup_done

            .bitmask_loop_check_exit:
            // for (mask = 1 ; mask ; mask <<= 1) {
            dec    e
        jr     nz, .bitmask_loop
        // end _bitmask_loop$:

    jr     .lookup_loop
    // end _lookup_loop$:

    .lookup_done:
    // Calculate bit index counter address from current pointer and base address
    // w = (uint16_t)(b - answerBitmap) << 3;
    ld  a, l
    sub a, #<(_answerBitmap)
    ld  l, a

    ld  a, h
    sbc a, #>(_answerBitmap)
    ld  h, a

    add hl, hl
    add hl, hl
    add hl, hl

    // Loop value left in e holds unused bits from packed byte,
    // subtract out to get final word index and store in de
    ld    a, l
    sub   a, e
    ld    e, a

    ld    a, h
    sbc   a, #0x00
    ld    d, a

    // Load index into bit packed array onto stack
    push    de

    call  _getWord
    // Restore stack after pushing previous arguments
    add    sp, #2

    pop DE
    pop BC
    pop HL
    pop AF
    ret

    __endasm;

}

#endif

#ifdef TEST
main() {
    char w[6] = {0};
    for (int i=0; i<100; i++) {
        getSpecialWord(i, w);
        puts(w);
    }
    printf("%d\n", filterWord("SEREZ"));
    printf("%d\n", filterWord("SERER"));
    printf("%d\n", filterWord("ZYMIC"));
    printf("%d\n", filterWord("BAAED"));
}
#endif
