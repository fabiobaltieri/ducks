#
# Copyright 2019 Fabio Baltieri (fabio.baltieri@gmail.com)
#
# Based on the original ben-wpan code written by:
#   Werner Almesberger, Copyright 2010-2011
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#

NAME = main

MCU = atmega168
F_CPU = 12000000UL

LFUSE = 0xef
HFUSE = 0xdd

CROSS_COMPILE = avr-
CC = $(CROSS_COMPILE)gcc
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE = $(CROSS_COMPILE)size

AVRDUDE = avrdude
AVRDUDEFLAGS = -cusbasp -p$(MCU)
BOOTLOADER = bootloadHID
BOOTLOADER_FLAGS = -r

CFLAGS  = -Iusbdrv -Wall -mmcu=$(MCU) -DF_CPU=$(F_CPU)
ASFLAGS =
LDFLAGS =

OBJS = main.o \
       usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o

# ----- Verbosity control -----------------------------------------------------

CC_normal	:= $(CC)
BUILD_normal	:=
DEPEND_normal	:= $(CPP) $(CFLAGS) -MM -MG

CC_quiet	= @echo "  CC       " $@ && $(CC_normal)
BUILD_quiet	= @echo "  BUILD    " $@ && $(BUILD_normal)
DEPEND_quiet	= @$(DEPEND_normal)

ifeq ($(V),1)
    CC		= $(CC_normal)
    BUILD	= $(BUILD_normal)
    DEPEND	= $(DEPEND_normal)
else
    CC		= $(CC_quiet)
    BUILD	= $(BUILD_quiet)
    DEPEND	= $(DEPEND_quiet)
endif

# ----- Rules -----------------------------------------------------------------

.PHONY:		all clean

all:		$(NAME)_text.bin $(NAME)_text.hex $(NAME)_eeprom.hex $(NAME)_eeprom.bin

$(NAME).elf: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)
	$(SIZE) $@

%_text.bin: %.elf
	$(BUILD) $(OBJCOPY) -j .text -j .data -O binary $< $@

%_text.hex: %.elf
	$(BUILD) $(OBJCOPY) -j .text -j .data -O ihex $< $@

%_eeprom.bin: %.elf
	$(BUILD) $(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O binary $< $@

%_eeprom.hex: %.elf
	$(BUILD) $(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

# ----- Cleanup ---------------------------------------------------------------

clean:
		rm -f $(NAME).bin $(NAME).elf $(NAME).hex
		rm -f $(NAME)_{text,eeprom}.{bin,hex}
		rm -f $(OBJS) $(OBJS:.o=.d)
		rm -f *~

# ----- Dependencies ----------------------------------------------------------

MKDEP =									\
	$(DEPEND) $< |							\
	  sed 								\
	    -e 's|^$(basename $(notdir $<)).o:|$@:|'			\
	    -e '/^\(.*:\)\? */{p;s///;s/ *\\\?$$/ /;s/  */:\n/g;H;}'	\
	    -e '$${g;p;}'						\
	    -e d >$(basename $@).d;					\
	  [ "$${PIPESTATUS[*]}" = "0 0" ] ||				\
	  { rm -f $(basename $@).d; exit 1; }

%.o: %.c
	$(CC) $(CFLAGS) -Os -c $< -o $@
	$(MKDEP)

%.o: %.S
	$(CC) $(CFLAGS) -D__ASSEMBLY__ -Os -c $< -o $@
	$(MKDEP)

-include $(OBJS:.o=.d)

# ----- Programming and device control ----------------------------------------

.PHONY: load fuse boot

load: $(NAME)_text.hex $(NAME)_eeprom.hex
	if ! $(SIZE) -A main.elf | grep -q "eeprom" ; then \
		$(AVRDUDE) $(AVRDUDEFLAGS) -Uflash:w:$(NAME)_text.hex ; \
	else \
		$(AVRDUDE) $(AVRDUDEFLAGS) -Uflash:w:$(NAME)_text.hex -Ueeprom:w:$(NAME)_eeprom.hex ; \
	fi

fuse:
	$(AVRDUDE) $(AVRDUDEFLAGS) -U lfuse:w:$(LFUSE):m -U hfuse:w:$(HFUSE):m

boot: $(NAME)_text.hex
	#../commandline/slider -R || true
	$(HOME)/usbtool -P ducks control out vendor device 0xff 0 0 || true
	sleep 1
	$(BOOTLOADER) $(BOOTLOADER_FLAGS) $(NAME)_text.hex
