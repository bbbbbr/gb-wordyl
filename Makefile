#
# Simple Makefile that compiles all .c and .s files in the same folder
#

# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
GBDK_HOME = gbdk/

LCC = $(GBDK_HOME)bin/lcc
CC = gcc

# You can uncomment the line below to turn on debug output
# LCC = $(LCC) -debug

# You can set the name of the .gb ROM file here
PROJECTNAME    = WORDLE

BINS	    = $(PROJECTNAME).gb
CSOURCES   := main.c bloom.c
HSOURCES   := bloom.h

all:	$(BINS)

compile.bat: Makefile
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> compile.bat

tools/hash_generator: tools/hash_generator.c bloom.c bloom.h
	$(CC) -O2 -o tools/hash_generator tools/hash_generator.c bloom.c

wordlist.bin: words.txt tools/gen-wordlist.py
	@# This generates wordlist.bin and filter.h
	python3 tools/gen-wordlist.py words.txt

wordlist.h: wordlist.bin

filter.h: wordlist.bin tools/hash_generator
	./tools/hash_generator wordlist.bin > filter.h

# Compile and link all source files in a single call to LCC
$(BINS):	$(CSOURCES) $(ASMSOURCES) $(HSOURCES) wordlist.h filter.h
	$(LCC) -v -o $@ $(CSOURCES) $(ASMSOURCES)

clean:
	@rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm wordlist.bin wordlist.h filter.h tools/hash_generator
	@rm -rf *.dSYM tools/*.dSYM

