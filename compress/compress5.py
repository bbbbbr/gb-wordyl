#!/usr/bin/env python3

import sys

# Default language, override with first console arg
lang = "ca" # Catalan
CEDILLA_CHR_NUM = 26 # a = 0, z = 25 (Note: in decode side all caps are used, so it becomes char after 'Z' instead)
ALPHA_LETTER_COUNT = 27


### ENCODING SETTINGS ###

# --- DEFAULTS ---
NUM_ANSWER_BUCKETS = 14
DICT_INDEX_CHUNK_SIZE = 500
DICT_BUCKETS_EOF = 0xFF

PER_LETTER_ENCODING = "5-bit"
WORD_NUMERIC_ENCODING = "7-bit-variable"
WORD_LETTER_ORDER = "normal"
ALPHABET_REMAP = "normal"
ZERO_DELTA = "yes-always-subtract-one"
ENCODING_ORDER_3BIT_LSBITS = "high-bits-first"

# --- OVERRIDES ---

# Base26:
# - Compression with max options is ~700 bytes smaller with Base 26
# - But decompression support code is about 900 bytes larger
#   due to 32 bit multiplication and division to extract the numbers
#   (even while avoiding 32 bit modulo import)
# See: 44f42f7e2e2b2817c817b726d3b0b91c8baef46e
#
# PER_LETTER_ENCODING = "base-26"

WORD_NUMERIC_ENCODING = "3-bit-variable"
# WORD_NUMERIC_ENCODING = "4-bit-variable"

#  These can both be turned on for Catalan
#   * But only ALPHABET_REMAP makes the compressed size smaller
#   * WORD_LETTER_ORDER actually makes the compressed output larger
# ALPHABET_REMAP = "freq_of_use"
# WORD_LETTER_ORDER = "reverse"

# ZERO_DELTA = "never-substract-by-one"

# ENCODING_ORDER_3BIT_LSBITS = "low-bits-first"



# TODO: would be nice if these options dumped control #define flags into the rendered header file




# Optional arg 1 is language type (defaults to "en" otherwise)
# which sets input source text files and output folders under ../src/
if (len(sys.argv)) > 1:
    lang = sys.argv[1]

# output_path = "../src/"
output_path = "../src/lang_" + lang + "/"

outfile = open(output_path + "encoded.c", 'w')
outfile.write("#include \"../encoded.h\"\n\n");


def preprocessWord(w):
    # Alphabet frequency remapping
    if (ALPHABET_REMAP == "freq_of_use"):
       w = remapAlpha(w)

    # Reverse order of letters ("tiles" -> "selit")
    if (WORD_LETTER_ORDER == "reverse"):
        w = w[::-1]

    # Remap any special chars
    w = remapSpecialChars(w)

    return w


def mask(n):
    # next power of 2 starting with n-i
    m = 1
    while m <= n:
        m *= 2
    return m

def toBitmap(bits):
    def encodeByte(offset):
        return sum( 1 << i for i in range(8) if offset+i < len(bits) and bits[offset+i] )

    return bytes(encodeByte(x) for x in range(0,len(bits),8))


# Compact nybbles into bytes, padding up to the nearest byte
def compactNybblestoBytes(nybbleBytes):

    # packedBytes = b''
    packedBytes = []
    n = len(nybbleBytes)
    i = 0
    cur_byte = 0
    needsFlush = False
    while i < n:
        # Accumulate two nybbles into each byte, then write it out
        if (i & 0x01):
            # Merge in high nybble and write out byte
            cur_byte = cur_byte | (nybbleBytes[i] << 4 & 0xF0)
            packedBytes.append(cur_byte)
            needsFlush = False
        else:
            cur_byte = nybbleBytes[i] & 0x0F
            needsFlush = True
        i+=1

    # Flush trailing (low) nybble/byte if needed
    if (needsFlush == True):
        packedBytes.append(cur_byte)

    return bytes(packedBytes)


def dumpBlobBytes(outputfile, name, blob):
    counter_var = 0
    n = len(blob)
    outputfile.write("const uint8_t %s[%u] = {\n" % (name, n))
    i = 0
    while i < n:
        outputfile.write("  ")
        for j in range(min(n-i,16)):
            counter_var += 1
            outputfile.write("0x%02x," % blob[i])
            i+=1
        outputfile.write("\n")
    outputfile.write("};\n\n")
    return counter_var


def remapAlpha(source_word):
    # Reorganize letters to assign lowest values to highest frequency letters
    # remaped_alpha = "esiarnmtodlcugpkhbyfvwjzxq" # english letter frequency ranked
    # remaped_alpha = "aeorilstuncpmdbghkfwyvzjxq" # word reversed, frequency count of last 4 letters
    # remaped_alpha = "eairnoltusmdcgpkbfwvhyzxjq" # frequency count of letters 3-4 (1 based index)
    remaped_alpha = "aeioustryhkbcdfgjlmnpqvwxz" # (hackernews suggestion)
       # inverse of the above map for reverse-mapping: "almnbopjcqkrstduvhfgewxyiz"

    remapped_word = ""

    for letter in range(len(source_word)):
        # Cedilla does not get remapped, it is always 27th letter
        if (source_word[letter] == 'ç'):
            remapped_word += source_word[letter]
        else:
            for c in range(len(remaped_alpha)):
                if (source_word[letter] == remaped_alpha[c]):
                    # print(source_word[letter] + " -> " + remaped_alpha[c] + "(" + str(c) + ") == " + chr(ord('a') + c))
                    remapped_word += chr(ord('a') + c)
                    break

    return remapped_word


def remapSpecialChars(source_word):

    remapped_word = ""

    for letter in range(len(source_word)):
        # Cedilla gets converted to '{', the char right after lowercase 'z'
        if (source_word[letter] == 'ç'):
             remapped_word += chr(ord('a') + CEDILLA_CHR_NUM)
        else:
            remapped_word += source_word[letter]

    return remapped_word


# Encode each letter with 5 bits
# (4 letters x 5 bits = bitpacked into 20 bytes, since first letter is dropped)
def tobinary_5bit(w):
    s = 0
    for i in range(len(w)):
        s = (s << 5) + (ord(w[i])-ord('a'))
    # print ("%s -> %x" % (w, s))
    return s


# Encode each letter in Base 26, summed into a single value (back to front)
def tobinary_base26(w):
    s = 0
    for i, c in enumerate(w.encode('ascii')[::-1]):
        s += (c - ord('a'))*26**i
    return s


# For each word encode the numeric delta from the previous with 3 bit variable length packing
# requires later packing of nybbles into bytes
def encodeDelta_3Bit(num):
    assert num > 0
    # The delta should never be zero, so 1 bit can be saved by always subtracting 1
    if (ZERO_DELTA == "yes-always-subtract-one"):
        num -= 1

    if num == 0:
        return bytes([0])
    res = []

    if (ENCODING_ORDER_3BIT_LSBITS == "high-bits-first"):
        # This option encodes the HIGHEST bits first
        #
        # First count number of required varints to pack the numeric value
        chunks_to_encode = 0
        temp_num = num
        while temp_num > 0:
            temp_num = temp_num >> 3
            chunks_to_encode += 1

        # Pack high bits first for easier decoding
        while chunks_to_encode > 0:
            part = (num >> (3 * (chunks_to_encode - 1))) & 0x7
            # If more bytes will follow, set bit 4
            if chunks_to_encode > 1:
                part |= 0x8
            res.append(part)
            chunks_to_encode -= 1

    else: # ENCODING_ORDER_3BIT_LSBITS = "low-bits-first"

        # This option encodes the LOWEST bits first,
        while num > 0:
            # 3 bit + 1 control bit encoding
            part = num & 0x7
            num = num >> 3
            # If more bytes will follow, set bit 4
            if num > 0:
                part |= 0x8
            res.append(part)

    return bytes(res)


# For each word encode the numeric delta from the previous with 7 bit variable length packing
def encodeDelta_7Bit(d):
    # The delta should never be zero, so 1 bit can be saved by always subtracting 1
    if (ZERO_DELTA == "yes-always-subtract-one"):
        d-=1

    assert d<0x80*0x80*0x80
    if d < 0x80:
        return bytes((0x80|d,))
    elif d < 0x80*0x80:
        return bytes((d & 0x7F, 0x80|(d>>7)))
    else:
        return bytes((d & 0x7F, (d>>7) & 0x7F, (d>>14)))



# typedef struct {
#   uint16_t   wordCount;   // * [Relative] Number of words in current bucket (delta)
#   uint16_t   blobOffset;  // * [Absolute] Byte Index of bucket start into blob array
#   uint8_t    firstLetter; // * [Absolute] Alphabet index of first letter (A = 0, B = 1, etc)
#   uint32_t   wordVal;     // * [Absolute] Numeric word value (last 4 letters, 24 bits max)
#                           //   of *preceding* word (last word for preceding bucket).
#                           //   Resets to 0 when bucket first letter changes, so the first encoded word
#                           //   after a first letter change will have it's full value as it's delta value
# } dictIndexBucket_t;


blobPtr = 0
blobIndexes = []

def addBlobIndex(bucketWordCount, bucketSizeBytes, alphaIdx, bucketStartWordVal):
    global blobPtr
    global blobIndexes
    blobIndexes.append([bucketWordCount, blobPtr, alphaIdx,  bucketStartWordVal])

    # Increment blobPtr after adding ssince it needs to point to the START of the current bucket index
    blobPtr = blobPtr + bucketSizeBytes


# Encode a list of words and output a sequence of bytes or nybbles (depending on encoding)
# This gets called for each a-z bucket, after which they get merged together
def encodeList3bitIndexed(bucketWordlist, alphaIdx):

    if (WORD_NUMERIC_ENCODING != "3-bit-variable"):
        print("Incompatible encoding type")
        sys.exit()

    if (PER_LETTER_ENCODING == "base-26"):
        bin = tuple( map(tobinary_base26, bucketWordlist) )
    else:
        bin = tuple( map(tobinary_5bit, bucketWordlist) )

    # Re-sorting is needed here in case the encoding changes the word numerical value sort order
    bin = sorted(bin)

    # Return zero bytes if no words are present
    if (len(bin) == 0):
        return (b'')

    bucketWordCount = 0
    prevByteSize = 0
    bucketStartWordVal = 0

    # Start each initial letter bucket with the "previous word value" set to the starting
    # word value. That value gets stored in the dictionary index, so the first word's
    # calculated delta can be zero, which saves a little space.
    prevWordVal = bin[0]
    # If needed, subtract by one to avoid a delta of zero when using that optimization
    if (ZERO_DELTA == "yes-always-subtract-one"):
        prevWordVal -= 1
    bucketStartWordVal = prevWordVal

    out = b''
    for i in range(len(bin)):
        curWordVal = bin[i]
        out += encodeDelta_3Bit(curWordVal - prevWordVal)
        bucketWordCount = bucketWordCount + 1

        # If needed, create a bucket (at saved START position, of length AFTER word that was just encoded)
        # Greater than here allows it to keep trying until an even byte encoding occurs
        if (bucketWordCount >= DICT_INDEX_CHUNK_SIZE):
            # Ensure start of next bucket would be byte aligned (not on a half-nybble)
            if ((len(out) & 0x01) == 0):
                bucketByteSize = int(len(out) / 2) - prevByteSize
                # print("len:%u, bSize:%u, bWCount:%u, Let:%u, bwStartVal:%u" % (len(out), bucketByteSize, bucketWordCount, alphaIdx, bucketStartWordVal))

                addBlobIndex(bucketWordCount, bucketByteSize, alphaIdx, bucketStartWordVal)
                # Advance pointer to start of a new bucket and reset word count
                bucketStartWordVal = curWordVal
                prevByteSize = int(len(out) / 2)
                bucketWordCount = 0

        # Save word value to calcualte delta on next pass
        prevWordVal = curWordVal

    # For 3-bit word encoding:
    # Squish the nybbles into bytes and round up to nearest byte at end
    # to ensure byte alignment for start of each letter bucket
    if (WORD_NUMERIC_ENCODING == "3-bit-variable"):
        out = compactNybblestoBytes(out)

    # Save any trailing words if needed
    # Note that "out" is now byte packed (1/2 prev size) and guaranteed to be byte padded/aligned at the end
    if (bucketWordCount > 0):
        bucketByteSize = len(out) - prevByteSize
        # print("len:%u, bSize:%u, bWCount:%u, Let:%u, bwStartVal:%u" % (len(out), bucketByteSize, bucketWordCount, alphaIdx, bucketStartWordVal))
        addBlobIndex(bucketWordCount, bucketByteSize, alphaIdx, bucketStartWordVal)

    return out



input_byte_length = 0
dict_byte_size = 0
answer_bitmap_size = 0

allWords = set()
answerWords = set()

print("full_" + lang + ".txt")
print("answers_" + lang + ".txt")

with open("full_" + lang + ".txt") as f:
    for w in f:
        w = w.strip()
        if len(w) == 5:
            w = preprocessWord(w)
            allWords.add(w)

with open("answers_" + lang + ".txt") as f:
    for w in f:
        w = w.strip()
        if len(w) == 5:
            w = preprocessWord(w)
            allWords.add(w)
            answerWords.add(w)

# Sort Dictionary Word List and split it into first-letter buckets
allWords = tuple(sorted(allWords))
buckets = [[] for i in range(ALPHA_LETTER_COUNT)]
for w in allWords:
    # print("allwords: %u: %s" % (input_byte_length / 5, w))
    input_byte_length += 5 # 5 letters per word
    buckets[ord(w[0])-ord('a')].append(w[1:])


encoded = tuple(map(encodeList3bitIndexed, buckets, range(len(buckets))))
# Add final terminating index entry
addBlobIndex(0,0,DICT_BUCKETS_EOF,0)


# Join the separate first-letter split encoded blobs
wordBlob = b''.join(encoded)

# == Create a bitmap for looking up Answer Words in the Dictionary ==
# This still works after all the pre-processing and sorting since it applies to both
answerBits = tuple(1 if w in answerWords else 0 for w in allWords)
answerBlob = toBitmap(answerBits)

# == Write out packed Dictionary  ==

dict_byte_size = dumpBlobBytes(outfile, "wordBlob", wordBlob)


# == Write out Dictionary Index Buckets ==

outfile.write("// Lookup Table to fast-forward through Dictionary blob\n")
outfile.write("// Adds a couple hundred extra bytes, but makes 3-bit decoding fast enough\n")
outfile.write("// and gives a net gain of several thousand bytes.\n")
outfile.write("//\n")
outfile.write("// The wasted top u32 byte was difficult to efficiently discard with C u32 handling\n")
outfile.write("// (net savings were about 40 bytes) and would need an asm version instead.\n")
outfile.write("// See: 8438d820e1af9cb9061b4e669ceee7c77fe96dd3\n")
outfile.write("//\n")
outfile.write("// {wordCount:u16, blobOffset:u16, firstLetter:u8, wordVal:u32}\n");
outfile.write("const dictIndexBucket_t dictIndexes[%u] = {\n" % (len(blobIndexes)))

for i in range(len(blobIndexes)):
    outfile.write("  { 0x%04X, 0x%04X, 0x%02X, 0x%06X }, // %s: %4u words\n" % (blobIndexes[i][0], blobIndexes[i][1], blobIndexes[i][2], blobIndexes[i][3],
                                                                      str(chr(ord('a')+ blobIndexes[i][2])) if blobIndexes[i][2] < ALPHA_LETTER_COUNT else "end",
                                                                      blobIndexes[i][0]))
    # print("  /* %s */ { 0x%04X, 0x%04X, 0x%02X, 0x%06X }," % (str(chr(ord('a')+ blobIndexes[i][2])) if blobIndexes[i][2] < 26 else "end",
    #                    blobIndexes[i][0], blobIndexes[i][1], blobIndexes[i][2], blobIndexes[i][3]))
outfile.write("};\n\n")


# == Write out Answer Word List Lookup Bitmap Bitmap ==

outfile.write("// Lookup Table to fast-forward through Answers->in->Dictionary bitmap\n")
outfile.write("// {uint8_t num Answer Words, uint8_t byte offset delta in Dictionary Bitmap} [\n")
outfile.write("const AnswerBucket_t answerBuckets[] = {\n""")

bucketAnswerSize = len(answerWords) // NUM_ANSWER_BUCKETS
if (bucketAnswerSize > 254):
    print("Error: answer bitmap bucket size exceeds max size. Use more buckets: words(%u) / buckets(%u) = bucketsize(%u)" % (len(answerWords), NUM_ANSWER_BUCKETS, bucketAnswerSize));
    sys.exit()

dictPosPreOverflow = 255 - 8

dictPos = 0
answerCount = 0
prevAnswerCount = 0
dictByteOffsetDelta = 0
dictPosLastBucketEnd = 0
answerBucketCount = 0


while (answerCount < len(answerWords)):
    bucketAnswerThreshold = answerCount + bucketAnswerSize

    # Break out and write a new bucket:
    #  * If there are enough answers in the bucket
    #    * Or there are close to overflowing the max (byte) bitmap offset size per bucket
    #  * But only on even answer bitmap byte boundaries
    #  * And as long as the dict count is within the bitmap size
    while (((answerCount < bucketAnswerThreshold) and ((dictByteOffsetDelta//8) < dictPosPreOverflow))
            or dictPos%8 != 0) and dictPos < len(answerBits):
        if answerBits[dictPos]:
            answerCount += 1
        dictPos += 1
        dictByteOffsetDelta = dictPos - dictPosLastBucketEnd

    if ((dictByteOffsetDelta//8) >= dictPosPreOverflow):
        print("Note: Answer bitmap bucket close to overflow, adding another. Bucket: %u { numAnswers: %u, dictByteOffset: %u},\n" % (i, answerCount-prevAnswerCount, dictByteOffsetDelta//8))
    dictPosLastBucketEnd = dictPos
    outfile.write("  { %u, %u},\n" % (answerCount-prevAnswerCount, dictByteOffsetDelta//8))
    answerBucketCount = answerBucketCount + 1
    dictByteOffsetDelta = 0
    prevAnswerCount = answerCount

outfile.write("};\n")

outfile.close()


# == Write out Bitmap of Answers in Dictionary  ==

# Note: the Answer Bitmap Index gets much worse compression when
#       WORD_LETTERS_REVERSED is not enabled. When word letters
#       are not reversed the distribution of the answers is much
#       more even and so harder to get good RLE results
#
# Write to a different c source file so it can get gbcompressed
with open(output_path + "answerbitmap.c.nocomp", "w") as answerfile:
    answerfile.write("// Bitmap of answers within dictionary (non-compressed)\n")
    answer_bitmap_size = dumpBlobBytes(answerfile, "answerBitmapIndex", answerBlob)
answerfile.close()

# == Write out data sizes and ranges ==

with open(output_path + "sizes.h", "w") as sizes:
    sizes.write("#define NUM_WORDS %u\n" % len(allWords))
    sizes.write("#define NUM_ANSWERS %u\n" % len(answerWords))
    sizes.write("#define NUM_ANSWER_BUCKETS %u\n" % answerBucketCount)
    sizes.write("#define NUM_ANSWERS_ROUNDED_UP_POW2 %u" % mask(len(answerWords)))

print ("Input size: " + str(input_byte_length) + ", Dict out size: " + str(dict_byte_size) + ", Answer Bitmap size: " + str(answer_bitmap_size));

# Get un-map version of remap alphabet string
# str_remap = "abcdefghijklmnopqrstuvwxyz"
# str_remap = remapAlpha(str_remap)
# print (str_remap)


