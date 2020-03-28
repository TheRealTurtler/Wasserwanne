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
		SET_VALVE_BIT();
		gstWasserwanneFlags.Valve_On_F = 1;
	}
	else if ( !( SENSOR_PINX & SENSOR_BIT ) )
	{
		CLEAR_VALVE_BIT();
		gstWasserwanneFlags.Valve_On_F = 0;
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
void InitWasserwanne( void )
// **************************
{
	//INIT_OVERRIDE_BIT();
	
	//INIT_OVERRIDE_ACTIVATE_BIT();
	
	INIT_SENSOR_BIT();
	INIT_SENSOR_BIT_PULLUP();
	
	INIT_VALVE_BIT();
	INIT_WASSERWANNE_BUSY_LED_BIT();
	
	InitOverrideActivateInterrupt();
	InitOverrideInterrupt();
	InitSensorInterrupt();
	
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