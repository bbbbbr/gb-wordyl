
#include <stdint.h>

#ifndef _ENCODED_H
#define _ENCODED_H

// Compressed dictionary
extern const uint8_t wordBlob[];

// Bitmap storing answer word locations in dictionary
extern const uint8_t answers[];

// TODO: delete me
typedef struct {
  uint16_t wordNumber;
  uint16_t blobOffset;
} LetterBucket_t;

typedef struct {
  uint16_t   wordCount;   // * [Relative] Number of words in current bucket (delta)
  uint16_t   blobOffset;  // * [Absolute] Byte Index of bucket start into blob array
  uint8_t    firstLetter; // * [Absolute] Alphabet index of first letter (A = 0, B = 1, etc)
  uint32_t   wordVal;     // * [Absolute] Numeric word value (last 4 letters, 24 bits max)
                          //   of *preceding* word (last word for preceding bucket).
                          //   Resets to (current wordval -1) when bucket first letter changes, so
                          //   first encoded word after a first letter change will have a delta of "1"
} dictIndexBucket_t;


extern const dictIndexBucket_t dictIndexes[];

// TODO: delete me
extern const LetterBucket_t buckets[];

typedef struct {
  uint8_t numWords;
  uint8_t byteOffsetDelta;
} AnswerBucket_t;

extern const AnswerBucket_t answerBuckets[];


#endif
