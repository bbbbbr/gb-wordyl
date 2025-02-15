# If you move this project you can change the directory
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
ifndef GBDK_HOME
	GBDK_HOME = ~/git/gbdev/gbdk2020/gbdk-2020-git/build/gbdk/
endif
LCC = $(GBDK_HOME)bin/lcc
PNG2ASSET = $(GBDK_HOME)bin/png2asset
GBCOMPRESS = $(GBDK_HOME)bin/gbcompress

VERSION=1.0.6

# Alternate languages can be passed in as follows
# de en es fr it nl pt-br
# make LANG_CODE=<lang code>
#
# Set default language
ifndef LANG_CODE
	LANG_CODE=en
endif

# Cart types can be passed in as follows
# make CART_TYPE=<cart type>
#
# Set default type:
ifndef CART_TYPE
#	CART_TYPE=32k_nosave
	CART_TYPE=mbc5
#	CART_TYPE=31k_1kflash
#	CART_TYPE=md0_sram
endif

CFLAGS += -DLANG_CODE=$(LANG_CODE)
CFLAGS += -DCART_$(CART_TYPE)
# CFLAGS += -Wf--max-allocs-per-node50000
# CFLAGS += -Wf--max-allocs-per-node150000 # diminishing (but present) size returns after this
# CFLAGS += -Wf--max-allocs-per-node500000
# CFLAGS += -Wf--opt-code-size # This doesn't shrink code size much vs -Wf--max-allocs-per-node150000


# Configure platform specific LCC flags here:
LCCFLAGS_gb      = -Wm-yc # Color No MBC  Wl-yt0x1B # Set an MBC for banking (1B-ROM+MBC5+RAM+BATT)
LCCFLAGS_pocket  = -Wm-yc # No MBC -Wl-yt0x1B # Usually the same as required for .gb
LCCFLAGS_duck    = # No MBC -Wl-yt0x1B # Usually the same as required for .gb
LCCFLAGS_gbc     = # No MBC -Wl-yt0x1B -Wm-yc # Same as .gb with: -Wm-yc (gb & gbc) or Wm-yC (gbc exclusive)
LCCFLAGS_sms     =
LCCFLAGS_gg      =


### Handle cart specific flags

ifeq ($(CART_TYPE),mbc5)
	TARGETS=gb pocket
	LCCFLAGS_gb      += -Wl-yt0x1B -Wl-ya1 # Set an MBC for banking (1B-ROM+MBC5+RAM+BATT)
	LCCFLAGS_pocket  += -Wl-yt0x1B -Wl-ya1 # Same as for .gb
endif

# 31K+1k cart loses 1024 bytes at the end for flash storage
ifeq ($(CART_TYPE),31k_1kflash)
	# No reason to build .pocket for the 31K + 1k flash cart
	TARGETS=gb megaduck
	# Add the flash 1K region as an exclusive no-use area for rom usage calcs
	ROMUSAGE_flags = -e:FLASH_SAVE:7C00:400
endif

# Generic 32 Cart with no save support
ifeq ($(CART_TYPE),32k_nosave)
	TARGETS=gb pocket megaduck
endif

# MD0 cart with SRAM (aka, picoDuck flashcart or Duck Laptop with Memory card
ifeq ($(CART_TYPE),md0_sram)
	TARGETS=megaduck
endif


# Targets can be forced with this override, but normally they will be controlled per-cart type above
#
# Set platforms to build here, spaced separated. (These are in the separate Makefile.targets)
# They can also be built/cleaned individually: "make gg" and "make gg-clean"
# Possible are: gb gbc pocket megaduck sms gg
# TARGETS=gb pocket


LCCFLAGS += $(LCCFLAGS_$(EXT)) # This adds the current platform specific LCC Flags

	# Super Game Boy (border) support (only for GB/AP targets, save ROM space otherwise)
ifeq ($(PLAT),gb)
	LCCFLAGS += -Wm-ys
else ifeq ($(PLAT),ap)
	LCCFLAGS += -Wm-ys
endif

# Set ROM name (11 chars max in CGB land)
LCCFLAGS += -Wm-yn"GB-WORDYL"

# No autobanking needed for 32k ROM
# LCCFLAGS += -Wl-j -Wm-yoA -Wm-ya4 -autobank -Wb-ext=.rel -Wb-v # MBC + Autobanking related flags
LCCFLAGS += -debug # Uncomment to enable debug output
# LCCFLAGS += -v     # Uncomment for lcc verbose output


# You can set the name of the ROM file here
PROJECTNAME = gb-wordyl_$(VERSION)_$(CART_TYPE)_$(LANG_CODE)

CFLAGS += -debug
CFLAGS += -Wf-MMD -Wf-Wp-MP
# Add include path for type of flash cart if enabled
CFLAGS += -Wf-I"$(CART_TYPE_DIR)/"
# Add language directory to include path
CFLAGS += -Wf-I"$(LANGDIR)/"


# Code size improvements continue to show up to 250000
# 15000 seems to balance code size improvements vs compile time
# CFLAGS += -Wf--max-allocs-per-node15000

# EXT?=gb # Only sets extension to default (game boy .gb) if not populated
SRCDIR         = src
SFXDIR         = $(SRCDIR)/sfx
LANGDIR        = $(SRCDIR)/lang_$(LANG_CODE)
CART_TYPE_DIR  = $(SRCDIR)/cart_$(CART_TYPE)
SGBDIR         = $(SRCDIR)/sgb
MEGADUCK_LAPTOP = $(SRCDIR)/megaduck_laptop


OBJDIR      = obj/$(EXT)/$(CART_TYPE)_$(LANG_CODE)

RESDIR      = res
BINDIR      = build/$(EXT)/$(CART_TYPE)
MKDIRS      = $(OBJDIR) $(BINDIR) # See bottom of Makefile for directory auto-creation

BINS	      = $(OBJDIR)/$(PROJECTNAME).$(EXT)
CSOURCES      = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c)))
CSOURCES      += $(foreach dir,$(RESDIR),$(notdir $(wildcard $(dir)/*.c)))
CSOURCES      += $(foreach dir,$(SFXDIR),$(notdir $(wildcard $(dir)/*.c)))

# Super Game Boy (border) support only for GB/AP targets, save ROM space otherwise
ifeq ($(PLAT),gb)
	CSOURCES  += $(foreach dir,$(SGBDIR),$(notdir $(wildcard $(dir)/*.c)))
else ifeq ($(PLAT),ap)
	CSOURCES  += $(foreach dir,$(SGBDIR),$(notdir $(wildcard $(dir)/*.c)))
endif

# Extra sources when building MegaDuck laptop
ifeq ($(PLAT),duck)
	CSOURCES  += $(foreach dir,$(MEGADUCK_LAPTOP),$(notdir $(wildcard $(dir)/*.c)))
endif

CSOURCES_LANG = $(foreach dir,$(LANGDIR),$(notdir $(wildcard $(dir)/*.c)))
CSOURCES_CART = $(foreach dir,$(CART_TYPE_DIR),$(notdir $(wildcard $(dir)/*.c)))

ASMSOURCES      = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.s)))
ASMSOURCES_CART = $(foreach dir,$(CART_TYPE_DIR),$(notdir $(wildcard $(dir)/*.s)))

OBJS        = $(CSOURCES:%.c=$(OBJDIR)/%.o)
OBJS        += $(CSOURCES_LANG:%.c=$(OBJDIR)/%.o)
OBJS        += $(CSOURCES_CART:%.c=$(OBJDIR)/%.o)

OBJS        += $(ASMSOURCES:%.s=$(OBJDIR)/%.o)
OBJS        += $(ASMSOURCES_CART:%.s=$(OBJDIR)/%.o)

# Builds all targets sequentially
all: $(TARGETS)

# Dependencies
DEPS = $(OBJS:%.o=%.d)

-include $(DEPS)

# Compile .c files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	$(LCC) $(CFLAGS) -c -o $@ $<

# Compile .c files in "res/" to .o object files
$(OBJDIR)/%.o:	$(RESDIR)/%.c
	$(LCC) $(CFLAGS) -c -o $@ $<

# Compile .c files in "src/sfx/" to .o object files
$(OBJDIR)/%.o:	$(SFXDIR)/%.c
	$(LCC) $(CFLAGS) -c -o $@ $<

# Compile .c files in "src/sgb/" to .o object files
$(OBJDIR)/%.o:	$(SGBDIR)/%.c
	$(LCC) $(CFLAGS) -c -o $@ $<

# Compile .c files in "src/megaduck_laptop/" to .o object files
$(OBJDIR)/%.o:	$(MEGADUCK_LAPTOP)/%.c
	$(LCC) $(CFLAGS) -c -o $@ $<

# Compile .c files in "src/<LANG_CODE>/" to .o object files
$(OBJDIR)/%.o:	$(LANGDIR)/%.c
	$(LCC) $(CFLAGS) -c -o $@ $<

# Compile .c files in "src/<CART_TYPE_DIR>/" to .o object files
$(OBJDIR)/%.o:	$(CART_TYPE_DIR)/%.c
	$(LCC) $(CFLAGS) -c -o $@ $<

# Compile .s assembly files in "src/" to .o object files
$(OBJDIR)/%.o:	$(CART_TYPE_DIR)/%.s
	$(LCC) $(CFLAGS) -c -o $@ $<


# Compile .s assembly files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.s
	$(LCC) $(CFLAGS) -c -o $@ $<


# If needed, compile .c files in "src/" to .s assembly files
# (not required if .c is compiled directly to .o)
$(OBJDIR)/%.s:	$(SRCDIR)/%.c
	$(LCC) $(CFLAGS) -S -o $@ $<PROJECTNAME

# Link the compiled object files into a .gb ROM file
$(BINS):	$(OBJS)
	$(LCC) $(LCCFLAGS) $(CFLAGS) -o $(BINDIR)/$(PROJECTNAME).$(EXT) $(OBJS)


langs:
	${MAKE} LANG_CODE=de    CART_TYPE=$(CART_TYPE)
	${MAKE} LANG_CODE=en    CART_TYPE=$(CART_TYPE)
	${MAKE} LANG_CODE=es    CART_TYPE=$(CART_TYPE)
	${MAKE} LANG_CODE=fr    CART_TYPE=$(CART_TYPE)
	${MAKE} LANG_CODE=it    CART_TYPE=$(CART_TYPE)
	${MAKE} LANG_CODE=nl    CART_TYPE=$(CART_TYPE)
	${MAKE} LANG_CODE=la    CART_TYPE=$(CART_TYPE)
	${MAKE} LANG_CODE=pt-br CART_TYPE=$(CART_TYPE)
	${MAKE} LANG_CODE=kw    CART_TYPE=$(CART_TYPE)
	${MAKE} LANG_CODE=hu    CART_TYPE=$(CART_TYPE)
#   PT full answer file is too big
#	${MAKE} LANG_CODE=pt

langs-clean:
	${MAKE} clean LANG_CODE=de     CART_TYPE=$(CART_TYPE)
	${MAKE} clean LANG_CODE=en     CART_TYPE=$(CART_TYPE)
	${MAKE} clean LANG_CODE=es     CART_TYPE=$(CART_TYPE)
	${MAKE} clean LANG_CODE=fr     CART_TYPE=$(CART_TYPE)
	${MAKE} clean LANG_CODE=it     CART_TYPE=$(CART_TYPE)
	${MAKE} clean LANG_CODE=nl     CART_TYPE=$(CART_TYPE)
	${MAKE} clean LANG_CODE=la     CART_TYPE=$(CART_TYPE)
	${MAKE} clean LANG_CODE=pt-br  CART_TYPE=$(CART_TYPE)
	${MAKE} clean LANG_CODE=kw     CART_TYPE=$(CART_TYPE)
	${MAKE} clean LANG_CODE=hu     CART_TYPE=$(CART_TYPE)
#   PT full answer file is too big
#	${MAKE} LANG_CODE=pt


carts:
	${MAKE} CART_TYPE=31k_1kflash langs
	${MAKE} CART_TYPE=mbc5 langs
	${MAKE} CART_TYPE=32k_nosave langs
	${MAKE} CART_TYPE=md0_sram langs


carts-clean:
	${MAKE} CART_TYPE=31k_1kflash langs-clean
	${MAKE} CART_TYPE=mbc5 langs-clean
	${MAKE} CART_TYPE=32k_nosave langs-clean
	${MAKE} CART_TYPE=md0_sram langs-clean


dictionaries: langs-compress

langs-compress:
	make -C compress compress-all-langs


romusage:
# Ignores failure if romusage not in path
	-romusage build/gb/$(CART_TYPE)/gb-wordyl_$(VERSION)_$(CART_TYPE)_$(LANG_CODE).noi $(ROMUSAGE_flags) -e:STACK:DEFF:100 -e:SHADOW_OAM:C000:A0
	-romusage build/gb/$(CART_TYPE)/gb-wordyl_$(VERSION)_$(CART_TYPE)_$(LANG_CODE).noi $(ROMUSAGE_flags) -e:STACK:DEFF:100 -e:SHADOW_OAM:C000:A0 > romusage.txt

package:
	mkdir -p "../Build Archive/$(VERSION)"
	zip -j -9 "../Build Archive/$(VERSION)/gb-wordyl_$(VERSION)_MegaDuck_32k_nosave.zip"       Manual.md README.md build/duck/32k_nosave/*.duck
	zip -j -9 "../Build Archive/$(VERSION)/gb-wordyl_$(VERSION)_MegaDuck_31k_1kflash.zip"      Manual.md README.md build/duck/31k_1kflash/*.duck
	zip -j -9 "../Build Archive/$(VERSION)/gb-wordyl_$(VERSION)_MegaDuck_md0_sram.zip"         Manual.md README.md build/duck/md0_sram/*.duck
	#
	zip -j -9 "../Build Archive/$(VERSION)/gb-wordyl_$(VERSION)_GameBoy_32k_nosave.zip"        Manual.md README.md build/gb/32k_nosave/*.gb
	zip -j -9 "../Build Archive/$(VERSION)/gb-wordyl_$(VERSION)_GameBoy_31k_1kflash.zip"       Manual.md README.md build/gb/31k_1kflash/*.gb
	zip -j -9 "../Build Archive/$(VERSION)/gb-wordyl_$(VERSION)_GameBoy_mbc5.zip"              Manual.md README.md build/gb/mbc5/*.gb
	#
	zip -j -9 "../Build Archive/$(VERSION)/gb-wordyl_$(VERSION)_AnaloguePocket_32k_nosave.zip" Manual.md README.md build/pocket/32k_nosave/*.pocket
	zip -j -9 "../Build Archive/$(VERSION)/gb-wordyl_$(VERSION)_AnaloguePocket_mbc5.zip"       Manual.md README.md build/pocket/mbc5/*.pocket

clean:
	@echo Cleaning
	@for target in $(TARGETS); do \
		$(MAKE) $$target-clean; \
	done

assets:
# Font tiles	
	$(PNG2ASSET) $(RESDIR)/font.png -no_palettes -keep_duplicate_tiles -tiles_only -map -bpp 1 -o $(RESDIR)/font_tiles_1bpp_precomp.c
	$(GBCOMPRESS) --cin --cout -v --varname=font_tiles $(RESDIR)/font_tiles_1bpp_precomp.c $(RESDIR)/font_tiles_1bpp_gbcompress.c
	rm $(RESDIR)/font_tiles_1bpp_precomp.c $(RESDIR)/font_tiles_1bpp_precomp.h
# TODO:
# cursor_tiles.c
# board_grid_tiles_gbcompress.c
# board_letter_tiles_8x8_1bpp_gbcomp.c
# font_num_tiles_1bpp_gbcompressed.c
# intro_and_dialog_tiles.c
# intro_dialog_map_gbcompress.c
# intro_stars_map_gbcompress.c


# Include available build targets
include Makefile.targets


# create necessary directories after Makefile is parsed but before build
# info prevents the command from being pasted into the makefile
ifneq ($(strip $(EXT)),)           # Only make the directories if EXT has been set by a target
$(info $(shell mkdir -p $(MKDIRS)))
endif
