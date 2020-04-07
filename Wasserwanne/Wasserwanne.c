/*
 * Wasserwanne.c
 *
 * Created: 28.03.2020 18:47:06
 *  Author: Michael
 */

#include "Wasserwanne.h"

#ifdef WASSERWANNE_USED

// ================ static Funktionen =====================

static void InitTimer2( void );


// ================ Globale Variablen =====================

volatile WASSERWANNE_FLAGS gstWasserwanneFlags;
volatile WASSERWANNE_DATA gstWasserwanneData;


#ifdef WASSERWANNE_DEBUG_USED
volatile WASSERWANNE_DEBUG gstWasserwanneDebug;
#endif


// ================ Interrupts ============================

/**************************************************************************************************
Function:	Timer 2 Compare A Interrupt
Purpose:	Sets the output ports according to set flags
Frequency:	1 kHz
**************************************************************************************************/
ISR( TIMER2_COMPA_vect )
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

/**************************************************************************************************
Function:	Initialize timer 2
Purpose:	Starts timer 2 with a frequency of 1 kHz -> 1 ms cycle time
Requirements:	--
Arguments:	--
Return:		--
**************************************************************************************************/
static void InitTimer2( void )
{
	TCNT2 = 0;
	OCR2A = 250;											// 16000000 / 64 / 250 = 1 kHz -> 1 ms
	TCCR2A = _BV( WGM21 );									// CTC mode
	TCCR2B = _BV( CS22 );									// Prescaler / 64
	TIMSK2 = _BV( OCIE2A );									// Interrrupt enable
}


/**************************************************************************************************
Function:	Initialize Wasserwanne
Purpose:	Initializes ports, structs and calls the InitTimer2() and CloseValve() functions
Requirements:	--
Arguments:	--
Return:		--
**************************************************************************************************/
void InitWasserwanne( void )
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
	
	
#ifdef WASSERWANNE_DEBUG_USED
	gstWasserwanneDebug.u8Debounce = 0;
#endif
	
	InitTimer2();
	
	sei();
	
	CloseValve();
}


/**************************************************************************************************
Function:	Check water-sensor
Purpose:	Checks the sensor-bit for changes and sets flags accordingly
Requirements:	stdbool.h
Arguments:	--
Return:		--
**************************************************************************************************/
void CheckWaterSensor( void )
{
	// Sensor-switch debouncing (according to Arduino sample project)
	static uint32_t u32LastBounceTime = 0;
	static bool bLastSensorState = SENSOR_ON_STATE;
	static bool bSetSensorState = ~SENSOR_ON_STATE;
	
	bool bSensorState;
	
	if ( SENSOR_PINX & SENSOR_BIT )
	{
		bSensorState = 1;
	}
	else
	{
		bSensorState = 0;
	}
	
	uint8_t u8Debounce = Debounce( bSensorState, &bLastSensorState,
	                               &bSetSensorState, &u32LastBounceTime, DEBOUNCE_DELAY_MS +
	                               VALVE_SIGNAL_TIME_MS, gstWasserwanneData.u32Ticks );
	                               
	if ( u8Debounce < 2 )
	{
		if ( u8Debounce == SENSOR_ON_STATE )
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
	
#ifdef WASSERWANNE_DEBUG_USED
	gstWasserwanneDebug.u8Debounce = u8Debounce;
#endif
}


/**************************************************************************************************
Function:	Debounce
Purpose:	Debouncing of a mechanical switch
Requirements:	stdbool.h
Arguments:	bSwitchState:		Current state of the switch-pin (SWITCH_PINX & SWITCH_BIT)
			bLastSwitchState:	State of the switch-pin the last time this function was called
			bSetSwitchState:	Currently set switch-state (after debouncing)
			u32LastBounceTime:	Time the switch last changed its state
			u32DebounceDelay:	Delay until no state changes means no further bounces
			u32TickCounter:		Externally incremented variable to keep track of time
Return:		0: Finished debouncing and switch state is low (0)
			1: Finished debouncing and switch state is high (1)
			2: Not finished debouncing, function needs to be called again
			3: Switch did not change state
**************************************************************************************************/
uint8_t Debounce( bool bSwitchState, bool* bLastSwitchState, bool* bSetSwitchState,
                  uint32_t* u32LastBounceTime, uint32_t u32DebounceDelay, uint32_t u32TickCounter )
{
	uint8_t returnVal;
	
	if ( bSwitchState != *bLastSwitchState )
	{
		*u32LastBounceTime = u32TickCounter;
	}
	
	// delay + signal time to ensure the previous signal has finished sending
	if ( ( u32TickCounter - *u32LastBounceTime ) > ( u32DebounceDelay ) )
	{
	
		if ( bSwitchState != *bSetSwitchState )
		{
			*bSetSwitchState = bSwitchState;
			
			if ( *bLastSwitchState )
			{
				returnVal = 1;
			}
			else
			{
				returnVal = 0;
			}
		}
		else
		{
			returnVal = 3;
		}
	}
	else
	{
		returnVal = 2;
	}
	
	*bLastSwitchState = bSwitchState;
	
	return returnVal;
}


/**************************************************************************************************
Function:	Close valve
Purpose:	Sets the flags for closing the valve
Requirements:	--
Arguments:	--
Return:		--
**************************************************************************************************/
void CloseValve( void )
{
	gstWasserwanneFlags.Valve_On_F = 0;
	gstWasserwanneFlags.Valve_Off_F = 1;
	
	gstWasserwanneFlags.Start_F = 1;
	
	while(gstWasserwanneFlags.Start_F || gstWasserwanneFlags.Active_F)
	{
		TOGGLE_WASSERWANNE_BUSY_LED_BIT();
	}	
}

#endif