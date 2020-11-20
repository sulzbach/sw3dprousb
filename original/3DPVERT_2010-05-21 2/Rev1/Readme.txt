This is the README file for the 3DP-Vert Rev1
=============================================

3DP-Vert is a device to connect a Microsoft SideWinder 3D Pro, Precision
Pro, or Force Feedback Pro joystick as a generic USB HID Joystick to a
computer while retaining full functionallity (8-way hat, slider, and all
buttons) at a high readout rate (100Hz) with low latency (< 5ms).


LICENSE
-------

This project is built with the AVR-USB driver from Objective Development,
which is published terms and conditions of the GNU GPL version 2 license.
3DP-Vert is distributed under the same license conditions. See the file
"firmware\usbdrv\License.txt" for details.


TOOLCHAIN
---------

The converter is realized using an Atmel ATmega8 AVR micro controller,
Objective Development's firmware-only USB driver AVR-USB, and Objective
Development's BootloadHID.

http://atmel.com/dyn/products/product_card.asp?part_id=2004
http://www.obdev.at/products/avrusb/index.html
http://www.obdev.at/products/avrusb/bootloadhid.html

The 3DP-Vert firmware was developt under Windows XP Pro using WinAVR
20071221 and Atmel AVR Studio 4.13 SP2b571. The ISP programmer used
is PonyProg2000, v2.07c BETA.

http://sourceforge.net/projects/winavr/
http://atmel.com/dyn/products/tools_card.asp?tool_id=2725
http://www.lancos.com/ppwin95.html


BUILDING 3DP-VERT FROM SCRATCH
------------------------------

Build the hardware, see the directory "Rev1\schematic". Configure PonyProg
according to http://www.flightsim.com/cgi/kds?$=main/howto/mind.htm ,
"Programming Procedure", and program "Rev1\3dpv12m8.hex" into the
ATmega8. Program the configuration and security bits according to
"Rev1\schematic\Fuses8.gif".

Refer to "Doc\Manualv3.pdf" for operating instructions.

NOTES:

C8 & C9 are 10nF capacitors in the schematic and should be changed to
4.7nF capacitors. "Rev1\schematic\bom.txt" reflects this.

You can install a 16MHz crystal for Y1 now. Change the F_CPU defines in
"firmware\f_cpu.h" and "Rev1\bootloader\firmware\bootloaderconfig.h" then
recompile both projects. Upload the bootloader first via an ISP, then use
it to upload the 3DPro main firmware (see below.)

BUILDING AND INSTALLING THE FIRMWARE
------------------------------------

To compile the firmware, install the GNU toolchain for your operating
system. Change to the firmware directory and run "make all". Use the
bootloader to upload "main.hex" (assuming you installed "3dpv12m8.hex"
to get going in the first place.) To start the bootloader:

 - have the converter connected to the computer with a 3DPro attached
 - make sure the mode select switch of the 3DPro is in position 1
 - press and hold the two thumb buttons on the 3DPro's handle, then
   switch the mode select switch to position 2
 - release the buttons as soon the LED starts flashing 10 times at 5Hz

The PP and FFP don't map the buttons, if you don't have a 3DP use the
following procedure:

 - carefully use two wires (or paperclips) to connect pins 4 & 10 and
   pins 5 & 14 of the DA15 connector.
 - connect the converter to the computer


Use "Rev1\bootloader\bin\bootloadHID.exe" to upload "main.hex" to the
3DP-Vert. The commandline is

  bootloadHID -r main.hex


NOTES
-----

The 3DP-Vert uses Microsofts VID (045E.) For the PIDs it does not use the
original id's (3DP - 0F1F, PP - 0008/0017. FFP - 0006) since DirectInput
knows these and adds some special handling to a device with these
VID/PID pairs (eg. it overrides the device name.) Some analysis of the
relevant INF files in W2000 and WXP shows that 0002 (3DP), 0004 (PP), and
0005 (FFP) are free PIDs and very unlikely to be used in the future by MS.


FILES IN THIS DISTRIBUTION
--------------------------

Rev1\Readme.txt ............................. This file
Rev1\3dpv12m8.hex ........................... 3DP-Vert image, serial #1234, includes the bootloader, 12MHz

Rev1\schematic\3dpschem.pdf ................. 3DP-Vert schematic
Rev1\schematic\avrisp.gif ................... Simple ISP schematic for use w/ PonyProg
Rev1\schematic\Board1.jpg ................... Prototype on a breadboard
Rev1\schematic\Board2.jpg ................... Prototype on a breadboard
Rev1\schematic\bom.txt ...................... Bill of materials with digikey.com order numbers
Rev1\schematic\Fuses8.gif ................... 3DP-Vert fuse settings (PonyProg)

Rev1\bootloader\bin\bootloadHID.exe ......... W32 executable to upload new code to the 3DP-Vert

Rev1\bootloader\firmware\BootHID.aps ........ AVR Studio project file for the boot loader
Rev1\bootloader\firmware\bootloaderconfig.h . Bootloader configuration
Rev1\bootloader\firmware\main.c ............. Bootloader main module
Rev1\bootloader\firmware\Makefile ........... Makefile to be used w/ AVRGCC w/o AVRS
Rev1\bootloader\firmware\usbconfig.h ........ USB configuration

Rev1\bootloader\firmware\bin\empty .......... Placeholder. Will contain object & image files after build w/ AVRS

Rev1\bootloader\firmware\usbdrv\empty ....... Placeholder. Copy firmware\usbdrv\* here.


2008-05-09 Detlef "Grendel" Mueller <detlef@gmail.com>
http://grendel.koolbear.com/3dpro.htm
