# Name: Makefile
# Project: bootloadHID
# Author: Christian Starkjohann
# Creation Date: 2007-03-19
# Tabsize: 4
# Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GNU GPL v2 (see License.txt)
# based on: Makefile 281 2007-03-20 13:22:10Z cs
# This Revision: $Id: Makefile 1.3 2008/05/09 07:28:16 Detlef Exp $

###############################################################################
# Configure the following variables according to your AVR. The example below
# is for an ATMega8. Program the device with
#     make fuse    # to set the clock generator, boot section size etc.
#     make flash   # to load the boot loader into flash
#     make lock    # to protect the boot loader from overwriting

DEVICE = atmega8
BOOTLOADER_ADDRESS = 1800
FUSEH = 0xd8
FUSEL = 0x1f

# Fuse high byte:
# 0xd8 = 1 1 0 1   1 0 0 0 <-- BOOTRST (boot reset vector at 0x1800)
#        ^ ^ ^ ^   ^ ^ ^------ BOOTSZ0
#        | | | |   | +-------- BOOTSZ1
#        | | | |   + --------- EESAVE (don't preserve EEPROM over chip erase)
#        | | | +-------------- CKOPT (full output swing)
#        | | +---------------- SPIEN (allow serial programming)
#        | +------------------ WDTON (WDT not always on)
#        +-------------------- RSTDISBL (reset pin is enabled)
# Fuse low byte:
# 0x1f = 0 0 0 1   1 1 1 1
#        ^ ^ \ /   \--+--/
#        | |  |       +------- CKSEL 3..0 (external >8M crystal)
#        | |  +--------------- SUT 1..0 (crystal osc, BOD enabled)
#        | +------------------ BODEN (BrownOut Detector enabled)
#        +-------------------- BODLEVEL (4.0V)

###############################################################################

AVRDUDE = avrdude -c stk500v2 -P avrdoper -p $(DEVICE)

LDFLAGS += -Wl,--section-start=.text=$(BOOTLOADER_ADDRESS)

COMPILE = avr-gcc -Wall -Os -Iusbdrv -I. -mmcu=$(DEVICE) #-DTEST_MODE -DDEBUG_LEVEL=2
# NEVER compile the final product with debugging! Any debug output will
# distort timing so that the specs can't be met.

OBJECTS = usbdrv/usbdrvasm.o usbdrv/oddebug.o main.o
# usbdrv/usbdrv.o usbdrv/usbdrvasm.o usbdrv/oddebug.o main.o

# symbolic targets:
all:	main.hex

.c.o:
	$(COMPILE) -Wa,-adhlns=$(<:.c=.lst) -c $< -o $@
	# $(COMPILE) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -S $< -o $@

flash:	all
	$(AVRDUDE) -U flash:w:main.hex:i

readflash:
	$(AVRDUDE) -U flash:r:read.hex:i

fuse:
	$(AVRDUDE) -U hfuse:w:$(FUSEH):m -U lfuse:w:$(FUSEL):m

lock:
	$(AVRDUDE) -U lock:w:0xef:m

read_fuses:
	$(UISP) --rd_fuses

clean:
	rm -f main.hex main.bin *.o *.lst usbdrv/*.o usbdrv/*.lst usbdrv/oddebug.s usbdrv/usbdrv.s

# file targets:
main.bin:	$(OBJECTS)
	$(COMPILE) -o main.bin $(OBJECTS) $(LDFLAGS)

main.hex:	main.bin
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
	avr-size main.bin

disasm:	main.bin
	avr-objdump -d main.bin

cpp:
	$(COMPILE) -E main.c
