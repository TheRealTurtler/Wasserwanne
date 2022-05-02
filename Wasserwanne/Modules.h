/*
 * Modules.h
 *
 * Created: 04.04.2020 00:36:59
 *  Author: Michael
 */


#ifndef MODULES_H_
#define MODULES_H_

#ifndef F_CPU
#define F_CPU 1000000UL
#endif

// ================ Modules ===============================

//#define TIMER0_ENABLED
//#define UART_ENABLED
//#define TEXT_SERVICE_ENABLED
//#define I2C_ENABLED
//#define LCD_ENABLED

#define WASSERWANNE_ENABLED
#define ADC_ENABLED
//#define WASSERWANNE_HEARTBEAT_ENABLED
//#define WASSERWANNE_DEBUG_ENABLED

// ================ Dependencies ==========================

#ifdef UART_ENABLED

#ifndef TIMER0_ENABLED
#define TIMER0_ENABLED
#endif

#ifndef TEXT_SERVICE_ENABLED
#define TEXT_SERVICE_ENABLED
#endif

#endif	// UART_ENABLED


#ifdef LCD_ENABLED
#ifndef I2C_ENABLED
#define I2C_ENABLED
#endif

#ifndef TEXT_SERVICE_ENABLED
#define TEXT_SERVICE_ENABLED
#endif

#endif	// LCD_ENABLED

// ========================================================

#endif /* MODULES_H_ */