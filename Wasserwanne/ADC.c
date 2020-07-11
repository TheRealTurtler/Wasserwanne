/*
 * ADC.c
 *
 * Created: 26.06.2020 18:19:09
 *  Author: Michael
 */

#include "ADC.h"

#ifdef ADC_ENABLED
/**************************************************************************************************
Function:	Read ADC
Purpose:	Reads the analogue voltage on speciefied pin, returns it and disables the ADC again
Requirements:	util/delay.h
Arguments:	u8Channel:	Channel to read from
Return:		uint16_t Voltage
**************************************************************************************************/
uint16_t ReadADC( uint8_t u8Channel )
{
	// Set Vcc as reference voltage and select channel
#if defined(__AVR_ATmega2560__)
	ADMUX = u8Channel | _BV( REFS0 );
#elif defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny84A__)
	ADMUX = u8Channel;
#else
#error "No preset for ADMUX register available"
#endif
	
	// Set Prescaler between 50 and 200 kHz -> 1 MHz / 8 = 125 kHz
#if F_CPU == 1000000UL
	ADCSRA = _BV( ADPS1 ) | _BV( ADPS0 );
#elif F_CPU == 16000000UL
	ADCSRA = _BV( ADPS2 ) | _BV( ADPS1 ) | _BV( ADPS0 );
#else
#error "No preset for ADCSRA register available"
#endif
	
	// Enable ADC
	ADCSRA |= _BV( ADEN );
	
	// Start conversion
	ADCSRA |= _BV( ADSC );
	
	while ( ADCSRA & _BV( ADSC ) )
	{
		_delay_us( 10 );
	}
	
	// Disable ADC
	ADCSRA &= ~_BV( ADEN );
	
	return ADC;
}


/**************************************************************************************************
Function:	Initialize fast ADC
Purpose:	Initializes the ADC for the specified port. To read the ADC value, call ReadFastADC();
Requirements:	--
Arguments:	u8Channel:	Channel to read from
Return:		--
**************************************************************************************************/
void InitFastADC( uint8_t u8Channel )
{
	// Disable ADC
	ADCSRA &= ~_BV( ADEN );
	
	// Set Vcc as reference voltage and select channel
#if defined(__AVR_ATmega2560__)
	ADMUX = u8Channel | _BV( REFS0 );
#elif defined(__AVR_ATtiny84__) || defined(__AVR_ATtiny84A__)
	ADMUX = u8Channel;
#else
#error "No preset for ADMUX register available"
#endif
	
	// Set Prescaler between 50 and 200 kHz -> 1 MHz / 8 = 125 kHz
#if F_CPU == 1000000UL
	ADCSRA = _BV( ADPS1 ) | _BV( ADPS0 );
#elif F_CPU == 16000000UL
	ADCSRA = _BV( ADPS2 ) | _BV( ADPS1 ) | _BV( ADPS0 );
#else
#error "No preset for ADCSRA register available"
#endif
	
	// Enable ADC
	ADCSRA |= _BV( ADEN );
}


/**************************************************************************************************
Function:	Read ADC fast
Purpose:	Reads the analogue voltage and returns it.
			Has to be initialized by InitFastADC( channel );
Requirements:	--
Arguments:	--
Return:		uint16_t Voltage
**************************************************************************************************/
uint16_t ReadFastADC( void )
{
	// Start conversion
	ADCSRA |= _BV( ADSC );
	
	while ( ADCSRA & _BV( ADSC ) )
	{
		_delay_us( 10 );
	}
	
	return ADC;
}
#endif