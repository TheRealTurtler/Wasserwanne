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
ISR( TIM1_COMPA_vect )
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
	TCNT1 = 0;
	OCR1A = 1000;											// 16000000 / 64 / 250 = 1 kHz -> 1 ms
	TCCR1A = _BV( WGM11 );									// CTC mode
	TCCR1B = _BV( CS10 );									// Prescaler / 64
	TIMSK1 = _BV( OCIE1A );									// Interrrupt enable
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
	gstWasserwanneFlags.Valve_State_F = 0;
	
	gstWasserwanneData.u32Ticks = 0;
	gstWasserwanneData.u16ValveTicks = 0;
	
	
#ifdef WASSERWANNE_DEBUG_USED
	gstWasserwanneDebug.Debug_F = 0;
	gstWasserwanneDebug.u8Debug = 0;
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
	
	uint8_t u8Debounce = DebounceSwitch( bSensorState, &bLastSensorState,
	                                     &bSetSensorState, &u32LastBounceTime, SENSOR_DEBOUNCE_DELAY_MS +
	                                     VALVE_SIGNAL_TIME_MS,
	                                     gstWasserwanneData.u32Ticks );
	                                     
	if ( u8Debounce < 2 )
	{
		if ( u8Debounce == SENSOR_ON_STATE && !gstWasserwanneFlags.Valve_State_F )
		{
			gstWasserwanneFlags.Valve_On_F = 1;
			gstWasserwanneFlags.Valve_Off_F = 0;
			
			gstWasserwanneFlags.Start_F = 1;
		}
		else if ( u8Debounce != SENSOR_ON_STATE && gstWasserwanneFlags.Valve_State_F )
		{
			gstWasserwanneFlags.Valve_On_F = 0;
			gstWasserwanneFlags.Valve_Off_F = 1;
			
			gstWasserwanneFlags.Start_F = 1;
		}
		
		
	}
	
#ifdef WASSERWANNE_DEBUG_USED
	gstWasserwanneDebug.u8Debug = u8Debounce;
#endif
}


/**************************************************************************************************
Function:	Debounce button
Purpose:	Debouncing of a mechanical button
Requirements:	stdbool.h
Arguments:	bSwitchState:		Current state of the button-pin
			bLastSwitchState:	State of the button-pin the last time this function was called
			bSetSwitchState:	Currently set button-state (after debouncing)
			u32LastBounceTime:	Time the button last changed its state
			u32DebounceDelay:	Delay until no state changes means no further bounces
			u32TickCounter:		Externally incremented variable to keep track of time
Return:		0: Finished debouncing and button state is low (0)
			1: Finished debouncing and button state is high (1)
			2: Not finished debouncing, function needs to be called again
			3: Button did not change state
**************************************************************************************************/
uint8_t DebounceButton( bool bButtonState, bool* bLastButtonState, bool* bSetButtonState,
                        uint32_t* u32LastBounceTime, uint32_t u32DebounceDelay, uint32_t u32TickCounter )
{
	uint8_t returnVal;
	
	if ( bButtonState != *bLastButtonState )
	{
		*u32LastBounceTime = u32TickCounter;
	}
	
	// delay + signal time to ensure the previous signal has finished sending
	if ( ( u32TickCounter - *u32LastBounceTime ) > ( u32DebounceDelay ) )
	{
		if ( bButtonState != *bSetButtonState )
		{
			*bSetButtonState = bButtonState;
			
			if ( *bLastButtonState )
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
	
	*bLastButtonState = bButtonState;
	
	return returnVal;
}


/**************************************************************************************************
Function:	Debounce switch
Purpose:	Debouncing of a mechanical switch
Requirements:	stdbool.h
Arguments:	bSwitchState:		Current state of the switch-pin
			bLastSwitchState:	State of the switch-pin the last time this function was called
			bSetSwitchState:	Currently set switch-state (after debouncing)
			u32LastBounceTime:	Time the switch last changed its state
			u32DebounceDelay:	Delay until no state changes means no further bounces
			u32TickCounter:		Externally incremented variable to keep track of time
Return:		0: Finished debouncing and switch state is low (0)
			1: Finished debouncing and switch state is high (1)
			2: Not finished debouncing, function needs to be called again
**************************************************************************************************/
uint8_t DebounceSwitch( bool bSwitchState, bool* bLastSwitchState, bool* bSetSwitchState,
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
	
	while ( gstWasserwanneFlags.Start_F || gstWasserwanneFlags.Active_F )
	{
		TOGGLE_WASSERWANNE_BUSY_LED_BIT();
	}
}


/**************************************************************************************************
Function:	Check override-activate
Purpose:	Checks the override-activate-bit for changes and sets flags accordingly
Requirements:	stdbool.h
Arguments:	--
Return:		bool depending on state of override-activate-bit
**************************************************************************************************/
bool CheckOverrideActivate( void )
{
	bool returnVal = 0;
	
	// Sensor-switch debouncing (according to Arduino sample project)
	static uint32_t u32LastBounceTime = 0;
	static bool bLastOverrideActivateState = OVERRIDE_ACTIVATE_ON_STATE;
	static bool bSetOverrideActivateState = ~OVERRIDE_ACTIVATE_ON_STATE;
	
	bool bOverrideActivateState;
	
	if ( OVERRIDE_ACTIVATE_PINX & OVERRIDE_ACTIVATE_BIT )
	{
		bOverrideActivateState = 1;
	}
	else
	{
		bOverrideActivateState = 0;
	}
	
	uint8_t u8Debounce = DebounceSwitch( bOverrideActivateState, &bLastOverrideActivateState,
	                                     &bSetOverrideActivateState, &u32LastBounceTime, OVERRIDE_DEBOUNCE_DELAY_MS,
	                                     gstWasserwanneData.u32Ticks );
	                                     
	if ( u8Debounce < 2 )
	{
		if ( u8Debounce == OVERRIDE_ACTIVATE_ON_STATE )
		{
			gstWasserwanneFlags.Override_Active_F = 1;
			returnVal = 1;
		}
		else
		{
			gstWasserwanneFlags.Override_Active_F = 0;
			returnVal = 0;
		}
	}
	
	return returnVal;
}


/**************************************************************************************************
Function:	Check override-switch
Purpose:	Checks the override-bit for changes and sets flags accordingly
Requirements:	stdbool.h
Arguments:	--
Return:		--
**************************************************************************************************/
void CheckOverride( void )
{
	// Sensor-switch debouncing (according to Arduino sample project)
	static uint32_t u32LastBounceTime = 0;
	static bool bLastOverrideState = OVERRIDE_ON_STATE;
	static bool bSetOverrideState = ~OVERRIDE_ON_STATE;
	
	bool bOverrideState;
	
	if ( OVERRIDE_PINX & OVERRIDE_BIT )
	{
		bOverrideState = 1;
	}
	else
	{
		bOverrideState = 0;
	}
	
	uint8_t u8Debounce = DebounceSwitch( bOverrideState, &bLastOverrideState,
	                                     &bSetOverrideState, &u32LastBounceTime, OVERRIDE_DEBOUNCE_DELAY_MS,
	                                     gstWasserwanneData.u32Ticks );
	                                     
	if ( u8Debounce < 2 )
	{
		if ( u8Debounce == OVERRIDE_ON_STATE && !gstWasserwanneFlags.Valve_State_F )
		{
			gstWasserwanneFlags.Valve_On_F = 1;
			gstWasserwanneFlags.Valve_Off_F = 0;
			
			gstWasserwanneFlags.Start_F = 1;
			
		}
		else if ( u8Debounce != OVERRIDE_ON_STATE && gstWasserwanneFlags.Valve_State_F )
		{
			gstWasserwanneFlags.Valve_On_F = 0;
			gstWasserwanneFlags.Valve_Off_F = 1;
			
			gstWasserwanneFlags.Start_F = 1;
		}
		
	}
}

#endif