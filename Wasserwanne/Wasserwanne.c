/*
 * Wasserwanne.c
 *
 * Created: 28.03.2020 18:47:06
 *  Author: Michael
 */

#include "Wasserwanne.h"

// ================ static Funktionen =====================

static void InitOverrideActivateInterrupt( void );
static void InitOverrideInterrupt( void );
static void InitSensorInterrupt( void );
static void InitTimer2( void );


// ================ Globale Variablen =====================

volatile WASSERWANNE_FLAGS gstWasserwanneFlags;


// ================ Interrupts ============================

// **************************
ISR( PCINT2_vect )
// **************************
{
	TOGGLE_WASSERWANNE_BUSY_LED_BIT();
	
	if ( SENSOR_PINX & SENSOR_BIT )
	{
		gstWasserwanneFlags.Valve_On_F = 1;
	}
	else if ( !( SENSOR_PINX & SENSOR_BIT ) )
	{
		gstWasserwanneFlags.Valve_Off_F = 1;
	}
	
	gstWasserwanneFlags.Start_F = 1;
}

// **************************
ISR( TIMER2_COMPA_vect )
// **************************
{
	static uint8_t u8WasserwanneTicks = 0;
	
	if ( gstWasserwanneFlags.Start_F )
	{
		if ( gstWasserwanneFlags.Valve_On_F )
		{
			SET_VALVE_ON_BIT();
		}
		else if ( gstWasserwanneFlags.Valve_Off_F )
		{
			SET_VALVE_OFF_BIT();
		}
		
		u8WasserwanneTicks = 0;
		
		gstWasserwanneFlags.Start_F = 0;
		gstWasserwanneFlags.Active_F = 1;
	}
	
	if ( gstWasserwanneFlags.Active_F )
	{
		u8WasserwanneTicks++;
		
		if ( u8WasserwanneTicks > VALVE_SIGNAL_TIME_MS )
		{
			if ( gstWasserwanneFlags.Valve_On_F )
			{
				CLEAR_VALVE_ON_BIT();
				gstWasserwanneFlags.Valve_On_F = 0;
			}
			else if ( gstWasserwanneFlags.Valve_Off_F )
			{
				CLEAR_VALVE_OFF_BIT();
				gstWasserwanneFlags.Valve_Off_F = 0;
			}
			
			gstWasserwanneFlags.Active_F = 0;
		}
	}
}


// ================ Funktionen ============================

// **************************
static void InitOverrideActivateInterrupt( void )	// INT1
// **************************
{
	return;
}

// **************************
static void InitOverrideInterrupt( void )			// INT2
// **************************
{
	return;
}

// **************************
static void InitSensorInterrupt( void )				// PCINT16
// **************************
{
	PCICR |= _BV( PCIE2 );
	PCMSK2 |= _BV( PCINT16 );
}

// **************************
static void InitTimer2( void )
// **************************
{
	TCNT2 = 0;
	OCR2A = 250;											// 16000000 / 64 / 250 = 1 kHz -> 1 ms
	TCCR2A = _BV( WGM21 );									// CTC mode
	TCCR2B = _BV( CS20 );									// Prescaler / 64
	TIMSK2 = _BV( OCIE2A );									// Interrrupt enable
}

// **************************
void InitWasserwanne( void )
// **************************
{
	cli();
	
	//INIT_OVERRIDE_BIT();
	
	//INIT_OVERRIDE_ACTIVATE_BIT();
	
	INIT_SENSOR_BIT();
	INIT_SENSOR_BIT_PULLUP();
	
	INIT_VALVE_ON_BIT();
	INIT_VALVE_OFF_BIT();
	INIT_WASSERWANNE_BUSY_LED_BIT();
	
	gstWasserwanneFlags.Start_F = 0;
	gstWasserwanneFlags.Active_F = 0;
	gstWasserwanneFlags.Valve_On_F = 0;
	gstWasserwanneFlags.Valve_Off_F = 0;
	
	InitOverrideActivateInterrupt();
	InitOverrideInterrupt();
	InitSensorInterrupt();
	
	InitTimer2();
	
	sei();
}


// **************************
void CheckWaterSensor( void )
// **************************
{
	cli();
	TOGGLE_WASSERWANNE_BUSY_LED_BIT();
	sei();
}