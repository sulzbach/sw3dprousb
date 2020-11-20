This is the README file for the 3DP-Vert Rev2
=============================================

3DP-Vert is a device to connect a Microsoft SideWinder 3D Pro, Precision
Pro, or Force Feedback Pro joystick as a generic USB HID Joystick to a
computer while retaining full functionallity (8-way hat, slider, and all
buttons) at a high readout rate (100Hz) with low latency (< 4ms).


LICENSE
-------

This project is built with the V-USB driver from Objective Development,
which is published under the terms and conditions of the GNU GPL version
2 license. 3DP-Vert is distributed under the same license conditions. See
the file "firmware\usbdrv\License.txt" for details.


TOOLCHAIN
---------

The converter is realized using an Atmel ATTiny461 AVR micro controller
and Objective Development's firmware-only USB driver V-USB for AVRs.

http://atmel.com/dyn/products/product_card.asp?part_id=2004
http://www.obdev.at/products/vusb/index.html

The 3DP-Vert firmware was developt under Windows XP Pro using WinAVR
20100110, Atmel AVR Studio 4.18 b700, and an Atmel AVRISP MkII In-
System-Programmer (ISP).

http://sourceforge.net/projects/winavr/
http://atmel.com/dyn/products/tools_card.asp?tool_id=2725
http://atmel.com/dyn/products/tools_card.asp?tool_id=3808


BUILDING 3DP-VERT FROM SCRATCH
------------------------------

Build the hardware according to ""Rev2\schematic\3DP-Vert2.pdf", see also
"Rev2\schematic\BoardXX.jpg". Install AVR Studio 4. Connect the ISP to
the board and connect the board to a USB port. Use AVR Studio to load the
firmware image "Rev2\3dpv12t4.hex" into the ATtiny461. Set the ATtiny461
fuse bytes according to "Rev2\schematic\Fuses461.gif". Disconnect the ISP.

Refer to "Doc\Manualv3.pdf" for operating instructions.

Notes:

 - the HEX image provided is compiled for a 12MHz crystal. You could use
   a 16 or 15MHz crystal but need to recompile the software to match the
   frequency (change "firmware\f_cpu.h" to match the crystal). I would
   suggest to stick w/ 12MHz tho, 3DP-Vert uses less power at this
   frequency. Also, the 10MHz version of the ATTiny461 can be run at
   12MHz w/o problems (I can't guarantee that tho, but I know of at least
   one user who successfully did this.)

 - proto 2 is built on the board that comes w/ RadioShacks "Project Box
   and PC Board Combo", part no. 270-283. The proto also doesn't include
   the ISP header.

 - the blue lines in the schematic pictures represent wires run on TOP
   of the boards. The turquoise lines in the schematic of proto 1 are
   only necessary if you include the ISP header.

 - depending on the LED you want to use you will have to calculate the
   value for R1. Try to minimize the power run through it to keep overall
   power consumption as low as possible. Eg. if your LED is spec'ed at
   1.8v/20mA, run it at 10mA (or even less, 5mA would still work):

   (5V - Vled) / Iled = R1

   (5 - 1.8) / 0.010 = 320 --> Round up to the closest SI R value: 330ohm

 - if you can't get the exact Z-diodes listed in the BOM make sure the
   ones you use are rated <= 500mW (most glas body diodes are <= 500mW).
   Bigger diodes have too much capacity and need too much current to
   work right (pulling the voltage too low.)

 - the gameport connector is implemented as a header on the protos. It
   maps 1:1 to a DB15F connector -- either replace the header w/ one or
   connect a game port bracket to the header:

   http://www.pugetsystems.com/store/item.php?cat=Accessories&id=3687&com=d41d8cd9

 - the shield of the USB connector (X1) needs to be connected to ground
   (as seen w/ the protos.) to prevent a ground loop the shield of the
   DB15F connector should not be connected to ground.

 - the source code contains some experimental code for rev 1 boards that
   is activated via the SIXAXIS and COMPDEV defines in 3DPro.h . COMPDEV
   adds a keyboard device that mirrors the POV positions to hard coded
   keys. SIXAXES needs some non-trivial hardware modifications on the
   board and a 3D Pro (replaces the POV with a mini-joystick.) Do not
   activate these code blocks unless you know exactly what is going on.
   You have been warned.. ;)

 - the very first time a joystick is detected, the serial no. will be
   written into the EEPROM and be read from there in all subsequent
   accesses.

BUILDING AND INSTALLING THE FIRMWARE
------------------------------------

To compile the firmware, install the GNU toolchain for your operating
system. Change to the firmware directory and run "make all", or:

If you use Windows, install AVR Studio 4 after installing the GNU
Toolchain. Start AVRS and load "firmware\3DPro.aps". From the menue
select "Build", "Rebuild All". The new hex image will be created as
"firmware\bin\3DPro.hex".

For questions please check the thread over at the Descent BB at
http://descentbb.net/viewtopic.php?t=15526


FILES IN THIS DISTRIBUTION
--------------------------

Readme.txt ..................... This file
Changelog.txt .................. Release changes

Rev1\* ......................... Files specific to the Rev1 release (deprecated)

Rev2\3dpv12t4.hex .............. 3DP-Vert firmware image, serial #1000, 12MHz

Rev2\schematic\3DP-Vert2.pdf ... 3DP-Vert rev2 schematic
Rev2\schematic\Board1B.jpg ..... Proto 1, bottom view
Rev2\schematic\Board1S.jpg ..... Proto 1, schematic
Rev2\schematic\Board1T.jpg ..... Proto 1, top view
Rev2\schematic\Board2B.jpg ..... Proto 2, bottom view
Rev2\schematic\Board2S.jpg ..... Proto 2, schematic
Rev2\schematic\Board2T.jpg ..... Proto 2, top view
Rev2\schematic\bom.txt ......... Bill Of Materials with digikey.com order numbers
Rev2\schematic\Fuses461.gif .... ATtiny461 fuse settings, AVR Studio w/ AVRISP MkII
Rev2\schematic\Fuses461PP.gif .. ATtiny461 fuse settings, PonyProg

doc\Manualv3.pdf ............... 3DP-Vert Users Manual

firmware\3DPro.aps ............. AVR Studio project file for 3DP-Vert
firmware\3DPro.c ............... SideWinder driver and hardware interface
firmware\3DPro.h ............... SideWinder driver header file
firmware\3DProasm.S ............ SideWinder driver
firmware\f_cpu.h ............... Crystal speed definition
firmware\includes.h ............ System incudes and global macros
firmware\main.c ................ USB to SideWinder converter main module
firmware\Makefile .............. Makefile, tested w/ WinAVR 20070525 & 20071221
firmware\checksize ............. Script used by Makefile
firmware\usbdescr.c ............ USB descriptors for the 3DP, PP & FFP
firmware\usbdescr.h ............ USB descriptors header file
firmware\usbconfig.h ........... AVR USB driver configuration

firmware\bin\empty ............. Place holder. Output directory for AVRS

firmware\usbdrv\* .............. AVR USB driver, for more information visit
                                 http://www.obdev.at/products/vusb/index.html

2010-05-21 Detlef "Grendel" Mueller <detlef@gmail.com>
http://code.google.com/p/sw3dprousb/
