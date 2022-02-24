
#include <stdint.h>

#ifndef _ENCODED_H
#define _ENCODED_H

extern const uint8_t wordBlob[];

extern const uint8_t answers[];

typedef struct {
  uint16_t wordNumber;
  uint16_t blobOffset;
} LetterBucket_t;

extern const LetterBucket_t buckets[];

typedef struct {
  uint16_t numWords;
  uint16_t byteOffset;
} AnswerBucket_t;

extern const AnswerBucket_t answerBuckets[];


#endif
