
// Version 28.03.2019
#ifndef TEXT_SERVICE_HEADER
	#define  TEXT_SERVICE_HEADER

//AUSSschalten wenn nicht benötigt 
#define TEXT_SERVICE_ENABLED

#ifdef TEXT_SERVICE_ENABLED

#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <avr\pgmspace.h> 

#define RX_STRMAX		31 //+1
#define COMAMNDSIZEMAX	31 //+1

#define STRMAX 63 //+1
#define NUMMAX 31 //+1

#define FLOAT_PRAEDIGITS_MAX	5 // DIgits vor   /dtostrf(fVal,NUMMAX, DIGITS_MAX, gcaNumStr);
#define FLOAT_DIGITS_MAX		3 // DIgits nach //dtostrf(fVal,NUMMAX, DIGITS_MAX, gcaNumStr);
#define ASC_CR	13
#define ASC_LF	10

extern void CRLF(void);
extern void CR(void);
extern char* FloatToNumStr(double fVal);
extern char* UIntToNumStr(int16_t lVal);
extern char* IntToNumStr(int16_t lVal);
extern char* LongToNumStr(int32_t lVal);
extern char* ULongToNumStr(uint32_t ulVal);
extern char gcaStr[STRMAX + 1]; //+1 for the 0
extern char gcaNumStr[NUMMAX + 1]; //+1 for the 0

#endif //TEXT_SERVICE_ENABLED
#endif //TEXT_SERVICE_HEADER