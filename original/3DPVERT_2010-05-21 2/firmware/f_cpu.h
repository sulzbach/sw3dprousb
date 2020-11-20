/*******************************************************************************
 * File Name	: f_cpu.h
 * Project	: Microsoft Sidewinder 3D Pro to USB converter
 * Date		: 2005/05/31, 2007/06/25
 * Version	: 2.0
 * Target MCU	: ATmega8
 * Tool Chain	: Atmel AVR Studio 4.11 SP4 / WinAVR 20060421
 * Author	: Detlef "Grendel" Mueller
 *		  detlef@gmail.com
 * Release Notes:
 *
 * $Id: f_cpu.h 1.5 2008/04/28 08:14:55 Detlef Exp Detlef $
 *
 * $Log: f_cpu.h $
 * Revision 1.5  2008/04/28 08:14:55  Detlef
 * Removed tiny461 dependency, removed tailing 'L' for ASM compatibility
 *
 * Revision 1.4  2008/02/25 09:21:00  Detlef
 * tiny461 will define 16MHz crystal, 12 & 15 will create code > 4kb.
 *
 * Revision 1.3  2008/02/20 03:28:29  Detlef
 * Added 15MHz define
 *
 * Revision 1.2  2008/02/18 09:44:20  Detlef
 * Added 16MHz define
 *
 * Revision 1.1  2007/06/26 06:24:01  Detlef
 * Initial revision
 *
 ******************************************************************************/

#ifndef	_F_CPU_H_
#define	_F_CPU_H_

//-------------------------------------------------------------------------------

 #define	F_CPU		12000000
// #define	F_CPU		15000000
// #define	F_CPU		16000000

#endif	// _F_CPU_H_
