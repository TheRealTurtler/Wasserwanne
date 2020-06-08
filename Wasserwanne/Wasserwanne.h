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

#define SENSOR_DDRX							DDRA
#define SENSOR_PORTX						PORTA
#define SENSOR_PINX							PINA
#define SENSOR_BIT							_BV(0)
#define INIT_SENSOR_BIT()					(SENSOR_DDRX &= ~SENSOR_BIT)
#define INIT_SENSOR_BIT_PULLUP()			(SENSOR_PORTX |= SENSOR_BIT)

#define VALVE_ON_DDRX						DDRA
#define VALVE_ON_PORTX						PORTA
#define VALVE_ON_BIT						_BV(3)
#define INIT_VALVE_ON_BIT()					(VALVE_ON_DDRX |= VALVE_ON_BIT)
#define SET_VALVE_ON_BIT()					(VALVE_ON_PORTX |= VALVE_ON_BIT)
#define CLEAR_VALVE_ON_BIT()				(VALVE_ON_PORTX &= ~VALVE_ON_BIT)

#define VALVE_OFF_DDRX						DDRA
#define VALVE_OFF_PORTX						PORTA
#define VALVE_OFF_BIT						_BV(4)
#define INIT_VALVE_OFF_BIT()				(VALVE_OFF_DDRX |= VALVE_OFF_BIT)
#define SET_VALVE_OFF_BIT()					(VALVE_OFF_PORTX |= VALVE_OFF_BIT)
#define CLEAR_VALVE_OFF_BIT()				(VALVE_OFF_PORTX &= ~VALVE_OFF_BIT)

#define OVERRIDE_DDRX						DDRA
#define OVERRIDE_PORTX						PORTA
#define OVERRIDE_PINX						PINA
#define OVERRIDE_BIT						_BV(1)
#define INIT_OVERRIDE_BIT()					(OVERRIDE_DDRX &= ~ OVERRIDE_BIT)
#define INIT_OVERRIDE_BIT_PULLUP()			(OVERRIDE_PORTX |= OVERRIDE_BIT)

#define OVERRIDE_ACTIVATE_DDRX				DDRA
#define OVERRIDE_ACTIVATE_PORTX				PORTA
#define OVERRIDE_ACTIVATE_PINX				PINA
#define OVERRIDE_ACTIVATE_BIT				_BV(2)
#define INIT_OVERRIDE_ACTIVATE_BIT()		(OVERRIDE_ACTIVATE_DDRX &= ~OVERRIDE_ACTIVATE_BIT)
#define INT_OVERRIDE_ACTIVATE_BIT_PULLUP()	(OVERRIDE_ACTIVATE_PORTX |= OVERRIDE_ACTIVATE_BIT)

#define WASSERWANNE_BUSY_LED_DDRX			DDRA
#define WASSERWANNE_BUSY_LED_PORTX			PORTA
#define WASSERWANNE_BUSY_LED_BIT			_BV(5)
#define INIT_WASSERWANNE_BUSY_LED_BIT()		(WASSERWANNE_BUSY_LED_DDRX |= WASSERWANNE_BUSY_LED_BIT)
#define SET_WASSERWANNE_BUSY_LED_BIT()		(WASSERWANNE_BUSY_LED_PORTX |= WASSERWANNE_BUSY_LED_BIT)
#define CLEAR_WASSERWANNE_BUSY_LED_BIT()	(WASSERWANNE_BUSY_LED_PORTX &= ~ WASSERWANNE_BUSY_LED_BIT)
#define TOGGLE_WASSERWANNE_BUSY_LED_BIT()	(WASSERWANNE_BUSY_LED_PORTX ^= WASSERWANNE_BUSY_LED_BIT)

#define VALVE_SIGNAL_TIME_MS		30U
#define SENSOR_DEBOUNCE_DELAY_MS	500U
#define OVERRIDE_DEBOUNCE_DELAY_MS	500U

#define SENSOR_ON_STATE				0	// Defines wether a high (1) or low (0) signals an activated sensor
#define OVERRIDE_ACTIVATE_ON_STATE	0	// Defines wether a high (1) or low (0) signals an activated override activation
#define OVERRIDE_ON_STATE			0	// Defines wether a high (1) or low (0) signals an activated override

// ================ Structs ===============================

typedef struct
{
	unsigned char Start_F : 1;
	unsigned char Active_F : 1;
	unsigned char Valve_On_F : 1;
	unsigned char Valve_Off_F : 1;
	unsigned char Valve_State_F : 1;
	unsigned char Override_Active_F : 1;
} WASSERWANNE_FLAGS;

typedef struct
{
	uint32_t u32Ticks;
	uint16_t u16ValveTicks;
} WASSERWANNE_DATA;

#ifdef WASSERWANNE_DEBUG_USED
typedef struct
{
	unsigned char Debug_F : 1;
	uint8_t u8Debug;
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
uint8_t DebounceButton( bool bButtonState, bool* bLastButtonState, bool* bSetButtonState,
                        uint32_t* u32LastBounceTime, uint32_t u32DebounceDelay, uint32_t u32TickCounter );
uint8_t DebounceSwitch( bool bSwitchState, bool* bLastSwitchState, bool* bSetSwitchState,
                        uint32_t* u32LastBounceTime, uint32_t u32DebounceDelay, uint32_t u32TickCounter );
void CloseValve( void );
bool CheckOverrideActivate( void );
void CheckOverride( void );

#endif

#endif /* WASSERWANNE_H_ */