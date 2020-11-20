/*******************************************************************************
 * File Name	: usbdescr.c
 * Project	: Microsoft Sidewinder 3D Pro to USB converter
 * Date		: 2007/06/11
 * Version	: 2.0
 * Target MCU	: ATmega8
 * Tool Chain	: Atmel AVR Studio 4.11 SP4 / WinAVR 20070122
 * Author	: Detlef "Grendel" Mueller
 *		  detlef@gmail.com
 * References	: USB Stack by Objective Development, http://www.obdev.at/avrusb
 *		  HIDKeys.2006-03-14 by Christian Starkjohann, OD
 * Description	: USB to 3D Pro converter USB descriptors
 * Device	: Microsoft Sidewinder 3D Pro/Precision Pro/Force Feedback Pro
 *
 * Release Notes:
 *
 * $Id: usbdescr.c 1.4 2009/03/07 08:17:42 Detlef Exp Detlef $
 *
 * $Log: usbdescr.c $
 * Revision 1.4  2009/03/07 08:17:42  Detlef
 * Added experimental SIXAXIS device
 *
 * Revision 1.3  2009/02/02 01:29:51  Detlef
 * Added support for an experimental composite device (keyboard/joystick).
 *
 * Revision 1.2  2007/06/26 07:32:05  Detlef
 * Changed USB version to 1.10
 *
 * Revision 1.1  2007/06/12 03:44:19  Detlef
 * Initial revision
 *
 *
 ******************************************************************************/

#include "3DPro.h"

#include "usbdrv.h"
#include "usbdescr.h"

/* ------------------------------------------------------------------------- */

const prog_uint8_t
    usbHidReportDesc3DP[] =
    {
	0x05,0x01,		// USAGE_PAGE (Generic Desktop)
	0x09,0x04,		// USAGE (Joystick)
	0xA1,0x01,		// COLLECTION (Application)
#if COMPDEV
	0x85, 0x01,		//   Report Id (1)
#endif
	0x09,0x01,		//   USAGE (Pointer)
	0xA1,0x00,		//   COLLECTION (Physical)
	0x95,0x02,		//     REPORT_COUNT (2)
	0x75,0x0A,		//     REPORT_SIZE (10)
	0x16,0x00,0xFE,		//     LOGICAL_MINIMUM (-512)
	0x26,0xFF,0x01,		//     LOGICAL_MAXIMUM (511)
#if HID_PHYS
	0x35,0x00,		//     PHYSICAL_MINIMUM (0)				-2
	0x46,0xFF,0x03,		//     PHYSICAL_MAXIMUM (1023)				-3
#endif
	0xA4,			//     PUSH
	0x09,0x30,		//     USAGE (X)
	0x09,0x31,		//     USAGE (Y)
	0x81,0x02,		//     INPUT (Data,Var,Abs)             20b X/Y
#if SIXAXIS
	0x75,0x08,		//     REPORT_SIZE (8)
	0x15,0x80,		//     LOGICAL_MINIMUM (-128)
	0x25,0x7F,		//     LOGICAL_MAXIMUM (127)
#if HID_PHYS
	0x46,0xFF,0x00,		//     PHYSICAL_MAXIMUM (255)				-3 SIX
#endif
	0x09,0x33,		//     USAGE (Rx)
	0x09,0x34,		//     USAGE (Ry)
	0x81,0x02,		//     INPUT (Data,Var,Abs)             16b Rx/Ry	+15 bytes
#endif
	0x95,0x01,		//     REPORT_COUNT (1)
	0x75,0x09,		//     REPORT_SIZE (9)
	0x16,0x00,0xFF,		//     LOGICAL_MINIMUM (-256)
	0x26,0xFF,0x00,		//     LOGICAL_MAXIMUM (255)
#if HID_PHYS
	0x45,0x2E,		//     PHYSICAL_MAXIMUM (46)				-2
	0x65,0x14,		//     UNIT (Eng Rot:Angular Pos)			-2
#endif
	0x09,0x35,		//     USAGE (Rz)
	0x81,0x02,		//     INPUT (Data,Var,Abs)              9b Rz
#if ! SIXAXIS
	0x75,0x04,		//     REPORT_SIZE (4)
	0x15,0x00,		//     LOGICAL_MINIMUM (0)
	0x25,0x07,		//     LOGICAL_MAXIMUM (7)
#if HID_PHYS
	0x46,0x3B,0x01,		//     PHYSICAL_MAXIMUM (315)				-3 ! SIX
#endif
	0x09,0x39,		//     USAGE (Hat switch)
	0x81,0x42,		//     INPUT (Data,Var,Abs,Null)         4b Hat		-13 bytes
#endif
	0x95,0x04,		//     REPORT_COUNT (4)
	0x75,0x01,		//     REPORT_SIZE (1)
#if SIXAXIS
	0x15,0x00,		//     LOGICAL_MINIMUM (0)				 +2 bytes
#endif
	0x25,0x01,		//     LOGICAL_MAXIMUM (1)
#if HID_PHYS
	0x45,0x01,		//     PHYSICAL_MAXIMUM (1)				-2
	0x65,0x00,		//     UNIT (None)					-2
#endif
	0x05,0x09,		//     USAGE_PAGE (Button)
	0x19,0x01,		//     USAGE_MINIMUM (Button 1)
	0x29,0x04,		//     USAGE_MAXIMUM (Button 4)
	0x81,0x02,		//     INPUT (Data,Var,Abs)              4b B1-4
	0xC0,			//   END_COLLECTION
	0x19,0x05,		//   USAGE_MINIMUM (Button 5)
	0x29,0x08,		//   USAGE_MAXIMUM (Button 8)
	0x81,0x02,		//   INPUT (Data,Var,Abs)                4b B5-8
	0xB4,			//   POP
	0x95,0x01,		//   REPORT_COUNT (1)
	0x09,0x36,		//   USAGE (Slider)
	0x81,0x02,		//   INPUT (Data,Var,Abs)               10b Throttle
#if SIXAXIS
	0x75,0x01,		//   REPORT_SIZE (1)			 1b Fill
#else
	0x75,0x05,		//   REPORT_SIZE (5)			 5b Fill
#endif
	0x81,0x01,		//   INPUT (Cnst,Ary,Abs)
	0xC0			// END_COLLECTION                       56b -> 7bytes, 8 bytes for SIXAXIS

#if COMPDEV
	,
	0x05, 0x01,		// USAGE_PAGE (Generic Desktop)
	0x09, 0x06,		// USAGE (Keyboard)
	0xA1, 0x01,		// COLLECTION (Application)
	0x85, 0x02,		//   Report Id (2)
	0x05, 0x07,		//   USAGE_PAGE (Keyboard)
	0x19, 0xE0,		//   USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xE7,		//   USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00,		//   LOGICAL_MINIMUM (0)
	0x25, 0x01,		//   LOGICAL_MAXIMUM (1)
	0x75, 0x01,		//   REPORT_SIZE (1)
	0x95, 0x08,		//   REPORT_COUNT (8)
	0x81, 0x02,		//   INPUT (Data,Var,Abs)
	0x95, 0x02,		//   REPORT_COUNT (2)
	0x75, 0x08,		//   REPORT_SIZE (8)
	0x25, 0x65,		//   LOGICAL_MAXIMUM (101)
	0x19, 0x00,		//   USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x65,		//   USAGE_MAXIMUM (Keyboard Application)
	0x81, 0x00,		//   INPUT (Data,Ary,Abs)
	0xC0			// END_COLLECTION
#endif // COMPDEV
    },

    usbHidReportDescFFP[] =
    {
	0x05, 0x01,		// USAGE_PAGE (Generic Desktop)
	0x09, 0x04,		// USAGE (Joystick)
	0xA1, 0x01,		// COLLECTION (Application)
#if COMPDEV
	0x85, 0x01,		//  Report Id (1)
#endif
	0x09, 0x01,		//  USAGE (Pointer)
	0xA1, 0x00,		//   COLLECTION (Physical)
	0x95, 0x02,		//     REPORT_COUNT (2)
	0x75, 0x0A,		//     REPORT_SIZE (10)
	0x16, 0x00, 0xFE,	//     LOGICAL_MINIMUM (-512)
	0x26, 0xFF, 0x01,	//     LOGICAL_MAXIMUM (511)
#if HID_PHYS
	0x35, 0x00,		//     PHYSICAL_MINIMUM (0)				-2
	0x46, 0xFF, 0x03,	//     PHYSICAL_MAXIMUM (1023)				-3
#endif
	0x09, 0x30,		//     USAGE (X)
	0x09, 0x31,		//     USAGE (Y)
	0x81, 0x02,		//     INPUT (Data,Var,Abs)		20b X/Y
	0x95, 0x01,		//     REPORT_COUNT (1)
	0x75, 0x04,		//     REPORT_SIZE (4)
	0x15, 0x00,		//     LOGICAL_MINIMUM (0)
	0x25, 0x07,		//     LOGICAL_MAXIMUM (7)
#if HID_PHYS
	0x46, 0x3B, 0x01,	//     PHYSICAL_MAXIMUM (315)				-3
	0x65, 0x14,		//     UNIT (Eng Rot:Angular Pos)			-2
#endif
	0x09, 0x39,		//     USAGE (Hat switch)
	0x81, 0x42,		//     INPUT (Data,Var,Abs,Null)	 4b Hat
	0x75, 0x06,		//     REPORT_SIZE (6)
	0x15, 0xE0,		//     LOGICAL_MINIMUM (-32)
	0x25, 0x1F,		//     LOGICAL_MAXIMUM (31)
#if HID_PHYS
	0x45, 0x2E,		//     PHYSICAL_MAXIMUM (46)				-2
#endif
	0x09, 0x35,		//     USAGE (Rz)
	0x81, 0x02,		//     INPUT (Data,Var,Abs)		 6b Rz
	0x95, 0x04,		//     REPORT_COUNT (4)
	0x75, 0x01,		//     REPORT_SIZE (1)
	0x15, 0x00,		//     LOGICAL_MINIMUM (0)
	0x25, 0x01,		//     LOGICAL_MAXIMUM (1)
#if HID_PHYS
	0x45, 0x01,		//     PHYSICAL_MAXIMUM (1)				-2
	0x65, 0x00,		//     UNIT (None)					-2
#endif
	0xA4,			//     PUSH
	0x05, 0x09,		//     USAGE_PAGE (Button)
	0x19, 0x01,		//     USAGE_MINIMUM (Button 1)
	0x29, 0x04,		//     USAGE_MAXIMUM (Button 4)
	0x81, 0x02,		//     INPUT (Data,Var,Abs)		 4b B1-4
	0xC0,			//   END_COLLECTION
	0x95, 0x05,		//   REPORT_COUNT (5)
	0x19, 0x05,		//   USAGE_MINIMUM (Button 5)
	0x29, 0x09,		//   USAGE_MAXIMUM (Button 9)
	0x81, 0x02,		//   INPUT (Data,Var,Abs)		 5b B5-9
	0xB4,			//   POP
	0x95, 0x01,		//   REPORT_COUNT (1)
	0x75, 0x07,		//   REPORT_SIZE (7)
	0x15, 0xC0,		//   LOGICAL_MINIMUM (-64)
	0x25, 0x3F,		//   LOGICAL_MAXIMUM (63)
#if HID_PHYS
	0x45, 0x7F,		//   PHYSICAL_MAXIMUM (127)				-2
#endif
	0x09, 0x36,		//   USAGE (Slider)
	0x81, 0x02,		//   INPUT (Data,Var,Abs)		 7b Throttle
	0x75, 0x02,		//   REPORT_SIZE (2)
	0x81, 0x01,		//   INPUT (Cnst,Ary,Abs)		 2b Fill
	0xC0			// END_COLLECTION			48b -> 6bytes

#if COMPDEV
	,
	0x05, 0x01,		// USAGE_PAGE (Generic Desktop)
	0x09, 0x06,		// USAGE (Keyboard)
	0xA1, 0x01,		// COLLECTION (Application)
	0x85, 0x02,		//   Report Id (2)
	0x05, 0x07,		//   USAGE_PAGE (Keyboard)
	0x19, 0xE0,		//   USAGE_MINIMUM (Keyboard LeftControl)
	0x29, 0xE7,		//   USAGE_MAXIMUM (Keyboard Right GUI)
	0x15, 0x00,		//   LOGICAL_MINIMUM (0)
	0x25, 0x01,		//   LOGICAL_MAXIMUM (1)
	0x75, 0x01,		//   REPORT_SIZE (1)
	0x95, 0x08,		//   REPORT_COUNT (8)
	0x81, 0x02,		//   INPUT (Data,Var,Abs)
	0x95, 0x02,		//   REPORT_COUNT (2)
	0x75, 0x08,		//   REPORT_SIZE (8)
	0x25, 0x65,		//   LOGICAL_MAXIMUM (101)
	0x19, 0x00,		//   USAGE_MINIMUM (Reserved (no event indicated))
	0x29, 0x65,		//   USAGE_MAXIMUM (Keyboard Application)
	0x81, 0x00,		//   INPUT (Data,Ary,Abs)
	0xC0			// END_COLLECTION
#endif // COMPDEV
    } ;

/* ------------------------------------------------------------------------- */

const prog_uint16_t
    usbDescStringDevice3DP[] =
    {
	USB_STRING_DESCRIPTOR_HEADER( CFG_DEVICE_NAME_SZ_3DP ),
	CFG_DEVICE_NAME_3DP
    },

    usbDescStringDevicePP[] =
    {
	USB_STRING_DESCRIPTOR_HEADER( CFG_DEVICE_NAME_SZ_PP ),
	CFG_DEVICE_NAME_PP
    },

    usbDescStringDeviceFFP[] =
    {
	USB_STRING_DESCRIPTOR_HEADER( CFG_DEVICE_NAME_SZ_FFP ),
	CFG_DEVICE_NAME_FFP
    } ;

/* ------------------------------------------------------------------------- */

const prog_uint8_t
    usbDescDevice3DP[] =
    {				// USB device descriptor
	18,			// length of descriptor in bytes
	USBDESCR_DEVICE,	// descriptor type
	LVAL( 0x0110),		// USB version supported
	USB_CFG_DEVICE_CLASS,
	USB_CFG_DEVICE_SUBCLASS,
	0,			// Protocol
	8,			// max packet size
	USB_CFG_VENDOR_ID,	// 2 bytes
#if SIXAXIS
	CFG_DEVICE_ID_6DP,	// 2 bytes
#else
	CFG_DEVICE_ID_3DP,	// 2 bytes
#endif
	USB_CFG_DEVICE_VERSION,	// 2 bytes
	1,			// manufacturerstring index
	2,			// product string index
	3,			// serial number string index
	1			// number of configurations
    },

    usbDescDevicePP[] =
    {				// USB device descriptor
	18,			// length of descriptor in bytes
	USBDESCR_DEVICE,	// descriptor type
	LVAL( 0x0110),		// USB version supported
	USB_CFG_DEVICE_CLASS,
	USB_CFG_DEVICE_SUBCLASS,
	0,			// Protocol
	8,			// max packet size
	USB_CFG_VENDOR_ID,	// 2 bytes
	CFG_DEVICE_ID_PP,	// 2 bytes
	USB_CFG_DEVICE_VERSION,	// 2 bytes
	1,			// manufacturerstring index
	2,			// product string index
	3,			// serial number string index
	1			// number of configurations
    },

    usbDescDeviceFFP[] =
    {				// USB device descriptor
	18,			// length of descriptor in bytes
	USBDESCR_DEVICE,	// descriptor type
	LVAL( 0x0110),		// USB version supported
	USB_CFG_DEVICE_CLASS,
	USB_CFG_DEVICE_SUBCLASS,
	0,			// protocol
	8,			// max packet size
	USB_CFG_VENDOR_ID,	// 2 bytes
	CFG_DEVICE_ID_FFP,	// 2 bytes
	USB_CFG_DEVICE_VERSION,	// 2 bytes
	1,			// manufacturerstring index
	2,			// product string index
	3,			// serial number string index
	1			// number of configurations
    } ;

/* ------------------------------------------------------------------------- */

const prog_uint8_t
    usbDescConfiguration3DP[] =
    {				// USB configuration descriptor
	9,			// length of descriptor in bytes
	USBDESCR_CONFIG,	// descriptor type
	LVAL( 9 + 9 + 9 + 7 ),	// total length of data returned (including inlined descriptors)
	1,			// number of interfaces in this configuration
	1,			// index of this configuration
	0,			// configuration name string index
	_BV( 7 ),		// attributes
	USB_CFG_MAX_BUS_POWER/2,// max USB current in 2mA units
				// Interface descriptor
	9,			// length of descriptor in bytes
	USBDESCR_INTERFACE,	// descriptor type
	0,			// index of this interface
	0,			// alternate setting for this interface
	1,			// endpoints excl 0: number of endpoint descriptors to follow
	USB_CFG_INTERFACE_CLASS,
	USB_CFG_INTERFACE_SUBCLASS,
	USB_CFG_INTERFACE_PROTOCOL,
	0,			// string index for interface
				// HID descriptor
	9,			// length of descriptor in bytes
	USBDESCR_HID,		// descriptor type: HID
	LVAL( 0x0111 ),		// BCD representation of HID version
	0,			// target country code
	1,			// number of HID Report (or other HID class) Descriptor infos to follow
	USBDESCR_HID_REPORT,	// descriptor type: report
	LVAL( sizeof( usbHidReportDesc3DP ) ), // total length of report descriptor
				// Endpoint descriptor for endpoint 1
	7,			// length of descriptor in bytes
	USBDESCR_ENDPOINT,	// descriptor type = endpoint
	0x81,			// IN endpoint number 1
	0x03,			// attrib: Interrupt endpoint
	LVAL( CFG_HID_ENDPOINT_PKTSZ_3DP ), // packet size
	USB_CFG_INTR_POLL_INTERVAL // in ms
    },

    usbDescConfigurationFFP[] =
    {				// USB configuration descriptor
	9,			// length of descriptor in bytes
	USBDESCR_CONFIG,	// descriptor type
	LVAL( 9 + 9 + 9 + 7 ),	// total length of data returned (including inlined descriptors)
	1,			// number of interfaces in this configuration
	1,			// index of this configuration
	0,			// configuration name string index
	_BV( 7 ),		// attributes
	USB_CFG_MAX_BUS_POWER/2,// max USB current in 2mA units
				// Interface descriptor
	9,			// length of descriptor in bytes
	USBDESCR_INTERFACE,	// descriptor type
	0,			// index of this interface
	0,			// alternate setting for this interface
	1,			// endpoints excl 0: number of endpoint descriptors to follow
	USB_CFG_INTERFACE_CLASS,
	USB_CFG_INTERFACE_SUBCLASS,
	USB_CFG_INTERFACE_PROTOCOL,
	0,			// string index for interface
				// HID descriptor
	9,			// length of descriptor in bytes
	USBDESCR_HID,		// descriptor type: HID
	LVAL( 0x0111 ),		// BCD representation of HID version
	0x00,			// target country code
	0x01,			// number of HID Report (or other HID class) Descriptor infos to follow
	USBDESCR_HID_REPORT,	// descriptor type: report
	LVAL( sizeof( usbHidReportDescFFP ) ), // total length of report descriptor
				// Endpoint descriptor for endpoint 1
	7,			// length of descriptor in bytes
	USBDESCR_ENDPOINT,	// descriptor type = endpoint
	0x81,			// IN endpoint number 1
	0x03,			// attrib: Interrupt endpoint
	LVAL( CFG_HID_ENDPOINT_PKTSZ_FFP ), // packet size
	USB_CFG_INTR_POLL_INTERVAL // in ms
    } ;

/* ------------------------------------------------------------------------- */
