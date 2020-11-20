/*******************************************************************************
 * File Name	: main.c
 * Project	: 3DP-Vert, Microsoft Sidewinder 3D Pro/PP/FFP to USB converter
 * Date		: 2006/12/15, 2008/02/12
 * Version	: 3.0
 * Target MCU	: ATmega8/ATtiny461/ATtiny861
 * Tool Chain	: Atmel AVR Studio 4.14 B589 / WinAVR 20071221
 * Author	: Detlef "Grendel" Mueller
 *		  detlef@gmail.com
 * References	: USB Stack by Objective Development, http://www.obdev.at/avrusb
 *		  HIDKeys.2006-03-14 by Christian Starkjohann, OD
 * Description	: USB to 3D Pro converter main program
 * Device	: Microsoft SideWinder 3D Pro, Precision Pro, Force Feedback Pro
 *
 * Release Notes:
 *
 * $Id: main.c 1.17 2009/03/07 08:21:16 Detlef Exp Detlef $
 *
 * $Log: main.c $
 * Revision 1.17  2009/03/07 08:21:16  Detlef
 * Some cleanup.
 *
 * Revision 1.16  2009/02/02 01:28:57  Detlef
 * Added support for an experimental composite device (keyboard/joystick).
 *
 *
 ******************************************************************************/

#include "3DPro.h"

#include "usbdrv.h"
#include "oddebug.h"
#include "usbdescr.h"

/* ------------------------------------------------------------------------- */

#if defined(__AVR_ATmega8__)

 #if COMPDEV

  #define SETTCNT( t )	TCNT2 = (t)
  #define TOVF		TOV2

  #define SETITCNT( t )	TCNT1 = (t)
  #define ITOVF		TOV1

 #else

  #define SETTCNT( t )	TCNT1 = (t)
  #define TOVF		TOV1

 #endif

#elif defined(__AVR_ATtinyX61__)

 #define SETTCNT( t )	\
	    __WRAP__( { TCNT0H = ((t) >> 8) ; TCNT0L = (t) & 0xFF ; } )

 #define TOVF		TOV0

#endif

/* ------------------------------------------------------------------------- */

static uint8_t
    sw_reportsz ;				// Size of SW report in bytes

#if COMPDEV

static uint8_t
    idleRate,					// Idlerate in 4ms increments
    idleCnt ;					// Idle timer (countdown)

static uint8_t
    kb_report[4],				// Keyboard report
    kb_repsav[4] ;				// Last KB report sent

#endif // COMPDEV

/* ------------------------------------------------------------------------- */

// Tiny watchdog is not turned off by a reset, see avr-libc's wdt.h documentation

#if defined(__AVR_ATtinyX61__)

void FA_INIT3( Init3 ) ( void )
{
    MCUSR = 0 ;
    wdt_disable() ;
}

#endif

/* ------------------------------------------------------------------------- */

#define	RET_DESC( desc )	\
	__WRAP__( { usbMsgPtr = VP( desc ) ; return ( sizeof( desc ) ) ; } )

static uint8_t
    usbDescSerial_R[2 + 2 * USB_CFG_SERIAL_NUMBER_LEN] ;

uchar usbFunctionDescriptor ( struct usbRequest *rq )
{
    if ( rq->bRequest != USBRQ_GET_DESCRIPTOR )
	return ( 0 ) ;

    switch ( rq->wValue.bytes[1] )
    {
	case USBDESCR_DEVICE :			// 1

	    if ( sw_id == SW_ID_3DP )
		RET_DESC( usbDescDevice3DP ) ;

	    if ( sw_id == SW_ID_PP )
		RET_DESC( usbDescDevicePP ) ;

	    RET_DESC( usbDescDeviceFFP ) ;

	case USBDESCR_CONFIG :			// 2

	    if ( sw_id == SW_ID_3DP )
		RET_DESC( usbDescConfiguration3DP ) ;

	    RET_DESC( usbDescConfigurationFFP ) ;

	case USBDESCR_STRING :			// 3

	    if ( rq->wValue.bytes[0] == 2 )	// Device name
	    {
        	if ( sw_id == SW_ID_3DP )
		    RET_DESC( usbDescStringDevice3DP ) ;

		if ( sw_id == SW_ID_PP )
		    RET_DESC( usbDescStringDevicePP ) ;

		RET_DESC( usbDescStringDeviceFFP ) ;
	    }
	    else
	    if ( rq->wValue.bytes[0] == 3 )	// Serial no.
		RET_DESC( usbDescSerial_R ) ;

	    break ;

	case USBDESCR_HID :			// 0x21

	    if ( sw_id == SW_ID_3DP )
		usbMsgPtr = (uchar *)usbDescConfiguration3DP + 18 ;
	    else
		usbMsgPtr = (uchar *)usbDescConfigurationFFP + 18 ;

	    return ( 9 ) ;

	case USBDESCR_HID_REPORT :		// 0x22

	    if ( sw_id == SW_ID_3DP )
		RET_DESC( usbHidReportDesc3DP ) ;

	    RET_DESC( usbHidReportDescFFP ) ;
    }

    return ( 0 ) ;
}

/* ------------------------------------------------------------------------- */

uchar usbFunctionSetup ( uchar data[8] )
{
    usbRequest_t
	*rq = (void *)data ;

    if ( (rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS )
    {						// class request type
	if ( rq->bRequest == USBRQ_HID_GET_REPORT ) // Mandatory
	{
#if COMPDEV
	    // wValue: ReportType (highbyte), ReportID (lowbyte)

	    if ( rq->wValue.bytes[0] == 1 )	// report #1, joystick
	    {
#endif
		usbMsgPtr = sw_report ;		// Return current report

		return ( sw_reportsz ) ;
#if COMPDEV
	    }
	    else
	    if ( rq->wValue.bytes[0] == 2 )	// report #2, keyboard
	    {
		usbMsgPtr = kb_report ;

		return ( sizeof( kb_report ) ) ;
	    }
#endif
	}
#if COMPDEV
	else					// wValue low byte is report id
	if ( rq->bRequest == USBRQ_HID_GET_IDLE && rq->wValue.bytes[0] != 1 )
	{					// not supported for the joystick
	    usbMsgPtr = &idleRate ;

	    return ( 1 ) ;
	}
	else
	if ( rq->bRequest == USBRQ_HID_SET_IDLE && rq->wValue.bytes[0] != 1 )
	{
	    uint8_t
		i = rq->wValue.bytes[1] ;	// wValue high byte is new idle rate

	    if ( ! i )				// Indefinite
		idleCnt = 0 ;
	    else
	    {
		if ( i < 4 )
		    i = 4 ;			// Limit to 16ms min.

		if ( ! idleRate || (idleRate - idleCnt) >= i )
		    idleCnt = 1 ;		// Was indefinite or new idle expired
		else
		    idleCnt = i - idleCnt ;	// Correct counter to new idle time
	    }

	    idleRate = i ;
	}
#endif // COMPDEV
    }
    else
    {
	// no vendor specific requests implemented
    }

    return ( 0 ) ;
}

/* ------------------------------------------------------------------------- */

static void init_serial ( void )
{
    static const prog_uint16_t
	usbDescSerial_F[] =
	{
	    USB_STRING_DESCRIPTOR_HEADER( USB_CFG_SERIAL_NUMBER_LEN ),
	    USB_CFG_SERIAL_NUMBER
	} ;

    uint8_t
	i, *s, *d ;

    s = VP( usbDescSerial_F ) ;
    d = NULL ;

    if ( eeprom_read_byte( d ) != sizeof( usbDescSerial_F ) )
	for ( i = sizeof( usbDescSerial_F ) ; i-- ; ++s, ++d )
	    eeprom_write_byte( d, pgm_read_byte( s ) ) ;

    s = NULL ;
    d = usbDescSerial_R ;

    for ( i = sizeof( usbDescSerial_F ) ; i-- ; ++s )
	*d++ = eeprom_read_byte( s ) ;
}

/* ------------------------------------------------------------------------- */

#if COMPDEV

/* ------------------------------------------------------------------------- */

static inline void SaveKbReport ( void )
{
    kb_repsav[2] = kb_report[2] ;		// Save key 1
    kb_repsav[3] = kb_report[3] ;		// Save key 2
}

/* ------------------------------------------------------------------------- */

#define	WINLOCK			1

#define	MOD_NONE		0
#define MOD_CONTROL_LEFT	_BV(0)
#define MOD_SHIFT_LEFT		_BV(1)
#define MOD_ALT_LEFT		_BV(2)
#define MOD_GUI_LEFT		_BV(3)
#define MOD_CONTROL_RIGHT	_BV(4)
#define MOD_SHIFT_RIGHT		_BV(5)
#define MOD_ALT_RIGHT		_BV(6)
#define MOD_GUI_RIGHT		_BV(7)

#define	KEY_NONE		 0
#define	KEY_D			 7
#define	KEY_L			15
#define	KEY_R			21
#define	KEY_U			24

#define	KEY_END			77
#define	KEY_PGDN		78

#define	KEY_PADm		86
#define	KEY_PADp		87
#define	KEY_PAD1		89
#define	KEY_PAD3		91

#define	_KEY_UP			KEY_PADm
#define	_KEY_DWN		KEY_PADp
#define	_KEY_RGT		KEY_PAD3
#define	_KEY_LFT		KEY_PAD1

static const prog_uint8_t
    keys[][2] =
    {
#if WINLOCK
 #undef  _KEY_DWN
 #define _KEY_DWN		KEY_L
 #define _MOD_ON		MOD_GUI_RIGHT

	{ MOD_NONE, _KEY_UP  },		// N
	{ MOD_NONE, KEY_NONE },		// NE
	{ MOD_NONE, _KEY_RGT },		// E
	{ MOD_NONE, KEY_NONE },		// SE
	{ _MOD_ON , _KEY_DWN },		// S
	{ MOD_NONE, KEY_NONE },		// SW
	{ MOD_NONE, _KEY_LFT }, 	// W
	{ MOD_NONE, KEY_NONE },		// NW
	{ MOD_NONE, KEY_NONE }		// idle
#else
	{ _KEY_UP , KEY_NONE },		// N
	{ _KEY_UP , _KEY_RGT },		// NE
	{ _KEY_RGT, KEY_NONE },		// E
	{ _KEY_RGT, _KEY_DWN },		// SE
	{ _KEY_DWN, KEY_NONE },		// S
	{ _KEY_DWN, _KEY_LFT },		// SW
	{ _KEY_LFT, KEY_NONE }, 	// W
	{ _KEY_LFT, _KEY_UP  },		// NW
	{ KEY_NONE, KEY_NONE }		// idle
#endif
    } ;

static uint8_t CrKbReport ( void )
{
    uint8_t
	pov ;

    // Extract POV info from sw_report

    if ( sw_id == SW_ID_3DP )
    {
	// 3DP report structure
	// -------0 -------1 -------2 -------3 -------4 -------5 -------6 +1
	// XXXXXXXX YYYYYYXX RRRRYYYY HHHRRRRR BBBBBBBH TTTTTTTB 00000TTT
	// 76543210 54321098 32109876 21087654 76543213 65432108      987

	pov = ((sw_report[4] >> 5) | (sw_report[5] << 3)) & 0x0F ;
    }
    else
    {
	// FFP report structure
	// -------0 -------1 -------2 -------3 -------4 -------5 +1
	// XXXXXXXX YYYYYYXX HHHHYYYY BBRRRRRR TBBBBBBB 00TTTTTT
	// 76543210 54321098 32109876 21543210 09876543   654321

	pov = sw_report[3] >> 4 ;
    }

    // Fill in report

    if ( pov > 8 )				// Limit pov value to 8
	pov = 8 ;

#if WINLOCK
    *(uint16_t *)(kb_report + 1) = pgm_read_word( keys[pov] ) ;

    return ( *(uint16_t *)(kb_report + 1) != *(uint16_t *)(kb_repsav + 1) ) ;
#else
    *(uint16_t *)(kb_report + 2) = pgm_read_word( keys[pov] ) ;

    return ( *(uint16_t *)(kb_report + 2) != *(uint16_t *)(kb_repsav + 2) ) ;
#endif
}

/* ------------------------------------------------------------------------- */

int FA_NORETURN( main ) ( void )
{
    uint8_t
	kb_sendrep ;				// Flag: need to send kb report
						// Initialize..
    init_hw() ;					// hardware. Note: defined as naked !
    odDebugInit() ;				// debug subsytem
    init_serial() ;				// Serial no.
    usbInit() ;					// USB stack

    sw_reportsz = (sw_id == SW_ID_3DP ? SW_REPSZ_3DP : SW_REPSZ_FFP) ;

    sw_report[0] = 1 ;				// Preset report IDs
    kb_report[0] =
    kb_repsav[0] = 2 ;

    kb_sendrep   = FALSE ;

    wdt_enable( WDTO_500MS ) ;			// Unleash watchdog

    sei() ;					// Enable interrupts

    usbDeviceConnect() ;			// Activate USB

    TCCR2 = T2PS_1024 ;				// Set T2 prescaler to / 1024 for delay

    for ( ;; )					// Forever..
    {
	wdt_reset() ;				// Calm watchdog

	usbPoll() ;				// Do USB stuff

	if ( usbConfiguration )
	{
	    if ( ! usbInterruptIsReady() )	// Report not sent yet
	    {
		SETTCNT( READ_DEL0 ) ;		// reset timer
		TIFR = _B1( TOVF ) ;		// clear overflow flag
	    }
	    else
	    if ( bit_is_set( TIFR, TOVF ) )	// Timed out
	    {
		TCCR2  = T2PS_64 ;		// Set T2 prescaler to / 64 for query

		getdata() ;			// Read 3DP data

		usbSetInterrupt( sw_report, sw_reportsz ) ;

		TCCR2  = T2PS_1024 ;		// reset T2 prescaler to / 1024 for delay

		if ( CrKbReport() )		// Create KB report from SW report
		    kb_sendrep = TRUE ;		// changed, need to send the report

		LED_off() ;			// may have been turned on in getdata()

		SETTCNT( READ_DEL0 ) ;		// reset timer (gcc optimizes as jump up)
		TIFR = _B1( TOVF ) ;		// clear overflow flag
	    }
	    else
	    if ( kb_sendrep )			// Need to send kb report
	    {
		usbSetInterrupt( kb_report, sizeof( kb_report ) ) ;

		SaveKbReport() ;		// Save current kb report

		kb_sendrep = FALSE ;		// Clear send kb report flag

		SETITCNT( T6DEL4MS ) ;		// reset idle timer
		TIFR = _B1( ITOVF ) ;		// clear overflow flag

		idleCnt = idleRate ;		// reset idle counter

		SETTCNT( READ_DEL0 ) ;		// reset timer (gcc optimizes as jump up)
		TIFR = _B1( TOVF ) ;		// clear overflow flag
	    }

	    if ( bit_is_set( TIFR, ITOVF ) )	// 4ms timed out
	    {
		SETITCNT( T6DEL4MS ) ;		// reset idle timer
		TIFR = _B1( ITOVF ) ;		// clear overflow flag

		if ( ! --idleCnt )		// Idle counter expired
		{
		    idleCnt = idleRate ;	// reset idle counter

		    if ( idleRate )		// If not indefinite,
			kb_sendrep = TRUE ;	// need to send a report
		}
	    }
	}
	else
	{
	    SETITCNT( T6DEL4MS ) ;		// start idle timer
	    SETTCNT( 0 ) ;			// start SW timer, ~17ms @ 15MHz
	    TIFR = _B1( ITOVF ) | _B1( TOVF ) ;	// clear overflow flags
	}
    } // for
}

/* ------------------------------------------------------------------------- */

#else // COMPDEV

/* ------------------------------------------------------------------------- */

int FA_NORETURN( main ) ( void )
{
						// Initialize..
    init_hw() ;					// hardware. Note: defined as naked !
    odDebugInit() ;				// debug subsytem

    init_serial() ;				// Setup serial no. descriptor

    usbInit() ;					// USB stack

    sw_reportsz = (sw_id == SW_ID_3DP ? SW_REPSZ_3DP : SW_REPSZ_FFP) ;

    wdt_enable( WDTO_500MS ) ;			// Unleash watchdog

    sei() ;					// Enable interrupts

    usbDeviceConnect() ;			// Activate USB

    for ( ;; )					// Forever..
    {
	wdt_reset() ;				// Calm watchdog

	usbPoll() ;				// Do USB stuff

	if ( usbConfiguration )			// USB is enumerated
	{
	    if ( ! usbInterruptIsReady() )	// Report not sent yet
	    {
		SETTCNT( READ_DEL ) ;		// reset timer
		TIFR = _B1( TOVF ) ;		// clear overflow flag
	    }
	    else				// Report sent
	    if ( bit_is_set( TIFR, TOVF ) )	// Timed out
	    {
		getdata() ;			// Read 3DP data
						// Prepare to send report
		usbSetInterrupt( sw_report, sw_reportsz ) ;

		LED_off() ;			// may have been turned on in getdata()
	    }
	}
	else
	{
	    SETTCNT( 0 ) ;			// preset timer, ~21ms @ 12MHz
	    TIFR = _B1( TOVF ) ;		// clear overflow flag
	}
    }
}

/* ------------------------------------------------------------------------- */

#endif // COMPDEV

/* ------------------------------------------------------------------------- */
