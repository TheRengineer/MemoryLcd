/*****************************************************************************
 *
 *  Project Name:       Sharp Memory Lcd Library
 *  File Name:          stenFont.h
 *  Author:             Adam Fabio
 *  Creation Date:      03/12/2012
 *
 *  Description:   		Declarations for the orbitron font.
 *
 ******************************************************************************/
#ifndef STENFONT_H
#define STENFONT_H

#include <avr/pgmspace.h>
#include "MemoryLCD.h"

// Character descriptors for Orbitron 10pt
// { [Char width in bits], [Char height in bits], [Offset into orbitron10ptCharBitmaps in bytes] }
extern PROGMEM FONT_CHAR_INFO orbitron10ptDescriptors[];

#if 0
// Font information for Arial 12pt
const FONT_INFO arial12ptFontInfo =
{
	17, //  Character height
	' ', //  Start character
	'~', //  End character
	arial12ptDescriptors, //  Character descriptor array
	arial12ptBitmaps, //  Character bitmap array
};
#endif

// Character bitmaps for Orbitron 10pt
extern PROGMEM prog_uchar orbitron10ptBitmaps[];

#endif
