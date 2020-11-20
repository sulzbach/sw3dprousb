/* Name: usbconfig.h
 * Project: AVR USB driver
 * Author: Christian Starkjohann
 * Creation Date: 2005-04-01
 * Tabsize: 4
 * Copyright: (c) 2005 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: Proprietary, free under certain conditions. See Documentation.
 * This Revision: $Id: usbconfig.h 1.14 2009/03/07 08:16:03 Detlef Exp Detlef $
 */

#ifndef __usbconfig_h_included__
#define __usbconfig_h_included__

/*
General Description:
This file is the configuration for the USB driver.
*/

/* ---------------------------- Hardware Config ---------------------------- */

#include "f_cpu.h"

#define USB_CFG_CLOCK_KHZ       (F_CPU/1000)
/* Clock rate of the AVR in MHz. Legal values are 12000, 15000, 16000 or 16500.
 * Default if not specified: 12 MHz
 */

#if	defined(__AVR_ATmega8__)

#define USB_CFG_IOPORTNAME      B
/* This is the port where the USB bus is connected.
 */
#define USB_CFG_DMINUS_BIT      0
/* This is the bit number in USB_CFG_IOPORT where the USB D- line is connected.
 */
#define USB_CFG_DPLUS_BIT       1
/* This is the bit number in USB_CFG_IOPORT where the USB D+ line is connected.
 * Please note that D+ must also be connected to interrupt pin INT0!
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

#elif	defined(__AVR_ATtiny461__) || defined(__AVR_ATtiny861__)

#define USB_CFG_IOPORTNAME      B
/* This is the port where the USB bus is connected.
 */
#define USB_CFG_DMINUS_BIT      3
/* This is the bit number in USB_CFG_IOPORT where the USB D- line is connected.
 */
#define USB_CFG_DPLUS_BIT       6
/* This is the bit number in USB_CFG_IOPORT where the USB D+ line is connected.
 * Please note that D+ must also be connected to interrupt pin INT0!
 */
/* ----------------------- Optional Hardware Config ------------------------ */

#define USB_CFG_PULLUP_IOPORTNAME   A
/* If you connect the 1.5k pullup resistor from D- to a port pin instead of
 * V+, you can connect and disconnect the device from firmware by calling
 * the macros usbDeviceConnect() and usbDeviceDisconnect() (see usbdrv.h).
 * This constant defines the port on which the pullup resistor is connected.
 */
#define USB_CFG_PULLUP_BIT          3
/* This constant defines the bit number in USB_CFG_PULLUP_IOPORT (defined
 * above) where the 1.5k pullup resistor is connected. See description
 * above for details.
 */

#endif

/* --------------------------- Functional Range ---------------------------- */

#define USB_CFG_HAVE_INTRIN_ENDPOINT    1
/* Define this to 1 if you want to compile a version with two endpoints: The
 * default control endpoint 0 and an interrupt-in endpoint 1.
 */
#define USB_CFG_HAVE_INTRIN_ENDPOINT3   0
/* Define this to 1 if you want to compile a version with three endpoints: The
 * default control endpoint 0, an interrupt-in endpoint 1 and an interrupt-in
 * endpoint 3. You must also enable endpoint 1 above.
 */
#define USB_CFG_IMPLEMENT_HALT          0
/* Define this to 1 if you also want to implement the ENDPOINT_HALT feature
 * for endpoint 1 (interrupt endpoint). Although you may not need this feature,
 * it is required by the standard. We have made it a config option because it
 * bloats the code considerably.
 */
#define USB_CFG_INTR_POLL_INTERVAL      10
/* If you compile a version with endpoint 1 (interrupt-in), this is the poll
 * interval. The value is in milliseconds and must not be less than 10 ms for
 * low speed devices. Windows will actually poll every 8ms.
 */
#define USB_CFG_IS_SELF_POWERED         0
/* Define this to 1 if the device has its own power supply. Set it to 0 if the
 * device is powered from the USB bus.
 */
#define USB_CFG_MAX_BUS_POWER           100
/* Set this variable to the maximum USB bus power consumption of your device.
 * The value is in milliamperes. [It will be divided by two since USB
 * communicates power requirements in units of 2 mA.]
 */
#define USB_CFG_SAMPLE_EXACT            1
/* This variable affects Sampling Jitter for USB receiving. When it is 0, the
 * driver guarantees a sampling window of 1/2 bit. The USB spec requires
 * that the receiver has at most 1/4 bit sampling window. The 1/2 bit window
 * should still work reliably enough because we work at low speed. If you want
 * to meet the spec, set this value to 1. This will unroll a loop which
 * results in bigger code size.
 * If you have problems with long cables, try setting this value to 1.
 */
#define USB_CFG_IMPLEMENT_FN_WRITE      0
/* Set this to 1 if you want usbFunctionWrite() to be called for control-out
 * transfers. Set it to 0 if you don't need it and want to save a couple of
 * bytes.
 */
#define USB_CFG_IMPLEMENT_FN_READ       0
/* Set this to 1 if you need to send control replies which are generated
 * "on the fly" when usbFunctionRead() is called. If you only want to send
 * data from a static buffer, set it to 0 and return the data from
 * usbFunctionSetup(). This saves a couple of bytes.
 */
#define USB_CFG_IMPLEMENT_FN_WRITEOUT   0
/* Define this to 1 if you want to use interrupt-out (or bulk out) endpoint 1.
 * You must implement the function usbFunctionWriteOut() which receives all
 * interrupt/bulk data sent to endpoint 1.
 */
#define USB_CFG_HAVE_FLOWCONTROL        0
/* Define this to 1 if you want flowcontrol over USB data. See the definition
 * of the macros usbDisableAllRequests() and usbEnableAllRequests() in
 * usbdrv.h.
 */
#define USB_RESET_HOOK( resetStarts )	if ( resetStarts ) usbConfiguration = 0 ;
/* This macro is a hook if you need to know when an USB RESET occurs. It has
 * one parameter which distinguishes between the start of RESET state and its
 * end.
 */

/* -------------------------- Device Description --------------------------- */

//#define  USB_CFG_VENDOR_ID       0x6D, 0x04	/* Logitech */

#ifdef	INTREL
 #define  USB_CFG_VENDOR_ID       0xDE, 0xC0	/* Test */
// #define  USB_CFG_VENDOR_ID       0x8B, 0x1B	/* ImTech */
#else
 #define  USB_CFG_VENDOR_ID       0x5E, 0x04	/* Microsoft */
#endif

/* USB vendor ID for the device, low byte first. If you have registered your
 * own Vendor ID, define it here
 */

//#define  USB_CFG_DEVICE_ID       0x1F, 0x0F	/* SW 3D Pro */
//#define  USB_CFG_DEVICE_ID       0x21, 0xC0	/* Cyberman II */
//#define  USB_CFG_DEVICE_ID       0x23, 0xC0	/* Interceptor */
#define  USB_CFG_DEVICE_ID       0x01, 0x00     /* Dummy */

#ifdef	INTREL
 #define CFG_DEVICE_ID_3DP       0x01, 0x00
 #define CFG_DEVICE_ID_PP        0x02, 0x00
 #define CFG_DEVICE_ID_FFP       0x03, 0x00
 #define CFG_DEVICE_ID_6DP       0x04, 0x00
#else
 #define CFG_DEVICE_ID_3DP       0x02, 0x00
 #define CFG_DEVICE_ID_PP        0x04, 0x00
 #define CFG_DEVICE_ID_FFP       0x05, 0x00
#endif
/* This is the ID of the product, low byte first. It is interpreted in the
 * scope of the vendor ID. If you have registered your own VID with usb.org
 * or if you have licensed a PID from somebody else, define it here. Otherwise
 * you use obdev's free shared VID/PID pair. Be sure to read the rules in
 * USBID-License.txt!
 * This template uses obdev's shared VID/PID pair for HIDs: 0x16c0/0x5df.
 * Use this VID/PID pair ONLY if you understand the implications!
 */

#define USB_CFG_DEVICE_VERSION  0x00, 0x03
/* Version number of the device: Minor number first, then major number.
 */

#define USB_CFG_VENDOR_NAME	'D','e','t','l','e','f',' ', \
				'<','G','r','e','n','d','e','l','>', ' ', \
				'M','u','e','l','l','e','r'
#define USB_CFG_VENDOR_NAME_LEN 24
/* These two values define the vendor name returned by the USB device.
 */

#ifdef	INTREL

// #define USB_CFG_SERIAL_NUMBER   '0', '0', '0', '0'
// #define USB_CFG_SERIAL_NUMBER   '0', '0', '0', '2'
// #define USB_CFG_SERIAL_NUMBER   '0', '0', '4', '2'
// #define USB_CFG_SERIAL_NUMBER   '0', '0', '5', '4'
// #define USB_CFG_SERIAL_NUMBER   '0', '0', '5', '8'
// #define USB_CFG_SERIAL_NUMBER   '0', '0', '5', '9' /* CrankShaft */
// #define USB_CFG_SERIAL_NUMBER   '0', '0', '6', '0'

// #define USB_CFG_SERIAL_NUMBER   '0', '0', '2', '7'
// #define USB_CFG_SERIAL_NUMBER   '0', '0', '3', '1'
// #define USB_CFG_SERIAL_NUMBER   '0', '0', '3', '0'
// #define USB_CFG_SERIAL_NUMBER   '0', '0', '3', '5'

// #define USB_CFG_SERIAL_NUMBER   '1', '0', '0', '0' /* COMPDEV test */

 #define USB_CFG_SERIAL_NUMBER   '1', '0', '0', '1' /* SIXAXIS mod */

#if	defined(__AVR_ATtiny461__) || defined(__AVR_ATtiny861__)
 #undef  USB_CFG_SERIAL_NUMBER
 #define USB_CFG_SERIAL_NUMBER   '0', '1', '0', '0' /* 1st tiny proto */
#endif

#else
 #define USB_CFG_SERIAL_NUMBER   '1', '2', '3', '4'
#endif

#define USB_CFG_SERIAL_NUMBER_LEN   4
/* Same as above for the serial number.
 */

#define USB_CFG_DEVICE_CLASS        0
#define USB_CFG_DEVICE_SUBCLASS     0
/* See USB specification if you want to conform to an existing device class.
 */

#define USB_CFG_INTERFACE_CLASS     3   /* HID */
#define USB_CFG_INTERFACE_SUBCLASS  0
#define USB_CFG_INTERFACE_PROTOCOL  0
/* This template defines a HID class device.
 */

/* ------------------- Fine Control over USB Descriptors ------------------- */
/* If you don't want to use the driver's default USB descriptors, you can
 * provide our own. These can be provided as (1) fixed length static data in
 * flash memory, (2) fixed length static data in RAM or (3) dynamically at
 * runtime in the function usbFunctionDescriptor(). See usbdrv.h for more
 * information about this function.
 * Descriptor handling is configured through the descriptor's properties. If
 * no properties are defined or if they are 0, the default descriptor is used.
 * Possible properties are:
 *   + USB_PROP_IS_DYNAMIC: The data for the descriptor should be fetched
 *     at runtime via usbFunctionDescriptor().
 *   + USB_PROP_IS_RAM: The data returned by usbFunctionDescriptor() or found
 *     in static memory is in RAM, not in flash memory.
 *   + USB_PROP_LENGTH(len): If the data is in static memory (RAM or flash),
 *     the driver must know the descriptor's length. The descriptor itself is
 *     found at the address of a well known identifier (see below).
 * List of static descriptor names (must be declared PROGMEM if in flash):
 *   char usbDescriptorDevice[];
 *   char usbDescriptorConfiguration[];
 *   char usbDescriptorHidReport[];
 *   char usbDescriptorString0[];
 *   int usbDescriptorStringVendor[];
 *   int usbDescriptorStringDevice[];
 *   int usbDescriptorStringSerialNumber[];
 * Other descriptors can't be provided statically, they must be provided
 * dynamically at runtime.
 *
 * Descriptor properties are or-ed or added together, e.g.:
 * #define USB_CFG_DESCR_PROPS_DEVICE   (USB_PROP_IS_RAM | USB_PROP_LENGTH(18))
 *
 * The following descriptors are defined:
 *   USB_CFG_DESCR_PROPS_DEVICE
 *   USB_CFG_DESCR_PROPS_CONFIGURATION
 *   USB_CFG_DESCR_PROPS_STRINGS
 *   USB_CFG_DESCR_PROPS_STRING_0
 *   USB_CFG_DESCR_PROPS_STRING_VENDOR
 *   USB_CFG_DESCR_PROPS_STRING_PRODUCT
 *   USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER
 *   USB_CFG_DESCR_PROPS_HID
 *   USB_CFG_DESCR_PROPS_HID_REPORT
 *   USB_CFG_DESCR_PROPS_UNKNOWN (for all descriptors not handled by the driver)
 *
 */

#define USB_CFG_DESCR_PROPS_DEVICE                  USB_PROP_IS_DYNAMIC
#define USB_CFG_DESCR_PROPS_CONFIGURATION           USB_PROP_IS_DYNAMIC
#define USB_CFG_DESCR_PROPS_STRINGS                 0
#define USB_CFG_DESCR_PROPS_STRING_0                0
#define USB_CFG_DESCR_PROPS_STRING_VENDOR           0
#define USB_CFG_DESCR_PROPS_STRING_PRODUCT          USB_PROP_IS_DYNAMIC
#define USB_CFG_DESCR_PROPS_STRING_SERIAL_NUMBER    (USB_PROP_IS_DYNAMIC+USB_PROP_IS_RAM)
#define USB_CFG_DESCR_PROPS_HID                     USB_PROP_IS_DYNAMIC
#define USB_CFG_DESCR_PROPS_HID_REPORT              USB_PROP_IS_DYNAMIC
#define USB_CFG_DESCR_PROPS_UNKNOWN                 0

/* ----------------------- Optional MCU Description ------------------------ */

/* The following configurations have working defaults in usbdrv.h. You
 * usually don't need to set them explicitly. Only if you want to run
 * the driver on a device which is not yet supported or with a compiler
 * which is not fully supported (such as IAR C) or if you use a differnt
 * interrupt than INT0, you may have to define some of these.
 */
/* #define USB_INTR_CFG            MCUCR */
/* #define USB_INTR_CFG_SET        ((1 << ISC00) | (1 << ISC01)) */
/* #define USB_INTR_CFG_CLR        0 */
/* #define USB_INTR_ENABLE         GIMSK */
/* #define USB_INTR_ENABLE_BIT     INT0 */
/* #define USB_INTR_PENDING        GIFR */
/* #define USB_INTR_PENDING_BIT    INTF0 */

#endif /* __usbconfig_h_included__ */
