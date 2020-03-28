
/*
Kurs
28.03.2019 
*/
// NUR wenn #define UART_USE_ENABLED gibt es weitere kleine Funktionen

#include "main.h"
#ifdef UART_USE_ENABLED
// *************
void CRLF(void)
// *************
{
uart_puts_p( PSTR("\r\n") );
};

// *************
void CR(void)
// *************
{
uart_putc(13);
};
#endif


#ifdef TEXT_SERVICE_ENABLED
char gcaStr[STRMAX + 1]; //+1 for the 0
char gcaNumStr[NUMMAX + 1]; //+1 for the 0

// *********************************************
char*  FloatToNumStr(double fVal)
// *************
{
return dtostrf(fVal, FLOAT_PRAEDIGITS_MAX, FLOAT_DIGITS_MAX, gcaNumStr ); //[-]d.ddd
};

// **********************************************
char*  IntToNumStr(int16_t lVal)
// **********************************************
{ 
return itoa(lVal, gcaNumStr, 10 );
};

char*  UIntToNumStr(int16_t lVal)
// **********************************************
{ 
return utoa(lVal, gcaNumStr, 10 );
};

// **********************************************
char*  LongToNumStr(int32_t lVal)
// **********************************************
{ 
return ltoa(lVal, gcaNumStr, 10 );
};

// **********************************************
char* ULongToNumStr(uint32_t ulVal)
// **********************************************
{ 
return ultoa(ulVal, gcaNumStr, 10 );
};

#endif  //TEXT_SERVICE_ENABLED
