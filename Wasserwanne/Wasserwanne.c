/*
 * Wasserwanne.c
 *
 * Created: 28.03.2020 18:47:06
 *  Author: Michael
 */

#include "Wasserwanne.h"

#ifdef WASSERWANNE_USED

// ================ static Funktionen =====================

static void InitOverrideActivateInterrupt( void );
static void InitOverrideInterrupt( void );
static void InitSensorInterrupt( void );
static void InitTimer2( void );


// ================ Globale Variablen =====================

volatile WASSERWANNE_FLAGS gstWasserwanneFlags;
volatile WASSERWANNE_DATA gstWasserwanneData;


// ================ Interrupts ============================

// **************************
ISR( PCINT2_vect )
// **************************
// no longer used
{
	gstWasserwanneData.u16Bounces++;
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
	gstWasserwanneData.u32Ticks++;
	
	if ( gstWasserwanneFlags.Start_F )
	{
		if ( gstWasserwanneFlags.Valve_On_F )
		{
			SET_VALVE_ON_BIT();
			CLEAR_VALVE_OFF_BIT();
			
			gstWasserwanneFlags.Valve_State_F = 1;
		}
		else if ( gstWasserwanneFlags.Valve_Off_F )
		{
			SET_VALVE_OFF_BIT();
			CLEAR_VALVE_ON_BIT();
			
			gstWasserwanneFlags.Valve_State_F = 0;
		}
		
		gstWasserwanneData.u16ValveTicks = 0;
		
		gstWasserwanneFlags.Start_F = 0;
		gstWasserwanneFlags.Active_F = 1;
	}
	
	if ( gstWasserwanneFlags.Active_F )
	{
		gstWasserwanneData.u16ValveTicks++;
		
		if ( gstWasserwanneData.u16ValveTicks > VALVE_SIGNAL_TIME_MS )
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
	// no longer used
	return;
}

// **************************
static void InitOverrideInterrupt( void )			// INT2
// **************************
{
	// no longer used
	return;
}

// **************************
static void InitSensorInterrupt( void )				// PCINT16
// **************************
{
	// no longer used
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
	TCCR2B = _BV( CS22 );									// Prescaler / 64
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
	
	gstWasserwanneData.u32Ticks = 0;
	gstWasserwanneData.u16ValveTicks = 0;
	gstWasserwanneData.u16Bounces = 0;
	gstWasserwanneData.u16SensorSwitches = 0;
	gstWasserwanneData.u8CurrentSensorState = 0;
	gstWasserwanneData.u8LastSensorState = 0;
	
	//InitOverrideActivateInterrupt();
	//InitOverrideInterrupt();
	//InitSensorInterrupt();
	
	InitTimer2();
	
	sei();
}


// **************************
void CheckWaterSensor( void )
// **************************
{
	// Sensor-switch debouncing (according to Arduino sample project)
	static uint32_t u32LastBounceTime = 0;
	
	uint8_t u8SensorState = SENSOR_PINX & SENSOR_BIT;
	
	if ( u8SensorState != gstWasserwanneData.u8LastSensorState )
	{
		gstWasserwanneData.u16Bounces++;
		u32LastBounceTime = gstWasserwanneData.u32Ticks;
	}
	
	// delay + signal time to ensure the previous signal has finished sending
	if ( (gstWasserwanneData.u32Ticks - u32LastBounceTime) > (DEBOUNCE_DELAY_MS + VALVE_SIGNAL_TIME_MS) )
	{
		if(u8SensorState != gstWasserwanneData.u8CurrentSensorState)
		{
			gstWasserwanneData.u8CurrentSensorState = u8SensorState;
			
			gstWasserwanneData.u16SensorSwitches++;
			
			if ( gstWasserwanneData.u8LastSensorState )
			{
				gstWasserwanneFlags.Valve_On_F = 1;
				gstWasserwanneFlags.Valve_Off_F = 0;
			}
			else
			{
				gstWasserwanneFlags.Valve_On_F = 0;
				gstWasserwanneFlags.Valve_Off_F = 1;
			}
			
			gstWasserwanneFlags.Start_F = 1;	
		}
	}
	
	gstWasserwanneData.u8LastSensorState = u8SensorState;
}

#endif