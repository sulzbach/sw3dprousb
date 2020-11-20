/*******************************************************************************
 * File Name	: 3DPro.h
 * Project	: 3DP-Vert, Microsoft Sidewinder 3D Pro/PP/FFP to USB converter
 * Date		: 2005/05/31, 2006/12/14, 2008/02/12
 * Version	: 3.0
 * Target MCU	: ATmega8/ATTiny461/ATtiny861
 * Tool Chain	: Atmel AVR Studio 4.16 628 / WinAVR 20081205
 * Author	: Detlef "Grendel" Mueller
 *		  detlef@gmail.com
 * Release Notes:
 *
 * $Id: 3DPro.h 1.19 2009/03/07 08:19:32 Detlef Exp Detlef $
 *
 * $Log: 3DPro.h $
 * Revision 1.19  2009/03/07 08:19:32  Detlef
 * Added experimental SIXAXIS device. Added support for WinAVR 20081205.
 *
 * Revision 1.18  2009/02/02 01:27:16  Detlef
 * Added support for an experimental composite device (keyboard/joystick).
 * Activated via the COMPDEV define.
 *
 * Revision 1.17  2008/05/18 06:59:15  Detlef
 * Corrected READ_DEL for latest AVR-USB
 *
 * Revision 1.16  2008/04/28 08:13:18  Detlef
 * NO_555 now works w/ mega8 code (modified rev1 design)
 *
 * Revision 1.15  2008/03/03 10:12:01  Detlef
 * Added NO_555 define. The CPU simulates the 555 timers if active.
 * For rev2 tiny design only.
 *
 * Revision 1.14  2008/02/25 09:19:30  Detlef
 * Added xxx_bit() macros
 *
 * Revision 1.13  2008/02/20 03:27:49  Detlef
 * Cleanup & documentation.
 *
 * Revision 1.12  2008/02/18 10:02:15  Detlef
 * Version 3. Added experimental tiny461/861 support for new hardware.
 *
 ******************************************************************************/

#ifndef	_3DPRO_H_
#define	_3DPRO_H_

//-------------------------------------------------------------------------------

#include "f_cpu.h"
#include "includes.h"

//-------------------------------------------------------------------------------

#if defined(__AVR_ATmega8__)

 #define	NO_556		0	/* Modified rev1 design: 0, 42, 59	*/
 #define	COMPDEV		0	/* Experimental composite device	*/
 #define	SIXAXIS		0	/* Experimental joystick as POV:59	*/
 #define	HID_PHYS	0	/* Physical min/max in HID report	*/

 #if ! NO_556 && (COMPDEV || SIXAXIS)
  #error "COMPDEV or SIXAXIS only work w/ NO_556 active"
 #endif

 #if SIXAXIS
  #if COMPDEV
   #error "COMPDEV and SIXAXIS can't be active at the same time"
  #endif
  #if F_CPU != 12000000
   #error "SIXAXIS needs a 12MHz crystal (iADCSRA)"
  #endif
 #endif

#elif defined(__AVR_ATtinyX61__)

 #define	NO_556		1	/* Must be 1 for rev2 design		*/
 #define	COMPDEV		0	/* Must not be 1 for rev2 design	*/
 #define	SIXAXIS		0	/* Must not be 1 for rev2 design	*/
 #define	HID_PHYS	0	/* Physical min/max in HID report	*/

#endif

#if COMPDEV > 1
 #error	"COMPDEV must be defined as 0 or 1"
#endif

#if SIXAXIS > 1
 #error	"SIXAXIS must be defined as 0 or 1"
#endif

#if NO_556 > 1
 #error	"NO_556 must be defined as 0 or 1"
#endif

//-------------------------------------------------------------------------------
// 3DP Packet sizes

#define	DATSZ3DP	22	/* 3DP data packet size in clock ticks (x3 bit, sent 3x) */
#define	IDSZ3DP		160	/* 3DP ID packet size in clock ticks */

//-------------------------------------------------------------------------------
// FFP/PP Packet sizes

#define	DATSZFFP	16	/* FFP/PP data packet size in clock ticks (x3 bit) */
#define	IDSZPP		40	/* PP ID packet size */
#define	IDSZFFP		14	/* FFP ID packet size */

//-------------------------------------------------------------------------------
// Hardware specific definitions

#if defined(__AVR_ATmega8__)

//-------------------------------------------------------------------------------

#define	GIER		GICR

#define	ACSCR		ACSR

#define	LEDPORT		PORTD
#define	LEDBIT		PD1

#define	TRGPORT		PORTD
#define	TRGBIT		PD0

#define	EXPPIN		PINC
#define	EXPBIT		PC0
#define	EXPBIT2		PC1

#if NO_556

 #define TRGDDR		DDRC

 #define TRGY2BIT	DDC0
 #define TRGX1BIT	DDC1

#endif

#define	BUTPIN		PIND
#define	BUT1		PD4
#define	BUTMSK		(_B1(PD7) | _B1(PD6) | _B1(PD5) | _B1(BUT1))

//-------------------------------------------------------------------------------
// Timer 0/1/2 prescaler settings

#define T1PS_64		(_B0(CS12)+_B1(CS11)+_B1(CS10)) /* TCCR1B, /64 */
#define T1PS_1024	(_B1(CS12)+_B0(CS11)+_B1(CS10)) /* TCCR1B, /1024 */

#define T2PS_64         (_B1(CS22)+_B0(CS21)+_B0(CS20)) /* TCCR2, /64 */
#define T2PS_1024       (_B1(CS22)+_B1(CS21)+_B1(CS20)) /* TCCR2, /1024 */

//-------------------------------------------------------------------------------

#elif defined(__AVR_ATtinyX61__)

//-------------------------------------------------------------------------------

#define	GIER		GIMSK

#define	ACSCR		ACSRA

#define	LEDPORT		PORTA
#define	LEDBIT		PA4

#define	TRGPORT		PORTA
#define	TRGBIT		PA1

#define	EXPPIN		PINA
#define	EXPBIT		PA0

#if NO_556

 #define TRGDDR		DDRA

 #define TRGY2BIT	DDA0
 #define TRGX1BIT	DDA1

#endif

#define	BUTPIN		PINA
#define	BUT1		PA2
#define	BUTMSK		(_B1(PA7) | _B1(PA6) | _B1(PA5) | _B1(BUT1))

//-------------------------------------------------------------------------------
// Timer 0/1 prescaler settings

#define	T0PS_64		(_B0(CS02) | _B1(CS01) | _B1(CS00) | _B1(PSR0)) /* TCCR0B, /64 */
#define	T0PS_1024	(_B1(CS02) | _B0(CS01) | _B1(CS00) | _B1(PSR0)) /* TCCR0B, /1024 */

// TCCR1B, /64 -- have to do w/o, can't get the *&$#@! timer to work :(
#define	T1PS_64		(_B0(CS13) | _B1(CS12) | _B1(CS11) | _B1(CS10) | _B1(PSR1))

//-------------------------------------------------------------------------------

#endif	// tiny461 || tiny861

//-------------------------------------------------------------------------------

#if ! defined(__ASSEMBLER__)

//-------------------------------------------------------------------------------
// C only definitions
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Timer Macros, msec or usec to timer ticks. ps = prescaler value

#define MS2TM( ms, ps ) -(int16_t)(((ms) * (F_CPU /    1000.)) / (ps) + .5)
#define US2TM( us, ps ) -(int16_t)(((us) * (F_CPU / 1000000.)) / (ps) + .5)

//-------------------------------------------------------------------------------
// Timer delay values for /1024 prescaler

#define	T0DEL500US	MS2TM(  .5, 1024 )
#define	T0DEL4MS	MS2TM(   4, 1024 )
#define	T0DEL8MS	MS2TM(   8, 1024 )
#define	T0DEL200MS	MS2TM( 200, 1024 )
#define	T0DEL300MS	MS2TM( 300, 1024 )
#define	T0DEL400MS	MS2TM( 400, 1024 )
#define	T0DEL500MS	MS2TM( 500, 1024 )

//-------------------------------------------------------------------------------
// Timer delay values for /64 prescaler

#define	T6DEL140US	US2TM( 140,  64 )
#define	T6DEL440US	US2TM( 440,  64 )
#define	T6DEL865US	US2TM( 865,  64 )

#define	T6DEL4MS	MS2TM(   4,  64 )

//-------------------------------------------------------------------------------
// main.c, delay until reading stick after last report was sent

#define	READ_DEL	MS2TM( 7.1, 64 )
#define	READ_DEL0	MS2TM( 7.1, 1024 )

//-------------------------------------------------------------------------------

#define	SW_REPSZ_3DP	(7+COMPDEV+SIXAXIS)	/* report size for 3DP */
#define	SW_REPSZ_FFP	(6+COMPDEV)		/* report size for PP/FFP */

#define	SW_ID_3DP	1			/* 3DP connected */
#define	SW_ID_PP	2			/*  PP connected */
#define	SW_ID_FFP	3			/* FFP connected */

//-------------------------------------------------------------------------------
// Inline code

#define	dis3DP_INT()	clr_bit( GIER, INT1 )	/* Disable INT1 */

#if NO_556

 #define TRG_pull()	__WRAP__( { clr_bit( TRGDDR, TRGX1BIT ) ; clr_bit( TRGDDR, TRGY2BIT ) ; } )
 #define TRG_rel()	__WRAP__( { set_bit( TRGDDR, TRGX1BIT ) ; set_bit( TRGDDR, TRGY2BIT ) ; } )

#else

 #define TRG_pull()	clr_bit( TRGPORT, TRGBIT )
 #define TRG_rel()	set_bit( TRGPORT, TRGBIT )

#endif

#define	LED_on()	clr_bit( LEDPORT, LEDBIT )
#define	LED_off()	set_bit( LEDPORT, LEDBIT )

//-------------------------------------------------------------------------------
// 3DPro.c interface

extern uint8_t
    sw_id,				// Will be SW_ID_...
    sw_report[7+COMPDEV+SIXAXIS] ;	// Report buffer

extern void
    init_hw( void ),			// Initialize HW & wait for stick
    getdata( void ) ;			// Read stick and set up sw_report

//-------------------------------------------------------------------------------
// 3DProasm.S interface

extern uint8_t				// Sidewinder 3D Pro packet buffers
    sw_idbuf[60],
    sw_packet3[8],
    sw_packet2[8],
    sw_packet1[8] ;

extern uint8_t				// Sidewinder PP/FFP packet buffer
    ffp_packet[6] ;

extern volatile uint8_t
    sw_pktptr,				// LSB of -> to current byte in buffer
    sw_clkcnt ;				// no. of clock ticks on B1

extern void
    CopyFFPData( uint8_t *packet ),	// Copy PP/FFP data from packet to sw_report
    Copy3DPData( uint8_t *packet ) ;	// Copy 3DP data from packet to sw_report

extern uint8_t
    QueryFFP( uint8_t id, uint8_t sz ),	// Read data from PP/FFP
    Query3DP( uint8_t id, uint8_t sz ) ;// Read data from 3DPro

//-------------------------------------------------------------------------------

#else // __ASSEMBLER__

//-------------------------------------------------------------------------------
// Assembler only definitions

#if F_CPU == 12000000

// Receiver timeout

#define	T1TO48US	247		/* ( 48*12/64) =  9 (0- 9 = 247) */
#define	T1TO100US	237		/* (100*12/64) = 19 (0-19 = 237) */

// Initial rec. timeouts

#define	T1TO400US	181		/* (400*12/64) = 75 (0-75 = 181) */
#define	T1TO150US	228		/* (150*12/64) = 28.125 (0-28 = 228) */

#elif F_CPU == 15000000

// Receiver timeout

#define	T1TO48US	245		/* ( 48*15/64) = 11.25 (0-11 = 245) */
#define	T1TO100US	233		/* (100*15/64) = 23.4375 (0-23 = 233) */

// Initial rec. timeouts

#define	T1TO400US	162		/* (400*15/64) = 93.75 (0-94 = 162) */
#define	T1TO150US	221		/* (150*15/64) = 35.15625 (0-35 = 221) */

#elif F_CPU == 16000000

// Receiver timeout

#define	T1TO48US	244		/* ( 48*16/64) = 12 (0-12 = 244) */
#define	T1TO100US	231		/* (100*16/64) = 25 (0-25 = 231) */

// Initial rec. timeouts

#define	T1TO400US	156		/* (400*16/64) = 100 (0-100 = 156) */
#define	T1TO150US	218		/* (150*16/64) = 37.5 (0-38 = 218) */

#else	// F_CPU undefined or out of range
 #error	"F_CPU must be 12000000, 15000000, or 16000000"
#endif	// F_CPU

//-------------------------------------------------------------------------------

#endif	// __ASSEMBLER__

//-------------------------------------------------------------------------------

#endif	// _3DPRO_H_
