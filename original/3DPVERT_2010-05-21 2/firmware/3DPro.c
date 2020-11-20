/******************************************************************************
 * File Name	: 3DPro.c
 * Project	: 3DP-Vert, Microsoft Sidewinder 3D Pro/PP/FFP to USB converter
 * Date		: 2005/05/31, 2006/12/14, 2008/02/12
 * Version	: 3.0
 * Target MCU	: ATmega8/ATTiny461/ATtiny861
 * Tool Chain	: Atmel AVR Studio 4.16 628 / WinAVR 20081205
 * Author	: Detlef "Grendel" Mueller
 *		  detlef@gmail.com
 * References	: Sidewinder.c by Vojtech Pavlik
 *		  Patent #5628686, Apperatus and Method for Bidirectional
 *			Data Communication in a Gameport, by Microsoft
 * Description	: 3D Pro driver and hardware interface
 * Device	: Microsoft SideWinder 3D Pro, Precision Pro, Force Feedback Pro
 *
 * Release Notes:
 *
 *	Pinout ATmega
 *	=============
 *
 *	PB0	USB D-		PC0	X1 axis (exp.)	PD0	(556 trigger)
 *	PB1	USB D+		PC1	Y2 axis (exp.)	PD1	LED
 *	PB2	USB IdR		PC2	(Rx)		PD2	USB D+
 *	PB3	(MOSI)		PC3	(Ry)		PD3	Button 1
 *	PB4	(MISO)		PC4	NC		PD4	Button 1
 *	PB5	(SCK)		PC5	NC		PD5	Button 2
 *	PB6	(XTal)		PC6     (RST)		PD6	Button 3
 *	PB7	(XTal)					PD7	Button 4
 *
 *	Pinout ATtiny
 *	=============
 *
 *	PB0	(MOSI)		PA0	Y2 axis (Axis exp.)
 *	PB1	(MISO)		PA1	X1 axis (556 trigger)
 *	PB2	(SCK)		PA2	Button 1
 *	PB3	USB D-		PA3	USB IdR
 *	PB4	(XTal)		PA4	LED
 *	PB5	(XTal)		PA5	Button 2
 *	PB6	USB D+		PA6	Button 3
 *	PB7	(RST)		PA7	Button 4
 *
 * $Id: 3DPro.c 1.22 2009/03/07 08:19:32 Detlef Exp Detlef $
 *
 * $Log: 3DPro.c $
 * Revision 1.22  2009/03/07 08:19:32  Detlef
 * Added experimental SIXAXIS device. Added support for WinAVR 20081205.
 *
 * Revision 1.21  2009/02/02 01:26:11  Detlef
 * Added support for an experimental composite device (keyboard/joystick).
 *
 * Revision 1.20  2008/12/03 07:26:26  Detlef
 * Removed N flag check from Check3DPPkt() to speed things up, added
 * cast to T6DEL865US to shut up compiler.
 *
 * Revision 1.19  2008/04/28 08:13:18  Detlef
 * NO_555 now works w/ mega8 code (modified rev1 design)
 *
 * Revision 1.18  2008/04/05 09:58:14  Detlef
 * Finetuned init_hw() for mega8, removed 16/48 cases.
 *
 * Revision 1.17  2008/03/05 04:02:35  Detlef
 * Optimized Check3DPPkt()
 *
 * Revision 1.16  2008/03/03 10:12:01  Detlef
 * Added NO_555 define. The CPU simulates the 555 timers if active.
 * For rev2 tiny design only.
 *
 * Revision 1.15  2008/02/25 09:18:46  Detlef
 * Cleanup & squeezing the code.
 *
 * Revision 1.14  2008/02/20 03:33:09  Detlef
 * Cleanup. Tiny uses T0 for all timings ATM, T1 proves troublesome.
 *
 * Revision 1.13  2008/02/18 10:02:15  Detlef
 * Version 3. Added experimental tiny461/861 support for new hardware.
 *
 ******************************************************************************/

#include "3DPro.h"

//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------

#if defined(__AVR_ATmega8__)

//------------------------------------------------------------------------------

#if SIXAXIS
			// Idle sleep enable, rising edges on INT 0/1
 #define MCUCRMSK	(_B1(   SE) | _B0(  SM2) | _B0(  SM1) | _B0(  SM0) | \
			 _B1(ISC11) | _B1(ISC10) | _B1(ISC01) | _B1(ISC00))
 			// AVcc w/ cap on AREF, left adjust result, select channel 2
 #define iADMUX		(_B0(REFS1) | _B1(REFS0) | _B1(ADLAR) | \
			 _B0(MUX3 ) | _B0(MUX2 ) | _B1(MUX1 ) | _B0(MUX0))
			// Enable ADC, disable free running, clr IF,
			// enable IRQ, set prescaler to 64 --> 187500Hz @ 12MHz
 #define iADCSRA	(_B1(ADEN ) | _B0(ADSC ) | _B0(ADFR ) | _B1(ADIF ) | \
			 _B1(ADIE ) | _B1(ADPS2) | _B1(ADPS1) | _B0(ADPS0))
#else
			// Rising edges on INT 0/1
 #define MCUCRMSK	(_B0(   SE) | _B0(  SM2) | _B0(  SM1) | _B0(  SM0) | \
			 _B1(ISC11) | _B1(ISC10) | _B1(ISC01) | _B1(ISC00))
#endif

			// TCCR0, clock via T0 (B1) on rising edge
#define T0MODE		(_B1(CS02) | _B1(CS01) | _B1(CS00))

#define PBPU		(_B0( PB7) | _B0( PB6) | _B1( PB5) | _B1( PB4) | \
			 _B1( PB3) | _B0( PB2) | _B0( PB1) | _B0( PB0))
#define DDB		(_B0(DDB7) | _B0(DDB6) | _B0(DDB5) | _B0(DDB4) | \
			 _B0(DDB3) | _B0(DDB2) | _B0(DDB1) | _B0(DDB0))

#if NO_556

 #define PCPU		(            _B1( PC6) | _B1( PC5) | _B1( PC4) | \
			 _B1( PC3) | _B1( PC2) | _B0( PC1) | _B0( PC0))
 #define DDC		(            _B0(DDC6) | _B0(DDC5) | _B0(DDC4) | \
			 _B0(DDC3) | _B0(DDC2) | _B1(DDC1) | _B1(DDC0))
#else

 #define PCPU		(            _B1( PC6) | _B1( PC5) | _B1( PC4) | \
			 _B1( PC3) | _B1( PC2) | _B1( PC1) | _B1( PC0))
 #define DDC		(            _B0(DDC6) | _B0(DDC5) | _B0(DDC4) | \
			 _B0(DDC3) | _B0(DDC2) | _B0(DDC1) | _B0(DDC0))
#endif

#define PDPU		(_B1( PD7) | _B1( PD6) | _B1( PD5) | _B1( PD4) | \
			 _B0( PD3) | _B0( PD2) | _B1( PD1) | _B1( PD0))
#define DDD		(_B0(DDD7) | _B0(DDD6) | _B0(DDD5) | _B0(DDD4) | \
			 _B0(DDD3) | _B0(DDD2) | _B1(DDD1) | _B1(DDD0))

//------------------------------------------------------------------------------

#elif defined(__AVR_ATtinyX61__)

//------------------------------------------------------------------------------

			// Rising edges on INT 0/1
#define	MCUCRMSK	(_B0(  PUD) | _B0(   SE) | \
			 _B0(  SM1) | _B0(  SM0) | \
			 _B1(ISC01) | _B1(ISC00))

#define	PBPU		(_B1( PB7) | _B0( PB6) | _B0( PB5) | _B0( PB4) | \
			 _B0( PB3) | _B1( PB2) | _B1( PB1) | _B1( PB0))
#define	DDB		(_B0(DDB7) | _B0(DDB6) | _B0(DDB5) | _B0(DDB4) | \
			 _B0(DDB3) | _B0(DDB2) | _B0(DDB1) | _B0(DDB0))

#if NO_556

 #define PAPU		(_B1( PA7) | _B1( PA6) | _B1( PA5) | _B1( PA4) | \
			 _B0( PA3) | _B1( PA2) | _B0( PA1) | _B0( PA0))
 #define DDA		(_B0(DDA7) | _B0(DDA6) | _B0(DDA5) | _B1(DDA4) | \
			 _B0(DDA3) | _B0(DDA2) | _B1(DDA1) | _B1(DDA0))
#else

 #define PAPU		(_B1( PA7) | _B1( PA6) | _B1( PA5) | _B1( PA4) | \
			 _B0( PA3) | _B1( PA2) | _B1( PA1) | _B1( PA0))
 #define DDA		(_B0(DDA7) | _B0(DDA6) | _B0(DDA5) | _B1(DDA4) | \
			 _B0(DDA3) | _B0(DDA2) | _B1(DDA1) | _B0(DDA0))
#endif

//------------------------------------------------------------------------------

#endif	// tiny461 || tiny861

//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------

uint8_t
    sw_id,					// ID of detected stick
    sw_report[7+COMPDEV+SIXAXIS] ;		// USB report data

static uint8_t
    sw_buttons,					// button buffer
    sw_problem ;				// problem counter

#if SIXAXIS

static uint16_t
    AcntrX,					// Analog center X
    AcntrY ;					// Analog center Y

#endif

//------------------------------------------------------------------------------
//******************************************************************************
//------------------------------------------------------------------------------

// Delay using a timer w/ prescaler /1024 as timing reference

static void FA_NOINLINE( Delay_1024 ) ( uint16_t time )
{
#if defined(__AVR_ATmega8__)

    SFIOR  = _B1( PSR10 ) ;			// reset prescaler
    TCNT1  = time ;				// set counter
    TIFR   = _B1( TOV1 ) ;			// clear overflow flag

    for ( ; bit_is_clear( TIFR, TOV1 ) ; )	// wait for ovf
	;

#elif defined(__AVR_ATtinyX61__)

    TCCR0B = T0PS_1024 ;			// reset prescaler
    TCNT0H = (time >> 8) ;			// set counter
    TCNT0L = time & 0xFF ;
    TIFR   = _B1( TOV0 ) ;			// clear overflow flag

    for ( ; bit_is_clear( TIFR, TOV0 ) ; )	// wait for ovf
	;
#endif
}

//------------------------------------------------------------------------------

// Delay using a timer w/ prescaler /64 as timing reference

static void FA_NOINLINE( Delay_64 ) ( uint8_t time )
{
#if defined(__AVR_ATmega8__)

    SFIOR = _B1( PSR2 ) ;			// reset prescaler
    TCNT2 = time ;				// set counter
    TIFR  = _B1( TOV2 ) ;			// clear overflow flag

    for ( ; bit_is_clear( TIFR, TOV2 ) ; )	// wait for ovf
	;

#elif defined(__AVR_ATtinyX61__)

//    TCCR1B = T1PS_64 ;			// reset prescaler
//    TCNT1  = time ;				// set counter
//    TIFR   = _B1( TOV1 ) ;			// clear overflow flag
//
//    for ( ; bit_is_clear( TIFR, TOV1 ) ; )	// wait for ovf
//	;

    TCCR0B = T0PS_64 ;				// reset prescaler
    TCNT0H = 0xFF ;				// set counter
    TCNT0L = time ;
    TIFR   = _B1( TOV0 ) ;			// clear overflow flag

    for ( ; bit_is_clear( TIFR, TOV0 ) ; )	// wait for ovf
	;

#endif
}

//------------------------------------------------------------------------------

// Trigger the 556 timer and wait until it expires or a 4ms timeout hits

#if NO_556

static void FA_NOINLINE( Trigger ) ( void )
{
    // Doing w/o 556, expires immediately

    TRG_pull() ;
    _delay_us( 47.8125 ) ;			// 255 loops @ 16MHz
    TRG_rel() ;
}

#else // NO_556

static uint8_t FA_NOINLINE( Trigger ) ( void )
{
#if defined(__AVR_ATmega8__)

    SFIOR  = _B1( PSR10 ) ;			// reset prescaler
    TCNT1  = T0DEL4MS ;				// set counter for timeout (4ms)
    TIFR   = _B1( TOV1 ) ;			// clear overflow flag

    TRG_pull() ;
    NOP() ;
    TRG_rel() ;

    for ( ; bit_is_clear( TIFR, TOV1 ) ; )	// While waiting for ovfl
	if ( bit_is_clear( EXPPIN, EXPBIT ) )	// check 556
	    return ( 1 ) ;			// signal expired

    return ( 0 ) ;				// signal timeout

#elif defined(__AVR_ATtinyX61__)

    TCCR0B = T0PS_1024 ;			// reset prescaler
    TCNT0H = (T0DEL4MS >> 8) ;			// set counter for timeout (4ms)
    TCNT0L = T0DEL4MS & 0xFF ;
    TIFR   = _B1( TOV0 ) ;			// clear overflow flag

    TRG_pull() ;
    NOP() ;
    TRG_rel() ;

    for ( ; bit_is_clear( TIFR, TOV0 ) ; )	// While waiting for ovfl
	if ( bit_is_clear( EXPPIN, EXPBIT ) )	// check 556
	    return ( 1 ) ;			// signal expired

    return ( 0 ) ;				// signal timeout
#endif
}

#endif // NO_556

//------------------------------------------------------------------------------

// Check if the FFP/PP data packet at *pkt is valid by xoring all bits together,
// result must be 1 for the packet to have correct parity.

static uint8_t FA_NOINLINE( CheckFFPPkt ) ( uint8_t *pkt )
{
    uint8_t
	x ;

    x  = pkt[0] ^ pkt[1] ^ pkt[2] ^ pkt[3] ^ pkt[4] ^ pkt[5] ;

    x ^= x >> 4 ;
    x ^= x >> 2 ;
    x ^= x >> 1 ;

    return ( x & 1 ) ;
}

//------------------------------------------------------------------------------

// Check if the 3DPro data packet at *pkt is valid by adding all nibbles
// (result must be 0) and checking the N bits of each byte.

static uint8_t FA_NOINLINE( Check3DPPkt ) ( uint8_t *pkt )
{
    uint8_t
	b,
	i   = 8,				// 8 bytes
	sum = 0 ;				// clear CHKSUM

    do						// for every byte
    {
	b = *pkt++ ;

//	To speed things up let's drop the N flag test. CHKSUM should be enough.
//
//	if ( b & 0x80 )				// N flag set ?
//	    if ( i != 1 )			// Only for the last byte
//		return ( 0 ) ;			// else bad packet

	sum += b ;				// add LSN

	__asm__ __volatile__
	(
	    "swap %0" : "+r" (b) : "0" (b)
	) ;

	sum += b ;				// add MSN
    }
    while ( --i ) ;

    return ( ! (sum & 0x0F) ) ;			// sum of all nibbs == 0, good
}

//------------------------------------------------------------------------------

// Hang out 12ms

static void FA_NOINLINE( Flash_LED_12MS ) ( void )
{
    LED_on() ;
    Delay_1024( T0DEL4MS ) ;
    LED_off() ;
    Delay_1024( T0DEL8MS ) ;
}

//------------------------------------------------------------------------------

// Initialize FFP/PP by reading the ID (this has to be done, else the PP will
// only use B0/1 for data transfer. Actually only the 3D Pro Plus does that.
// Do it anyways for type determination.
// QueryFFP() needs to be called before this function.

// #include <avr/eeprom.h>

static uint8_t InitFFPro ( void )
{
    uint8_t
        butt,
        idpktsz ;

    butt = BUTPIN ;				// Save current button data

    idpktsz = sw_clkcnt ;			// remember packetsize

//  eeprom_write_byte( (uint8_t *)0, sw_clkcnt ) ;	/**/

    QueryFFP( -(sw_clkcnt - 3), 126 ) ;		// Read ID, signal kick at -3 clk of data packet

    Delay_1024( T0DEL8MS ) ;			// Give the stick some time..

    idpktsz = sw_clkcnt - idpktsz ;		// Sizeof ID packet

//  eeprom_write_byte( (uint8_t *)1, sw_clkcnt ) ;	/**/

    if ( ! QueryFFP( 0, DATSZFFP ) )		// Signal for a regular packet, 16 triplets
	return ( FALSE ) ;			// Timed out..

//  Delay_1024( T0DEL4MS ) ;				/**/
//  eeprom_write_byte( (uint8_t *)2, sw_clkcnt ) ;	/**/
//  eeprom_write_byte( (uint8_t *)3, butt ) ;		/**/
//
//  for ( ;; )						/**/
//  {
//	Flash_LED_12MS() ;
//	Delay_1024( T0DEL500MS ) ;
//  }

    if ( CheckFFPPkt( ffp_packet ) )		// Packet parity checks out
    {
	if ( idpktsz == IDSZPP )
	    sw_id = SW_ID_PP ;			// Found PP
	else
	if ( idpktsz == IDSZFFP )
	    sw_id = SW_ID_FFP ;			// Found FFP
	else
	if ( idpktsz == 0 )			// 10nF timer, FFP or PP in 3bit mode
	{
	    if ( (butt & BUTMSK) == BUTMSK )	// PP
		sw_id = SW_ID_PP ;
	    else				// FFP
		sw_id = SW_ID_FFP ;
	}
	else
	    return ( FALSE ) ;			// Unknown stick..

	CopyFFPData( ffp_packet ) ;		// Copy data to report buffer

	return ( TRUE ) ;			// Got it
    }

    return ( FALSE ) ;				// Stick not found
}

//------------------------------------------------------------------------------

// Initialize 3D Pro by setting it into digital mode followed by reading the ID
// (this has to be done, else the 3DPro will only use B0/1 for data transfer.)
// The ID packet contains: "(ppPNP0F1F\01.00.08)" w/ pp being the encoded PNP
// version no. (1.00)
// QueryFFP() needs to be called before this function,

static uint8_t Init3DPro ( void )
{
    if ( ! sw_clkcnt )				// Previous trigger came up empty
    {
	// Set to digital mode sequence

	dis3DP_INT() ;				// Don't want INT1 to run..

#if NO_556
	Trigger() ;				// Trigger
#else
	if ( ! Trigger() )			// Trigger, wait until 556 timer expired
	    return ( FALSE ) ;			// Timed out
#endif
	Delay_64( T6DEL140US ) ;		// Wait 140us

	Trigger() ;

	Delay_64( (uint8_t)T6DEL865US ) ;	// Wait 140+725us

	Trigger() ;

	Delay_64( T6DEL440US ) ;		// Wait 140+300us

	Trigger() ;

	Delay_1024( T0DEL4MS ) ;		// Hang out 4ms to let everything calm down

						// Try to read a data packet,
	Query3DP( 0, 126 ) ;			// don't know how long (64 or 66) - let it time out

//	eeprom_write_byte( (uint8_t *)0, sw_clkcnt ) ;
//
//	for ( ;; )
//	{
//	    Flash_LED_12MS() ;
//	    Delay_1024( T0DEL500MS ) ;
//	}

	if ( ! sw_clkcnt || ! ~sw_clkcnt )	// nothing..
	    return ( FALSE ) ;

	Delay_1024( T0DEL4MS ) ;		// Another break for the stick
    }

    if ( sw_clkcnt == 64 )			// 3DP in 1-bit mode
    {						// Read ID, signal kick at -8 clk of data packet
						// ID packet is 160 long + 64 or 66
	Query3DP( -(sw_clkcnt - 8), sw_clkcnt + IDSZ3DP ) ;

	Delay_1024( T0DEL8MS ) ;		// Give the 3DP some rest..

//	eeprom_write_block( sw_idbuf, (void *)0, 60+8+8+8 ) ;	/**/
    }
    else
    if ( sw_clkcnt != 66 )			// 3DP not in 3-bit mode
	return ( FALSE ) ;

    if ( ! Query3DP( 0, DATSZ3DP ) )		// Signal for a regular packet, 22 triplets
	return ( FALSE ) ;			// Timed out..

    if ( Check3DPPkt( sw_packet1 ) )		// Packet 1 CRC checks out
    {
	Copy3DPData( sw_packet1 ) ;		// Copy data to report buffer

	sw_id = SW_ID_3DP ;			// Found 3DP

	return ( TRUE ) ;
    }

// It should work the 1st time, let's save some space.
//
//    Delay_1024( T0DEL500US ) ;		// Wait a bit to have 2nd packet ready
//
//    if ( Check3DPPkt( sw_packet2 ) )		// Packet 2 CRC checks out
//    {
//	Copy3DPData( sw_packet2 ) ;		// Copy data to report buffer
//
//	sw_id = SW_ID_3DP ;			// Found 3DP
//
//	return ( TRUE ) ;
//    }

    return ( FALSE ) ;				// Stick not found
}

//------------------------------------------------------------------------------

// Reboot converter. Kill USB and wait for the watchdog to catch us.
// Note: the watchdog is unleashed in main() after init_hw() returns.

static void FA_NOINRET( reboot ) ( void )
{
    cli() ;					// Disable interrupts
 
    // Tri-state ID resistor, D-, D+

#if defined(__AVR_ATtinyX61__)
    DDRA  = DDA ;
    PORTA = PAPU ;
#endif

    DDRB  = DDB ;
    PORTB = PBPU ;

    for ( ;; )					// Wait for watchdog to bite (.5s)
	;
}

//------------------------------------------------------------------------------

#if SIXAXIS

// Need the ADC ISR handler for wakeup from sleep.

EMPTY_INTERRUPT( ADC_vect ) 

//------------------------------------------------------------------------------

#define	ADC_RX		0x00
#define	ADC_RY		0x01

#define	ADC_CRX		0x10
#define	ADC_CRY		0x11

static void ReadADC ( uint8_t chan )
{
    uint8_t
	adc8 ;
    uint16_t
	adc16 ;

//    if ( chan & 0x01 )			// Copy Y to Ry
//    {
//	sw_report[3] = (sw_report[3] & 0x0F) | (sw_report[1] & 0xF0) ;
//	sw_report[4] = (sw_report[4] & 0xF0) | (sw_report[2] & 0x0F) ;
//    }
//    else					// Copy X to Rx
//    {
//	sw_report[2] = (sw_report[2] & 0x0F) | ((sw_report[0] << 2) & 0xF0) ;
//	sw_report[3] = (sw_report[3] & 0xF0) |
//			(((sw_report[1] << 2) & 0x0C) | ((sw_report[0] >> 6) & 0x03)) ;
//    }

    if ( chan & 0x01 )
	set_bit( ADMUX, MUX0 ) ;		// select channel 3, Ry
    else
	clr_bit( ADMUX, MUX0 ) ;		// select channel 2, Rx

    do
	SLEEP() ;				// ADCNR/IDLE sleep, starts conv.
    while ( bit_is_set( ADCSRA, ADSC ) ) ;	// wait for it to finish in
						// case we woke up early
    adc16 = ADC ;				// Read A value, left adj. (!)

    switch ( chan )
    {
	case ADC_RX :
//	    adc8 = (adc16 - AcntrX) >> 8 ;

	    if ( adc16 >= AcntrX )
	    {
		adc8 = -((adc16 - AcntrX) >> 8) ;

//		adc8 = (adc16 - AcntrX) >> 8 ;
//
//		if ( adc8 >= 0x80 )
//		    adc8 = 0x80 ;
//		else
//		    adc8 = -adc8 ;
	    }
	    else
	    {
		adc8 = (AcntrX - adc16) >> 8 ;

		if ( adc8 > 0x7F )
		    adc8 = 0x7F ;
	    }

	    sw_report[2] = (sw_report[2] & 0x0F) | (adc8 << 4) ;
	    sw_report[3] = (sw_report[3] & 0xF0) | (adc8 >> 4) ;

	    break ;

	case ADC_RY :
//	    adc8 = (adc16 - AcntrY) >> 8 ;

	    if ( adc16 >= AcntrY )
		adc8 = -((adc16 - AcntrY) >> 8) ;
	    else
	    {
		adc8 = (AcntrY - adc16) >> 8 ;

		if ( adc8 > 0x7F )
		    adc8 = 0x7F ;
	    }

	    sw_report[3] = (sw_report[3] & 0x0F) | (adc8 << 4) ;
	    sw_report[4] = (sw_report[4] & 0xF0) | (adc8 >> 4) ;

	    break ;

	case ADC_CRX :			// center Rx
	    AcntrX = adc16 ;
	    break ;

	case ADC_CRY :			// center Ry
	    AcntrY = adc16 ;
	    break ;
    }
}

#endif

//------------------------------------------------------------------------------

// Initialize the hardware
//
// init_hw() is called only once, first thing in main().
// Doesn't need a stack frame, saves lots of push/pops

void FA_NAKED( init_hw ) ( void )
{
    ACSCR  = _B1(ACD) ;				// Disable analog comparator

    DDRB   = DDB ;				// Initialize ports
    PORTB  = PBPU ;

#if defined(__AVR_ATmega8__)

    DDRC   = DDC ;
    PORTC  = PCPU ;
    DDRD   = DDD ;
    PORTD  = PDPU ;

    TCCR0  = T0MODE ;				// T0 clock on B1, rising edge
    TCCR1B = T1PS_1024 ;			// Set T1 prescaler to / 1024 for ms delay
    TCCR2  = T2PS_64 ;				// Set T2 prescaler to / 64 for query & us delay

#elif defined(__AVR_ATtinyX61__)

    DDRA   = DDA ;
    PORTA  = PAPU ;

    TCCR0A = _B1(TCW0) ;			// Set T0 to 16bit mode
#endif

    MCUCR  = MCUCRMSK ;				// Set INT sensitivities

    Delay_1024( T0DEL200MS ) ;			// Allow the stick to boot

    for ( ;; )					// Forever..
    {
	Flash_LED_12MS() ;			// Flash LED and wait
	Delay_1024( T0DEL200MS ) ;

#if ! NO_556

	if ( ! Trigger() )			// 556 not expired
	    continue ;

	Flash_LED_12MS() ;			// Flash LED
#endif
						// Try to read a data packet,
	QueryFFP( 0, 126 ) ;			// don't know how long - let it time out

	Flash_LED_12MS() ;

	// Analyze clock count

	if ( ! ~sw_clkcnt )
	    sw_clkcnt = 0 ;

	if (    sw_clkcnt == (16 + 1)		// FFP/PP in 3-bit mode
	     || sw_clkcnt == (48 + 1) )		// 3DPP in 1-bit mode
	{
	    if ( InitFFPro() )			// found FFP/PP
		break ;				// break forever
	}
	else
//	if ( ! sw_clkcnt ||			// Nothing, maybe 3DP in analog mode
//	     sw_clkcnt == 66 ||			// 3DP in 3-bit mode
//	     sw_clkcnt == 64 )			// 3DP in 1-bit mode
	{
	    if ( Init3DPro() )			// found 3DP
	    {
#if SIXAXIS
		Delay_1024( T0DEL4MS ) ;	// Wait for all activity to stop

		// Turn off PC3 & PC2 pull-up's
		clr_bits( PORTC, _B1( PC3) | _B1( PC2) ) ;

		ADMUX  = iADMUX ; 		// Initialize ADC for Rx/Ry
		ADCSRA = iADCSRA ;

		ReadADC( ADC_CRX ) ;		// Start up ADC, read AcntrX
		ReadADC( ADC_CRY ) ;		// Read AcntrY

//		if ( sw_packet1[3] & 0x20 )	// B9 in pos. II, could sig. calibrate
//		{
//		}
#endif
		break ;				// break forever
	    }
	}

	dis3DP_INT() ;
    }

    dis3DP_INT() ;				// Disable INT1

    cli() ;					// Disable interrupts

    sw_buttons = BUTMSK ;			// All buttons released

#if defined(__AVR_ATmega8__)

    TCCR1B = T1PS_64 ;				// Set T1 prescaler to /64 for main loop delay

#elif defined(__AVR_ATtinyX61__)

    TCCR0B = T0PS_64 ;				// Set T0 prescaler to /64 for main loop delay

#endif

    RET() ;					// naked also means no ret..
}

//------------------------------------------------------------------------------

// Patch current 3DP buttons into report

static void patchbutt ( void )
{
    uint8_t
	b = sw_buttons ;

#if SIXAXIS
    __asm__ __volatile__
    (
	"com	%0		\n\t"
	"andi	%0,0b11110000	\n\t"
	"bst	%0,7		\n\t"
	"lsl	%0		\n\t"
	"bld	%0,0		\n\t"
	: "+r" (b) : "0" (b) : "cc"
    ) ;

    // Patch buttons into report data

    sw_report[5] = (sw_report[5] & 0x1F) | (b & 0xE0) ;
    sw_report[6] = (sw_report[6] & 0xFE) | (b & 0x01) ;
#else

    __asm__ __volatile__
    (
#if defined(__AVR_ATtinyX61__)
	"bst	%0,2		\n\t"
	"bld	%0,4		\n\t"
#endif
	"com	%0		\n\t"
	"swap	%0		\n\t"
	"lsl	%0		\n\t"
	"andi	%0,0b00011110	\n\t"
	: "+r" (b) : "0" (b) : "cc"
    ) ;

    // Patch buttons into report data

    sw_report[4+COMPDEV] = (sw_report[4+COMPDEV] & 0xE1) | b ;
#endif
}

//------------------------------------------------------------------------------

// Read the stick and create a report

void getdata ( void )
{
    uint8_t
	i ;

#if ! NO_556

    if ( bit_is_set( EXPPIN, EXPBIT ) )		// 556 timer not expired
    {
        if ( ++sw_problem > 10 )		// 11th problem in a row
	    reboot() ;				// We lost the stick, lets start over

	if ( sw_id == SW_ID_3DP )
	    patchbutt() ;			// patch button data into last good info (3DP only)

	return ;
    }
#endif

#if defined(__AVR_ATmega8__)

    if ( bit_is_clear( TIFR, TOV0 ) )		// Not everything clocked in from last query
    {
        if ( ++sw_problem > 10 )		// 11th problem in a row
            reboot() ;                          // We lost the stick, lets start over
    }
    else
	sw_buttons = BUTPIN ;			// Save current button data
#endif

    if ( sw_id == SW_ID_3DP )
    {
#if defined(__AVR_ATtinyX61__)
	// No timer clock source on INT1 in a tinyX61,
	// we have to assume everyting got clocked in.

	sw_buttons = BUTPIN ;			// Save current button data
#endif

#if SIXAXIS
	ReadADC( ADC_RX ) ;			// Read Rx
	ReadADC( ADC_RY ) ;			// Read Ry
#endif
	i = Query3DP( 0, DATSZ3DP ) ;		// Query 3DP
    }
    else
	i = QueryFFP( 0, DATSZFFP ) ;		// Query FFP

    dis3DP_INT() ;

    if ( ! i )					// If query timed out,
    {
	if ( ++sw_problem > 10 )		// 11th problem in a row
	    reboot() ;				// We lost the stick, lets start over

	if ( sw_id == SW_ID_3DP )
	    patchbutt() ;			// patch button data into last good info (3DP only)
    }
    else
    {
	sw_problem = 0 ;			// Reset problem counter

	if ( sw_id == SW_ID_3DP )
	{
	    if ( Check3DPPkt( sw_packet1 ) )	// If 3DP packet ok
	    {
	 	LED_on() ;			// Signal good packet read

	 	Copy3DPData( sw_packet1 ) ;	// Copy data into report
	    }
	    else
	    {
		// wait 300us, try packet 2
		// not: a FUed CRCpkt1 is most likely caused by a missed INT,
		// let's just go on w/ the show. Don't turn on LED tho.

		patchbutt() ;			// patch button data into last good info
	    }
	}
	else
	if ( CheckFFPPkt( ffp_packet ) )	// If PP/FFP packet ok
	{
	    LED_on() ;				// Signal good packet read

	    CopyFFPData( ffp_packet ) ;		// Copy data into report
	}
    }
}

//------------------------------------------------------------------------------
