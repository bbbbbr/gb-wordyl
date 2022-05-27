import random
import sys

WORD_COUNT = 212

words = []
with open(sys.argv[1], "rb") as f:
    ccedile = u"Ç".encode("utf-8")
    for line in f:
        word = line.strip()
        if len(word.decode("utf-8")) != 5:
            print("Invalid word: {}".format(word))
            exit(1)
        word = word.replace(ccedile, b"\x87")
        words.append(word)

# 1. Generate wordlist.bin

with open("wordlist.bin", "wb") as f:
    for word in words:
        f.write(word + b"\n")

# 2. Generate wordlist.h

sample = sorted(random.sample(words, WORD_COUNT))

with open("wordlist.h", "w") as f:
    f.write("#define WORDS_COUNT {}\n".format(len(sample)))
    f.write("const char *words[WORDS_COUNT] = {\n")

    for word in sample:
        word = word.replace(b"\x87", b"\\x87")
        f.write("  \"{}\",\n".format(word.decode("utf-8")))

    f.write("};\n")
