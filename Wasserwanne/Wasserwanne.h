/*
 * Wasserwanne.h
 *
 * Created: 28.03.2020 18:47:18
 *  Author: Michael
 */


#ifndef WASSERWANNE_H_
#define WASSERWANNE_H_

#include "Modules.h"

#ifdef WASSERWANNE_USED

// ================ Includes ==============================

//#include <ctype.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/atomic.h>
#include <stdbool.h>

#ifdef UART_USE_ENABLED
#include "uart.h"
#include "TextService.h"
#endif

// ================ Definitionen ==========================

#define SENSOR_DDRX							DDRK
#define SENSOR_PORTX						PORTK
#define SENSOR_PINX							PINK
#define SENSOR_BIT							_BV(0)
#define INIT_SENSOR_BIT()					(SENSOR_DDRX &= ~SENSOR_BIT)
#define INIT_SENSOR_BIT_PULLUP()			(SENSOR_PORTX |= SENSOR_BIT)

#define VALVE_ON_DDRX						DDRF
#define VALVE_ON_PORTX						PORTF
#define VALVE_ON_BIT						_BV(0)
#define INIT_VALVE_ON_BIT()					(VALVE_ON_DDRX |= VALVE_ON_BIT)
#define SET_VALVE_ON_BIT()					(VALVE_ON_PORTX |= VALVE_ON_BIT)
#define CLEAR_VALVE_ON_BIT()				(VALVE_ON_PORTX &= ~VALVE_ON_BIT)

#define VALVE_OFF_DDRX						DDRF
#define VALVE_OFF_PORTX						PORTF
#define VALVE_OFF_BIT						_BV(1)
#define INIT_VALVE_OFF_BIT()				(VALVE_OFF_DDRX |= VALVE_OFF_BIT)
#define SET_VALVE_OFF_BIT()					(VALVE_OFF_PORTX |= VALVE_OFF_BIT)
#define CLEAR_VALVE_OFF_BIT()				(VALVE_OFF_PORTX &= ~VALVE_OFF_BIT)

#define OVERRIDE_DDRX						DDRK
#define OVERRIDE_PORTX						PORTK
#define OVERRIDE_PINX						PINK
#define OVERRIDE_BIT						_BV(1)
#define INIT_OVERRIDE_BIT()					(OVERRIDE_DDRX &= ~ OVERRIDE_BIT)
#define INIT_OVERRIDE_BIT_PULLUP()			(OVERRIDE_PORTX |= OVERRIDE_BIT)

#define OVERRIDE_ACTIVATE_DDRX				DDRK
#define OVERRIDE_ACTIVATE_PORTX				PORTK
#define OVERRIDE_ACTIVATE_PINX				PINK
#define OVERRIDE_ACTIVATE_BIT				_BV(2)
#define INIT_OVERRIDE_ACTIVATE_BIT()		(OVERRIDE_ACTIVATE_DDRX &= ~OVERRIDE_ACTIVATE_BIT)
#define INT_OVERRIDE_ACTIVATE_BIT_PULLUP()	(OVERRIDE_ACTIVATE_PORTX |= OVERRIDE_ACTIVATE_BIT)

#define WASSERWANNE_BUSY_LED_DDRX			DDRF
#define WASSERWANNE_BUSY_LED_PORTX			PORTF
#define WASSERWANNE_BUSY_LED_BIT			_BV(2)
#define INIT_WASSERWANNE_BUSY_LED_BIT()		(WASSERWANNE_BUSY_LED_DDRX |= WASSERWANNE_BUSY_LED_BIT)
#define SET_WASSERWANNE_BUSY_LED_BIT()		(WASSERWANNE_BUSY_LED_PORTX |= WASSERWANNE_BUSY_LED_BIT)
#define CLEAR_WASSERWANNE_BUSY_LED_BIT()	(WASSERWANNE_BUSY_LED_PORTX &= ~ WASSERWANNE_BUSY_LED_BIT)
#define TOGGLE_WASSERWANNE_BUSY_LED_BIT()	(WASSERWANNE_BUSY_LED_PORTX ^= WASSERWANNE_BUSY_LED_BIT)

#define VALVE_SIGNAL_TIME_MS 30U
#define DEBOUNCE_DELAY_MS 2000UL

#define SENSOR_ON_STATE 0				// Defines wether a high (1) or low (0) signals an activated sensor

// ================ Structs ===============================

typedef struct
{
	unsigned char Start_F : 1;
	unsigned char Active_F : 1;
	unsigned char Valve_On_F : 1;
	unsigned char Valve_Off_F : 1;
	unsigned char Valve_State_F : 1;
} WASSERWANNE_FLAGS;

typedef struct
{
	uint32_t u32Ticks;
	uint16_t u16ValveTicks;
} WASSERWANNE_DATA;

#ifdef WASSERWANNE_DEBUG_USED
typedef struct
{
	uint8_t u8Debounce;
} WASSERWANNE_DEBUG;
#endif

extern volatile WASSERWANNE_FLAGS gstWasserwanneFlags;
extern volatile WASSERWANNE_DATA gstWasserwanneData;

#ifdef WASSERWANNE_DEBUG_USED
extern volatile WASSERWANNE_DEBUG gstWasserwanneDebug;
#endif

// ================ Funktionen ============================

void InitWasserwanne( void );
void CheckWaterSensor( void );
uint8_t Debounce( bool bSwitchState, bool* bLastSwitchState, bool* bSetSwitchState,
                  uint32_t* u32LastBounceTime, uint32_t u32DebounceDelay, uint32_t u32TickCounter );
void CloseValve( void );

#endif

#endif /* WASSERWANNE_H_ */