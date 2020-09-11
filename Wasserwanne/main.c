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

#ifdef ADC_ENABLED
const uint16_t cu16ADCMin = ( uint16_t )( ADC_MIN * 1023.0f / ADC_REF );
const uint16_t cu16ADCReactivate = ( uint16_t )( ADC_REACTIVATE * 1023.0f / ADC_REF );
#endif

// #define TICK_1S_EVENT		0x04
// #define TICK_100MS_EVENT	0x20
// #define TICK_FAST_EVENT		0x10

#ifdef TIMER0_ENABLED
// *******************
ISR( TIM0_COMPA_vect ) // 250*/sec
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
		
#ifdef UART_ENABLED
		UART_RX_Check(); // Polling
#endif
		
	}
	
};
#endif

#ifdef TIMER0_ENABLED
/************************************************************
Function:	Initialize timer 0
Purpose:	Starts timer 0 with a frequency of 250,00 Hz
			-> 4,00 ms cycle time
************************************************************/
static void InitTimer0( void )
{
	TCNT0 = 0;
	OCR0A = 250;
	TCCR0A = _BV( WGM01 ); 
	TCCR0B = _BV( CS02 ); 
	TIMSK0 = _BV( OCIE0A );
 }
#endif


// *****************
int main( void )
// *****************
{
#ifdef UART_ENABLED
	uart_init( UART_BAUD_SELECT_DOUBLE_SPEED( 115200UL, F_CPU ) );
	//uart_init(UART_BAUD_SELECT( 9600UL, F_CPU) );
	ResetRxBuff();
	
	gstFlags.Geschwaetzig = 1;
#endif
	
#ifndef I2C_ENABLED
	wdt_reset();
	wdt_enable( WDTO_1S ); //Totmannknopf
	wdt_reset();
#endif
	
	
	
#ifdef I2C_ENABLED
	i2c_init();
#endif
	
#ifdef LCD_ENABLED
	uint8_t u8LCDprevSize = 0;
	
	lcd_init();
	
	strcpy_P( gcaStr, PSTR( "Tick:" ) );
	lcd_print( ( unsigned char * )gcaStr );
	
	strcpy_P( gcaStr, PSTR( "ADC:" ) );
	lcd_printxy( 1, 2, ( unsigned char * )gcaStr );
	
	strcpy_P( gcaStr, PSTR( "V" ) );
	lcd_printxy( 12, 2, ( unsigned char * )gcaStr );
#endif
	
#ifdef ADC_ENABLED
	//InitFastADC( 7 );
#endif
	
#ifdef WASSERWANNE_ENABLED
	InitWasserwanne();
	
	bool bLastStableSwitchState = false;
#endif
	
#ifdef TIMER0_ENABLED
	gu32Ticks = 0;
	
	InitTimer0();
#endif
	
	sei();  // Interrupt einschalten
	
	while ( 1 ) /* loop forever */
	{
		wdt_reset();
		
#ifdef WASSERWANNE_ENABLED
		
#ifdef ADC_ENABLED
		if ( gstWasserwanneFlags.ADC_F )
		{
			gstWasserwanneFlags.ADC_F = 0;
			
			uint16_t u16ADCValue = ReadADC( 7 );
			
			if ( u16ADCValue <= cu16ADCMin )
			{
				gstWasserwanneFlags.Power_Low_F = 1;
				
				if ( gstWasserwanneFlags.Valve_State_F == 1 )
				{
					CloseValve();
				}
			}
			else if ( u16ADCValue >= cu16ADCReactivate )
			{
				gstWasserwanneFlags.Power_Low_F = 0;
			}
		}
		
		if ( !gstWasserwanneFlags.Power_Low_F )
		{
			uint8_t u8OverActState = CheckOverrideActivate( &bLastStableSwitchState );
			if ( u8OverActState == 1 )
			{
				CheckOverride();
			}
			else if ( u8OverActState == 0 )
			{
				CheckWaterSensor();
			}
		}
#else
		if ( CheckOverrideActivate( &bLastStableSwitchState ) )
		{
			CheckOverride();
		}
		else
		{
			CheckWaterSensor();
		}
#endif
		
#endif
		
#ifdef TIMER0_ENABLED
// 		if ( gstFlags.TICK_100MS__Flag ) // Ask Flag == TRUE
// 		{
// 			gstFlags.TICK_100MS__Flag = 0;
//
// 		};

		if ( gstFlags.TickEvent_Flag ) // ?-->1
		{
			gstFlags.TickEvent_Flag = 0; // clear Flag
			
			//cli();
			//PORTA = _BV(6);
			//sei();
			
#ifdef LCD_ENABLED
			cli();
			
			lcd_printxyCRLF( 7, 1, ( unsigned char * )ULongToNumStr( gu32Ticks ) );
			
			FloatToNumStr( ( ( float )ReadADC( 7 ) / 1023.0f ) * 5.0f );
			//UIntToNumStr( ReadFastADC() );
			
			lcd_printxy_overwrite( 6, 2, ( unsigned char * )gcaNumStr, u8LCDprevSize );
			
			u8LCDprevSize = strlen( gcaNumStr );
			
			sei();
#endif
			
#ifdef UART_ENABLED
			if ( gstFlags.Geschwaetzig )
			{
				uart_puts_p( PSTR( "Tick: " ) );
				uart_puts( ULongToNumStr( gu32Ticks ) );
				CRLF();
				
#ifdef WASSERWANNE_DEBUG_ENABLED
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
				
				uart_puts_p( PSTR( "\tOverride: " ) );
				
				if ( gstWasserwanneFlags.Override_Active_F )
				{
					uart_puts_p( PSTR( "ACTIVE" ) );
				}
				else
				{
					uart_puts_p( PSTR( "INACTIVE" ) );
				}
				
				CRLF();
				
				uart_puts_p( PSTR( "\tDebug: " ) );
				uart_puts( ULongToNumStr( gstWasserwanneDebug.u8Debug ) );
				
				CRLF();
				
				CRLF();
#endif
			};
#endif
			
		};
#endif
		
#ifdef UART_ENABLED
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

