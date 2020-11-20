/*******************************************************************************
 * File Name	: usbdescr.h
 * Project	: Microsoft Sidewinder 3D Pro to USB converter
 * Date		: 2005/05/31, 2006/12/14
 * Version	: 2.0
 * Target MCU	: ATmega8
 * Tool Chain	: Atmel AVR Studio 4.11 SP4 / WinAVR 20060421
 * Author	: Detlef "Grendel" Mueller
 *		  detlef@gmail.com
 * Release Notes:
 *
 * $Id: usbdescr.h 1.4 2009/03/07 08:17:42 Detlef Exp Detlef $
 *
 * $Log: usbdescr.h $
 * Revision 1.4  2009/03/07 08:17:42  Detlef
 * Added experimental SIXAXIS device
 *
 * Revision 1.3  2009/02/02 01:30:07  Detlef
 * Added support for an experimental composite device (keyboard/joystick).
 *
 * Revision 1.2  2008/05/26 06:58:27  Detlef
 * Moved CFG_DEVICE_ID_.. to usbconfig.h
 *
 * Revision 1.1  2007/06/12 03:44:19  Detlef
 * Initial revision
 *
 ******************************************************************************/

#ifndef	_USBDESCR_H_
#define	_USBDESCR_H_

//-------------------------------------------------------------------------------

#define LSB( n )		( (uint16_t)(n)       & 0xFF)
#define MSB( n )		(((uint16_t)(n) >> 8) & 0xFF)

#define	LVAL( n )		LSB( n ), MSB( n )

#define	USBLV( h, l )		(((uint16_t)(h) << 8) + (l))

//-------------------------------------------------------------------------------
// Descriptor defines

#define CFG_HID_REPORT_DESC_SZ_3DP (84 + HID_PHYS * 16 + COMPDEV * 39 + SIXAXIS * 4)
#define CFG_HID_REPORT_DESC_SZ_FFP (92 + HID_PHYS * 18 + COMPDEV * 39)	/* PP, FFP */

#define CFG_HID_ENDPOINT_PKTSZ_3DP (7+COMPDEV+SIXAXIS)
#define CFG_HID_ENDPOINT_PKTSZ_FFP (6+COMPDEV)			/* PP, FFP */

#define CFG_DEVICE_NAME_3DP	'M','i','c','r','o','s','o','f','t',' ', \
				'S','i','d','e','W','i','n','d','e','r',' ', \
				'3' + SIXAXIS * 3,'D',' ','P','r','o',' ', \
				'(','U','S','B',')'
#define CFG_DEVICE_NAME_SZ_3DP  33

#define CFG_DEVICE_NAME_PP      'M','i','c','r','o','s','o','f','t',' ', \
                                'S','i','d','e','W','i','n','d','e','r',' ', \
                                'P','r','e','c','i','s','i','o','n',' ',\
                                'P','r','o',' ','(','U','S','B',')'
#define CFG_DEVICE_NAME_SZ_PP   40

#define CFG_DEVICE_NAME_FFP     'M','i','c','r','o','s','o','f','t',' ', \
                                'S','i','d','e','W','i','n','d','e','r',' ', \
                                'F','o','r','c','e',' ', \
                                'F','e','e','d','b','a','c','k',' ', \
                                'P','r','o',' ','(','U','S','B',')'
#define CFG_DEVICE_NAME_SZ_FFP  45

#define	USB_DEVICE_NAME_SZ_3DP   (2 + 2 * CFG_DEVICE_NAME_SZ_3DP)
#define	USB_DEVICE_NAME_SZ_PP    (2 + 2 * CFG_DEVICE_NAME_SZ_PP )
#define	USB_DEVICE_NAME_SZ_FFP   (2 + 2 * CFG_DEVICE_NAME_SZ_FFP)

#define	USB_DEVICE_DESC_SZ      18

#define	USB_CONFIG_DESC_SZ      (9 + 9 + 9 + 7)

//-------------------------------------------------------------------------------
// Descriptor declarations

extern const prog_uint8_t
    usbHidReportDesc3DP[CFG_HID_REPORT_DESC_SZ_3DP],
    usbHidReportDescFFP[CFG_HID_REPORT_DESC_SZ_FFP],

    usbDescDevice3DP[USB_DEVICE_DESC_SZ],
    usbDescDevicePP [USB_DEVICE_DESC_SZ],
    usbDescDeviceFFP[USB_DEVICE_DESC_SZ],

    usbDescConfiguration3DP[USB_CONFIG_DESC_SZ],
    usbDescConfigurationFFP[USB_CONFIG_DESC_SZ] ;

extern const prog_uint16_t
    usbDescStringDevice3DP[USB_DEVICE_NAME_SZ_3DP / 2],
    usbDescStringDevicePP [USB_DEVICE_NAME_SZ_PP / 2],
    usbDescStringDeviceFFP[USB_DEVICE_NAME_SZ_FFP / 2] ;

#endif	// _USBDESCR_H_
