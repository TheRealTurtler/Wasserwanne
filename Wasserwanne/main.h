/*
Header Bibliothek Browser:
file:///C:/WinAVR-20100110/doc/avr-libc/avr-libc-user-manual/index.html
µController Defines File Explorer: C:\WinAVR-20100110\avr\include\avr\iom328p.h
*/
#ifndef MAIN_HEADER  //Prevents multiple includes
#define MAIN_HEADER

// =============================================================

// F_CPU also used in: Wasserwanne.h
// Better: define in properties -> symbols : F_CPU=1000000UL
#ifndef F_CPU
//#define F_CPU 16000000UL	// 16 MHz for ATmega2560
#define F_CPU 1000000UL		// 1 MHz for ATtiny84
#endif

// =============================================================

#include <ctype.h>
#include <inttypes.h>
#include <avr/io.h>
//#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <math.h>
#include <stdlib.h>
//#include <stdarg.h>
#include <avr/interrupt.h>  // obsolete #include <avr\signal.h>
#include <avr/wdt.h>
#include <util/delay.h>	//_delay_loop_1 _delay_loop_2 delay_ms
//#include "debug.h"
//#include "uartATM328.h"
#include "uart.h"
#include "TextService.h"
#include "Befehle.h"
#include "Wasserwanne.h"
#include "i2clcd.h"
#include "ADC.h"

#define _CLEARMASK_	&=	//Und= löscht Bits OHNE Komplement
#define _CLEARBIT_	&=~	//Und=~ löscht Bits mit Komplement
#define _SETBIT_	|=	//Oder= setzts Bits
#define _TOOGLEBIT_	^=  //XOR= TOGGLE  BITS  Wie ein Wechselschalter
#define _UNGLEICH_	!=
#define _AND_LOG_	&&
#define _OR_LOG_	||
#define _AND_BIN_	&
#define _OR_BIN_	|

#define DISABLE_ALL_PULLUP()	(SFIOR |= _BV(PUD));
#define ENABLE_ALLUP_SCL()		(SFIOR &= ~(_BV(PUD)));

#define LED5	0x20
#define SETLED() (PORTB |= LED5)
#define CLRLED() (PORTB &= ~LED5)


// GLOBALE VARIABLEN
//CONSTANT ARRAYs IN .TEXT = ROM ARREA   .DATA IST SRAM !! .BSS SRA
#define TICKS_1000MS	250
#define TICKS_100MS		25
#define TICKS_25HZ		10

typedef struct
{
	unsigned char TickEvent_Flag	: 1;
	unsigned char TICK_100MS__Flag	: 1;
	unsigned char Geschwaetzig		: 1;
} BIT_FIELD_TYPE;

//=========================================================
//PORT_ASSIGMNEMTS

// ========================================================
// *******   PORTB   ****************

#define INTERN_LED_BIT		(1 << PB5)					// Shiftleft "1" 5 mal
#define INTERN_LED_PORT		PORTB
#define INTERN_LED_INIT()	(DDRB |= INTERN_LED_BIT)	// OR = 
#define INTERN_LED_ON()		(PORTB |= INTERN_LED_BIT)	// OR =
#define INTERN_LED_OFF()	(PORTB &= ~INTERN_LED_BIT)	// AND = Komplement
#define INTERN_LED_TOGGLE()	(PORTB ^= INTERN_LED_BIT)	// XOR =


extern volatile BIT_FIELD_TYPE gstFlags;
extern volatile uint32_t gu32Ticks;

#endif //MAIN_HEADER
