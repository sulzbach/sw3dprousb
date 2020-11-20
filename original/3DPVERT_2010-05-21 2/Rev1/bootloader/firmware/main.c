/* Name: main.c
 * Project: AVR bootloader HID
 * Author: Christian Starkjohann
 * Creation Date: 2007-03-19
 * Tabsize: 4
 * Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt)
 * Based on: main.c 281 2007-03-20 13:22:10Z cs
 * This Revision: $Id: main.c 1.2 2007/07/05 10:48:12 Detlef Exp Detlef $
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <string.h>

#include "bootloaderconfig.h"
#include "usbdrv.h"
#include "oddebug.h"

#include "usbdrv/usbdrv.c"

static int8_t
    reportId = -1 ;

static uint32_t
    currentAddress ;			// in bytes

static uint8_t
    offset ;				// data already processed in current transfer

#if	BOOTLOADER_CAN_EXIT
static uint8_t
    exitMainloop ;
#endif

PROGMEM char
    usbHidReportDescriptor[33] =
    {
	0x06, 0x00, 0xff,		// USAGE_PAGE (Generic Desktop)
	0x09, 0x01,			// USAGE (Vendor Usage 1)
	0xa1, 0x01,			// COLLECTION (Application)
	0x15, 0x00,			//   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00,		//   LOGICAL_MAXIMUM (255)
	0x75, 0x08,			//   REPORT_SIZE (8)

	0x85, 0x01,			//   REPORT_ID (1)
	0x95, 0x06,			//   REPORT_COUNT (6)
	0x09, 0x00,			//   USAGE (Undefined)
	0xb2, 0x02, 0x01,		//   FEATURE (Data,Var,Abs,Buf)

	0x85, 0x02,			//   REPORT_ID (2)
	0x95, 0x83,			//   REPORT_COUNT (131)
	0x09, 0x00,			//   USAGE (Undefined)
	0xb2, 0x02, 0x01,		//   FEATURE (Data,Var,Abs,Buf)

	0xc0				// END_COLLECTION
    } ;

// allow compatibility with avrusbboot's bootloaderconfig.h:

#ifdef BOOTLOADER_INIT
#   define bootLoaderInit()         BOOTLOADER_INIT
#endif
#ifdef BOOTLOADER_CONDITION
#   define bootLoaderCondition()    BOOTLOADER_CONDITION
#endif

static void (*nullVector)( void ) __attribute__((__noreturn__)) ;


uchar usbFunctionSetup ( uchar data[8] )
{
    static uchar
	replyBuffer[] =
	{
	    1,				// report ID
	    SPM_PAGESIZE & 0xFF,
	    SPM_PAGESIZE >> 8,
	    ((long)FLASHEND  + 1) & 0xFF,
	    ((long)(FLASHEND + 1) >>  8) & 0xFF,
	    ((long)(FLASHEND + 1) >> 16) & 0xFF,
	    ((long)(FLASHEND + 1) >> 24) & 0xFF
	} ;

    usbRequest_t
	*rq = (void *)data ;

    if ( rq->bRequest == USBRQ_HID_SET_REPORT )
    {
	reportId = rq->wValue.bytes[0] ; // store report ID
	offset = 0 ;

        return ( 0xFF ) ;
    }

    if ( rq->bRequest == USBRQ_HID_GET_REPORT )
    {
        usbMsgPtr = replyBuffer ;

        return ( sizeof( replyBuffer ) ) ;
    }

    return ( 0 ) ;
}

uchar usbFunctionWrite ( uchar *data, uchar len )
{
    union
    {
	uint32_t l ;
	uint16_t s[2] ;
	uint8_t  c[4] ;
    }
    address ;

#if	BOOTLOADER_CAN_EXIT
    if ( reportId == 1 )		// leave boot loader
    {
	exitMainloop = 1 ;

	return ( 1 ) ;
    }
#endif

    if ( reportId == 2 )		// write page
    {
	LED_ON() ;

	if ( offset == 0 )
	{
	    data++ ;

	    DBG1( 0x30, data, 4 ) ;

	    address.c[0] = *data++ ;
	    address.c[1] = *data++ ;
	    address.c[2] = *data++ ;
	    address.c[3] = 0 ;
	    len -= 4 ;
	}
	else
	{
	    DBG1( 0x31, (void *)&currentAddress, 4 ) ;

	    address.l = currentAddress ;
        }

	offset += len ;
	len >>= 1 ;

	do
	{
	    DBG1( 0x32, 0, 0 ) ;

	    if ( (address.s[0] & (SPM_PAGESIZE - 1)) == 0 )
	    {			   	// if page start: erase
		DBG1( 0x33, 0, 0 ) ;
#ifndef TEST_MODE
		cli() ;
		boot_page_erase( address.l ) ;// erase page
		sei();

		boot_spm_busy_wait() ;	// wait until page is erased
#endif
	    }

	    cli() ;
	    boot_page_fill( address.l, *(short *)data ) ;
	    sei() ;

	    address.l += 2 ;
	    data += 2 ;

	    // write page when we cross page boundary

	    if ( (address.s[0] & (SPM_PAGESIZE - 1)) == 0 )
	    {
		DBG1( 0x34, 0, 0 ) ;
#ifndef TEST_MODE
		cli() ;
		boot_page_write( address.l - 2 ) ;
		sei() ;

		boot_spm_busy_wait() ;
#endif
	    }
        }
	while ( --len != 0 ) ;

	currentAddress = address.l ;

	DBG1( 0x35, (void *)&currentAddress, 4 ) ;

	LED_OFF() ;

	if ( offset < 128 )
	    return ( 0 ) ;

	reportId = -1 ;
    }

    return ( 1 ) ;
}

#define MS2T1( ms, ps ) -(int)(((ms) * (F_CPU / 1000.)) / (ps) + .5)

#define T1DEL20MS	MS2T1(  20, 1024 )
#define T1DEL100MS	MS2T1( 100, 1024 )
#define T1DEL200MS	MS2T1( 200, 1024 )
#define T1DEL500MS	MS2T1( 500, 1024 )

void __attribute__((__noinline__)) delay_ms ( uint16_t time )
{
    SFIOR  = (1 << PSR10) ;             // reset prescaler
    TCNT1  = time ;                     // set counter
    TIFR   = (1 << TOV1) ;              // clear overflow flag

    for ( ; bit_is_clear( TIFR, TOV1 ) ; ) // wait for ovf
	;
}

int main ( void )
{
    uint8_t
	i ;

    // initialize hardware

    bootLoaderInit() ;

    odDebugInit() ;

    DBG1( 0x00, 0, 0 ) ;

    // jump to application if condition is not met

    delay_ms( T1DEL200MS ) ;		// wait for stick to boot

    if ( bootLoaderCondition() )
    {
	for ( i = 10 ; i-- ; )		// Signal bootloader mode
	{
	    LED_ON() ;
	    delay_ms( T1DEL100MS ) ;
	    LED_OFF() ;
	    delay_ms( T1DEL100MS ) ;
	}

	GICR = (1 << IVCE) ;		// enable change of interrupt vectors
	GICR = (1 << IVSEL) ;		// move interrupts to boot flash section

	usbInit() ;
	sei() ;
	usbDeviceConnect() ;

	for ( ;; )		 	// main event loop
	{
	    usbPoll() ;

#if	BOOTLOADER_CAN_EXIT
	    if ( exitMainloop )
	    {
		for ( i = 0 ; --i ; )
		    usbPoll() ;		// try to send the response

		break ;
	    }
#endif
	}
    }

    DBG1(0x01, 0, 0) ;

    cli() ;

    DDRB  = DDB ;			// Tri-state ID resistor, D-, D+
    PORTB = PBPU ;

    boot_rww_enable() ;

    GICR = (1 << IVCE) ;		// enable change of interrupt vectors
    GICR = (0 << IVSEL) ;		// move interrupts to application flash section

    // We must go through a global function pointer variable instead of
    // writing ((void (*)(void))0)() ; because the compiler optimizes a
    // constant 0 to "rcall 0" which is not handled correctly by the
    // assembler.

    nullVector() ;

    return ( 0 ) ;
}
