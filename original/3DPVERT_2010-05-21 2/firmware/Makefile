#*******************************************************************************
#* File Name	: Makefille
#* Title	: Microsoft Sidewinder 3D Pro to USB converter
#* Date		: 2006/12/14
#* Version	: 2.0
#* Target MCU	: ATmega8
#* Tool Chain	: Atmel AVR Studio 4.11 SP4 / WinAVR 20060421
#* Author	: Detlef "Grendel" Mueller
#*		  detlef@gmail.com
#* References	: HIDKeys by Christian Starkjohann, ODS
#* Description	: USB to 3D Pro converter makefile for WinAVR
#* Device	: Microsoft Sidewinder 3D Pro
#*
#* Release Notes:
#*
#* $Id: Makefile 1.7 2008/05/09 07:28:39 Detlef Exp $
#*
#* $Log: Makefile $
#* Revision 1.7  2008/05/09 07:28:39  Detlef
#* Minor cleanup
#*
#* Revision 1.6  2008/04/28 09:09:38  Detlef
#* Added MAXCODE/DATA for checksize script
#*
#* Revision 1.5  2008/04/28 08:19:20  Detlef
#* Added tiny461, corrected fuses
#*
#* Revision 1.4  2007/08/01 08:42:05  Detlef
#* New WinAVR didn't like capital S in .S=.lst macro.
#*
#* Revision 1.3  2007/06/12 03:26:35  Detlef
#* Added usbdescr.o
#*
#* Revision 1.2  2007/01/03 08:59:22  Detlef
#* Corrected fuse bits
#*
#* Revision 1.1  2006/12/16 11:57:02  Detlef
#* Initial revision
#*
#*******************************************************************************

# MCU can be atmega8 or attiny461

MCU = attiny461

UISP = uisp -dprog=stk500 -dpart=$(MCU) -dserial=/dev/ttyS1
COMPILE = avr-gcc -Wall -Os -Iusbdrv -I. -mmcu=$(MCU) #-DDEBUG_LEVEL=1
COMMON_OBJS = usbdrv/usbdrv.o 3DPro.o usbdrv/usbdrvasm.o 3DProasm.o usbdescr.o usbdrv/oddebug.o main.o

OBJECTS = usbdrv/usbdrv.o 3DPro.o usbdrv/usbdrvasm.o 3DProasm.o usbdescr.o usbdrv/oddebug.o main.o

ifeq ($(MCU),attiny461)
MAXCODE = 4096
MAXDATA = 160
FUSES = --wr_fuse_e=0xff --wr_fuse_h=0xdd --wr_fuse_l=0xdf
endif

ifeq ($(MCU),atmega8)
MAXCODE = 6144
MAXDATA = 768

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

FUSES = --wr_fuse_h=0xd8 --wr_fuse_l=0x1f

endif

# symbolic targets:
all:	main.hex

.c.o:
	$(COMPILE) -Wa,-adhlns=$(<:.c=.lst) -c $< -o $@

.S.o:
	$(COMPILE) -x assembler-with-cpp -Wa,-adhlns=$(<:.s=.lst),-gstabs -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

.c.s:
	$(COMPILE) -Wa,-adhlns=$(<:.c=.lst) -S $< -o $@


clean:
	rm -f main.hex *.lst *.o main.map main.bin *.o usbdrv/*.o usbdrv/*.lst

# file targets:
main.bin:	$(COMMON_OBJS) 
	$(COMPILE) -o main.bin $(OBJECTS) -Wl,-Map=main.map

main.hex:	main.bin
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.bin main.hex
	./checksize main.bin $(MAXCODE) $(MAXDATA)

flash:	all
	$(UISP) --erase --upload --verify if=main.hex

fuse:
	$(UISP) $(FUSES)
