/* Name: bootloaderconfig.h
 * Project: AVR bootloader HID
 * Author: Christian Starkjohann
 * Creation Date: 2007-03-19
 * Tabsize: 4
 * Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt)
 * Based on: bootloaderconfig.h 281 2007-03-20 13:22:10Z cs
 * This Revision: $Id: bootloaderconfig.h 1.2 2007/07/05 10:47:26 Detlef Exp Detlef $
 */

#ifndef __bootloaderconfig_h_included__
#define __bootloaderconfig_h_included__

/*
General Description:
This file (together with some settings in Makefile) configures the boot loader
according to the hardware.

This file contains (besides the hardware configuration normally found in
usbconfig.h) two functions or macros: bootLoaderInit() and
bootLoaderCondition(). Whether you implement them as macros or as static
inline functions is up to you, decide based on code size and convenience.

bootLoaderInit() is called as one of the first actions after reset. It should
be a minimum initialization of the hardware so that the boot loader condition
can be read. This will usually consist of activating a pull-up resistor for an
external jumper which selects boot loader mode.

bootLoaderCondition() is called immediately after initialization and in each
main loop iteration. If it returns TRUE, the boot loader will be active. If it
returns FALSE, the boot loader jumps to address 0 (the loaded application)
immediately.

For compatibility with Thomas Fischl's avrusbboot, we also support the macro
names BOOTLOADER_INIT and BOOTLOADER_CONDITION for this functionality. If
these macros are defined, the boot loader usees them.
*/

/* ---------------------------- Hardware Config ---------------------------- */

// #define F_CPU                   16000000L
// #define F_CPU                   15000000L
#define F_CPU                   12000000L

#define USB_CFG_IOPORTNAME      B
/* This is the port where the USB bus is connected. When you configure it to
 * "B", the registers PORTB, PINB and DDRB will be used.
 */
#define USB_CFG_DMINUS_BIT      0
/* This is the bit number in USB_CFG_IOPORT where the USB D- line is connected.
 * This may be any bit in the port.
 */
#define USB_CFG_DPLUS_BIT       1
/* This is the bit number in USB_CFG_IOPORT where the USB D+ line is connected.
 * This may be any bit in the port. Please note that D+ must also be connected
 * to interrupt pin INT0!
 */

#define USB_CFG_CLOCK_KHZ       (F_CPU/1000)
/* Clock rate of the AVR in MHz. Legal values are 12000, 16000 or 16500.
 * The 16.5 MHz version of the code requires no crystal, it tolerates +/- 1%
 * deviation from the nominal frequency. All other rates require a precision
 * of 2000 ppm and thus a crystal!
 * Default if not specified: 12 MHz
 */

/* ----------------------- Optional Hardware Config ------------------------ */

#define USB_CFG_PULLUP_IOPORTNAME   B
/* If you connect the 1.5k pullup resistor from D- to a port pin instead of
 * V+, you can connect and disconnect the device from firmware by calling
 * the macros usbDeviceConnect() and usbDeviceDisconnect() (see usbdrv.h).
 * This constant defines the port on which the pullup resistor is connected.
 */
#define USB_CFG_PULLUP_BIT          2
/* This constant defines the bit number in USB_CFG_PULLUP_IOPORT (defined
 * above) where the 1.5k pullup resistor is connected. See description
 * above for details.
 */

/* --------------------------- Functional Range ---------------------------- */

#define BOOTLOADER_CAN_EXIT     1
/* If this macro is defined to 1, the boot loader command line utility can
 * initiate a reboot after uploading the FLASH when the "-r" command line
 * option is given. If you define it to 0 or leave it undefined, the "-r"
 * option won't work and you save a couple of bytes in the boot loader. This
 * may be of advantage if you compile with gcc 4 instead of gcc 3 because it
 * generates slightly larger code.
 */
 
 /* ------------------------------------------------------------------------- */

#ifndef __ASSEMBLER__   /* assembler cannot parse function definitions */

//      Pinout
//      ======
//
//      PB0     USB D-          PC0     X-axis 1        PD0     Trigger
//      PB1     USB D+          PC1     Y-axis 2        PD1     LED
//      PB2     USB Id          PC2     -               PD2     USB D+
//      PB3     (MOSI)          PC3     -               PD3     Button 1
//      PB4     (MISO)          PC4     -               PD4     Button 1
//      PB5     (SCK)           PC5     -               PD5     Button 2
//      PB6     XTal            PC5     (RST)           PD6     Button 3
//      PB7     XTal                                    PD7     Button 4

#define T1PS_1024	((1<<CS12)+(0<<CS11)+(1<<CS10))

#define PBPU    ((0 <<  PB7) | (0 <<  PB6) | (1 <<  PB5) | (1 <<  PB4) | \
                 (1 <<  PB3) | (0 <<  PB2) | (0 <<  PB1) | (0 <<  PB0))

#define DDB     ((0 << DDB7) | (0 << DDB6) | (0 << DDB5) | (0 << DDB4) | \
                 (0 << DDB3) | (0 << DDB2) | (0 << DDB1) | (0 << DDB0))

#define PCPU    (              (1 <<  PC6) | (1 <<  PC5) | (1 <<  PC4) | \
                 (1 <<  PC3) | (1 <<  PC2) | (1 <<  PC1) | (1 <<  PC0))

#define DDC     (              (0 << DDC6) | (0 << DDC5) | (0 << DDC4) | \
                 (0 << DDC3) | (0 << DDC2) | (0 << DDC1) | (0 << DDC0))

#define PDPU    ((1 <<  PD7) | (1 <<  PD6) | (1 <<  PD5) | (1 <<  PD4) | \
                 (0 <<  PD3) | (0 <<  PD2) | (1 <<  PD1) | (1 <<  PD0))

#define DDD     ((0 << DDD7) | (0 << DDD6) | (0 << DDD5) | (0 << DDD4) | \
                 (0 << DDD3) | (0 << DDD2) | (1 << DDD1) | (1 << DDD0))

#define bootLoaderInit()	\
    DDRB   = DDB ;		\
    PORTB  = PBPU ;		\
    DDRC   = DDC ;		\
    PORTC  = PCPU ;		\
    DDRD   = DDD ;		\
    PORTD  = PDPU ;		\
    TCCR1B = T1PS_1024

#define bootLoaderCondition() ((PIND & 0xF0) == ((1 << PD5) | (1 << PD4)))

#define LED_OFF()       (PORTD |=  (1 << PD1))
#define LED_ON()        (PORTD &= (uint8_t)~(1 << PD1))

#endif

/* ------------------------------------------------------------------------- */

#endif /* __bootloader_h_included__ */
