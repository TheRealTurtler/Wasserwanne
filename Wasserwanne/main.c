/*
 * Wasserwanne.c
 *
 * Created: 28.03.2020 18:34:38
 * Author : Michael
 */

#include "main.h"


// Globale Variablen
volatile uint32_t gu32Ticks;
volatile BIT_FIELD_TYPE gstFlags;

// #define TICK_1S_EVENT		0x04
// #define TICK_100MS_EVENT	0x20
// #define TICK_FAST_EVENT		0x10

// *******************
ISR( TIMER0_COMPA_vect ) // 250*/sec
// *******************
{
	gu32Ticks++;  // ~250/Sekunde
	
	if ( !( gu32Ticks % TICKS_1000MS ) ) //Modulo ==0 ?
	{
		gstFlags.TickEvent_Flag = 1;	//Set FLAG
	};
	
	if ( !( gu32Ticks % TICKS_100MS ) ) //Modulo ==0 ?
	{
		gstFlags.TICK_100MS__Flag = 1;
		
#ifdef UART_USE_ENABLED
		UART_RX_Check(); // Polling
#endif
		
	}
	
};

// *****************************
void StartTickTimer( void )
// *****************************
{
// Timer TickCounter starten im Compare Mode
// M_CPU=16^6 / 256 / 250 = {ebenso} 250!Zähle 0 bis 250-->exakt 1 SeKunde
	TCNT0 = 0;
	OCR0A = 250;//Compare Value
	TCCR0A = ( 1 << WGM01 );	//CTC Mode = Compare Timer Counter mit OCR0A
	TIMSK0 = ( 1 << OCIE0A );	//Compare Interrupt aktivieren
	TCCR0B = ( 1 << CS02 );	//Vorteiler /256
//TCCR0B = (1<< CS02) | _BV(CS00);	//Vorteiler /1024
};


// *****************
int main( void )
// *****************
{

#ifdef UART_USE_ENABLED
	uart_init( UART_BAUD_SELECT_DOUBLE_SPEED( 115200UL, F_CPU ) );
	//uart_init(UART_BAUD_SELECT( 9600UL, F_CPU) );
	ResetRxBuff();
	
	gstFlags.Geschwaetzig = 1;
#endif
	
	wdt_reset();
	wdt_enable( WDTO_1S ); //Totmannknopf
	wdt_reset();
	
	StartTickTimer();
	
#ifdef WASSERWANNE_USED
	InitWasserwanne();
#endif
	
	sei();  // Interrupt einschalten
	
	
	while ( 1 ) /* loop forever */
	{
		wdt_reset();
		
#ifdef WASSERWANNE_USED
		CheckWaterSensor();
#endif
		
// 		if ( gstFlags.TICK_100MS__Flag ) // Ask Flag == TRUE
// 		{
// 			gstFlags.TICK_100MS__Flag = 0;
//
// 		};

		if ( gstFlags.TickEvent_Flag ) // ?-->1
		{
			gstFlags.TickEvent_Flag = 0; // clear Flag
			
#ifdef UART_USE_ENABLED
			if ( gstFlags.Geschwaetzig )
			{
				uart_puts_p( PSTR( "Tick: " ) );
				uart_puts( ULongToNumStr( gu32Ticks ) );
				CRLF();
				
#ifdef WASSERWANNE_DEBUG_USED
				uart_puts_p( PSTR( "Status:" ) );
				CRLF();
				uart_puts_p( PSTR( "\tSensor: " ) );
				uart_puts( UIntToNumStr( gstWasserwanneFlags.Start_F ) );
				uart_puts( UIntToNumStr( gstWasserwanneFlags.Active_F ) );
				uart_puts( UIntToNumStr( gstWasserwanneFlags.Valve_On_F ) );
				uart_puts( UIntToNumStr( gstWasserwanneFlags.Valve_Off_F ) );
				uart_puts( UIntToNumStr( gstWasserwanneFlags.Valve_State_F ) );
				CRLF();
				
				uart_puts_p( PSTR( "\tValve: " ) );
				
				if ( gstWasserwanneFlags.Valve_State_F )
				{
					uart_puts_p( PSTR( "OPEN" ) );
				}
				else
				{
					uart_puts_p( PSTR( "CLOSED" ) );
				}
				
				CRLF();
				
				uart_puts_p( PSTR( "\tWasserwanne Ticks: " ) );
				uart_puts( ULongToNumStr( gstWasserwanneData.u32Ticks ) );
				CRLF();
				uart_puts_p( PSTR( "\tDebounce: " ) );
				uart_puts( ULongToNumStr( gstWasserwanneDebug.u8Debounce ) );
				CRLF();
				CRLF();
#endif
			};
#endif
			
		};
		
#ifdef UART_USE_ENABLED
		if ( gsCmd.UartCREventFlag ) // bearbeite Carriage Return Order
		{
			gsCmd.UartCREventFlag = 0;
			CheckOrder();
			ResetRxBuff();
		};
#endif
	}; //while
	
};
// ******** MAIN END ********

