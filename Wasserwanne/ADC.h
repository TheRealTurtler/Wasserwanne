/*
 * ADC.h
 *
 * Created: 26.06.2020 18:19:21
 *  Author: Michael
 */ 


#ifndef ADC_H_
#define ADC_H_

#include "Modules.h"

#ifdef ADC_ENABLED

#include <avr/io.h>
#include <util/delay.h>

#ifndef _BV
#define _BV(bit) (1 << bit)
#endif

uint16_t ReadADC( uint8_t u8Channel );

void InitFastADC( uint8_t u8Channel );

uint16_t ReadFastADC( void );

#endif	// ADC_ENABLED

#endif /* ADC_H_ */