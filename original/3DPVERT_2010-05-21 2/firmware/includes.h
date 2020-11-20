/*******************************************************************************
 * File Name	: includes.h
 * Project	: Generic AVR based
 * Date		: 2009/06/22
 * Version      : 1.0
 * Target MCU   : AVR
 * Tool Chain   : Atmel AVR Studio 4.16 638 / WinAVR 20090313
 * Author       : Detlef Mueller
 *                detlef@gmail.com
 * Release Notes:
 *
 * $Id$
 *
 * $Log$
 ******************************************************************************/

#ifndef	__includes_h__
#define	__includes_h__

//------------------------------------------------------------------------------

#if ! defined(__AVR_ATmega8__)
 #if defined(__AVR_ATtiny461__) || defined(__AVR_ATtiny861__)
  #define __AVR_ATtinyX61__
 #else
  #error "Unsupported device, must be atmega8, attiny461, or attiny861"
 #endif
#endif

//------------------------------------------------------------------------------

#ifdef	__ASSEMBLER__

//------------------------------------------------------------------------------

#define	__SFR_OFFSET	0
//#define	_VECTOR(N)	__vector_ ## N	/* io.h does not define this for asm */

#include <avr/io.h>

//------------------------------------------------------------------------------

#else  // ! __ASSEMBLER__

//------------------------------------------------------------------------------

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

//------------------------------------------------------------------------------

#define set_bit( sfr, bit )	(_SFR_BYTE(sfr) |=  _BV(bit))
#define clr_bit( sfr, bit )	(_SFR_BYTE(sfr) &= ~_BV(bit))
#define tog_bit( sfr, bit )	(_SFR_BYTE(sfr) ^=  _BV(bit))

#define set_bits( sfr, msk )	(_SFR_BYTE(sfr) |=  (msk))
#define clr_bits( sfr, msk )	(_SFR_BYTE(sfr) &= ~(msk))
#define tog_bits( sfr, msk )	(_SFR_BYTE(sfr) ^=  (msk))

#define	bits_are_set( n, msk )	 (((n) & (msk)) == (msk))
#define	bits_are_clear( n, msk ) (!((n) & (msk)))

//------------------------------------------------------------------------------

#define	__FA__( _func, ... )	__attribute__((__VA_ARGS__)) _func

#define	FA_NAKED( _f )		__FA__( _f, __naked__ )
#define	FA_NORETURN( _f )	__FA__( _f, __noreturn__ )
#define	FA_NOINLINE( _f )	__FA__( _f, __noinline__ )
#define	FA_NOINRET( _f )	__FA__( _f, __noinline__,__noreturn__ )
#define	FA_INIT3( _f )		__FA__( _f, __used__,__naked__,__section__(".init3") )

//------------------------------------------------------------------------------

#define	VA_PROGMEM( _v )	_v PROGMEM

//------------------------------------------------------------------------------

#define	__WRAP__( _c )		do _c while (0)

//------------------------------------------------------------------------------

#define	CRITICAL_VAR()		uint8_t __sSREG
#define	ENTER_CRITICAL()	__WRAP__( { __sSREG = SREG ; cli() ; } )
#define	EXIT_CRITICAL()		(SREG = __sSREG)
#define	EXIT_CRITICAL_RET( n )	__WRAP__( { SREG = __sSREG ; return ( n ) ; } )

//------------------------------------------------------------------------------

#define	RET()			__asm__ __volatile__ ( "ret\n\t" :: )
#define	SLEEP()			__asm__ __volatile__ ( "sleep\n\t" :: )
#define RESET()			__asm__ __volatile__ ( "jmp 0\n\t" :: )
#define NOP()			__asm__ __volatile__ ( "nop\n\t" :: )

//------------------------------------------------------------------------------

#define	VP( p )			(void *)(p)
#define ARRSZ( a )		(sizeof(a) / sizeof(*(a)))

#ifndef	FALSE
 #define FALSE			0
#endif

#ifndef	TRUE
 #define TRUE			(! FALSE)
#endif

#ifndef NULL
 #define NULL			0
#endif

//------------------------------------------------------------------------------

#endif // __ASSEMBLER__

//------------------------------------------------------------------------------

#define	_B1( b )		_BV(b)
#define	_B0( b )		0

//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------

