

// Verison 05.02.2019
/*
*ERMER Christof**
*/

/* HYSTORY
Befehle: Viele  CheckOrder() Druckausgaben produziert Neustart
Grund ??  Lösung:	Auslagerung in Main via Flags.
strtok_r  raus --> strtok rein
*/

/*
BEFEHLE
Befelsform:
Kommando,u16Val-1,u16Val-2<13>  <13>  //<13> = CR = \r = 13
Bsp:
Motor,123,34<CR>
Ping<CR>
GetSW>CR>

Diese Datei dient zum einfachen Auffinden und Zufügen der Befehle.
Deshalb wurden die Befehle in diese Datei ausgelagert.
*/
#include "main.h"

/*
EIne Besonderheit des AVR Mikrocontrollers  mitHARVARD Architektur ist die unterschiedliche Behandlung von Zeichen=Strings,
die entweder im Flash-Speicher  Stehen, und somit von dort gelesen werden,
oder die im RAM stehen,
Es gibt also also C-Funktionen die Strings behandeln doppelt.
einmal mit "Programm Memory Handling" und
quasi Normal: ohne besonderen, dafür RAM-Verbrauchende Stringbehandlung.

Hier liegt mustergülitig ein "const cahr" string , d
der aber im "Programm-Flash" abgespeichert wird und nicht im "RAM".
*/

//	\r return
//	\r\n return, line fedd
//	\r\n \t  return, line fedd, tabulator

//Prototype
// GLOBALE VARIABLEN
// CONSTANT ARRAYs IN .TEXT = ROM ARREA   .DATA IST SRAM !! .BSS SRAM
// const char MCA_DELIMTERSTR[] PROGMEM = { ',',0 };


#ifdef UART_USE_ENABLED
char gcaStr[STRMAX + 1]; //+1 for the 0
char gcaNumStr[NUMMAX + 1]; //+1 for the 0
char gcaRxStr[RX_STRMAX + 1]; //+1 for the 0
char * gpcRxPars;
COMMAND_TYPE gsCmd;  // MIT OrderFlag
//  gsCmd.CREventFlag

//PROTO
void TokensRXStr(char *pcStrOrder); //von gcaRxStr

// *********************************
void CheckOrder(void) //von gcaRxStr
// *********************************
{
	//Input von der Seriellen Schnittstlle, in der Form:  "BEFEHL[,WERT]" BSP: "SFR,1000"
	//========================
	TokensRXStr( gcaRxStr );
	//========================
	ResetRxBuff();
	//========================

	if(!strcasecmp_P( gsCmd.ucaCmd, PSTR("DEVICE")))
	{
		CRLF();
		uart_puts_p( PSTR("Arduino UNO") );
		CRLF();
		CRLF();
		return;
	};

	if(!strcasecmp_P( gsCmd.ucaCmd, PSTR("VV")))
	{
		if( gsCmd.fCmdVal_1 )
		{
			gstFlags.Geschwaetzig = 1;
			CRLF();
			uart_puts_p( PSTR("\t** Bin wieder Geschwaetzig **") );
			CRLF();
			CRLF();
		}
		else
		{
			gstFlags.Geschwaetzig = 0;
			CRLF();
			uart_puts_p( PSTR("\t** ich halte die Klappe **") );
			CRLF();
			CRLF();
		};
		return;
	};

	if(!strcasecmp_P( gsCmd.ucaCmd, PSTR("VO")))
	{
		gstFlags.Geschwaetzig = 1;
		CRLF();
		uart_puts_p( PSTR("\t** Bin wieder Geschwaetzig **") );
		CRLF();
		CRLF();
		return;
	};

	if(!strcasecmp_P( gsCmd.ucaCmd, PSTR("VF")))
	{
		gstFlags.Geschwaetzig = 0;
		CRLF();
		uart_puts_p( PSTR("\t** ich halte die Klappe **") );
		CRLF();
		CRLF();
		return;
	};

	if(!strcasecmp_P( gsCmd.ucaCmd, PSTR("PING")))
	{
		CRLF();
		uart_puts_p( PSTR("\tPONG") ); //ESCAPE Sequenzen
		CRLF();
		CRLF();
		return;
	};


	if(!strcasecmp_P( gsCmd.ucaCmd, PSTR("TT"))) //TICKS anzeigen
	{
		CRLF();
		uart_puts( ULongToNumStr( gu32Ticks ) );
		CRLF();
		CRLF();
		return;
	};
}; // ORDER


// **************************
void ResetRxBuff(void)
// **************************
{
	uart_flush();
	gpcRxPars = gcaRxStr; // Set parse pointer back;
	*gpcRxPars = 0;	//clear Rx
	gsCmd.UartCREventFlag = 0;
};


//Funktionen Definition
// *******************************************************************
void UART_RX_Check(void)
/*
* Get received character from ringbuffer
* uart_getc() returns in the lower byte the received character and
* in the higher byte (bitmask) the last receive error
* UART_NO_DATA is returned when no data is available.
*/
// *******************************************************************
{
	uint16_t uiRxUart;
	if( gsCmd.UartCREventFlag == 1)  // if 1 Tu nix.
	{
		return;
	}; // Verriegle wen Job aktiv
	uiRxUart = uart_getc();//!FETCH ALLWAYS BYTE FROM RX RINGBUFF INTERRUPT!
	if ( !(uiRxUart & UART_NO_DATA) )  //0x0100
	{
//   * new data available from UART
//  * check for Frame or Overrun error
//NOW SELECT ACTION FROM ORDER
		if(gpcRxPars < (gcaRxStr + RX_STRMAX))	//ENDANSCHLAG ERREICHT ?
		{
			switch( (unsigned  char)uiRxUart  )
			{
			case 10: // LF raus=nix machen
			{
				break;
			};
			case 13: ////Check CR order
			{
				gsCmd.UartCREventFlag = 1;
				break;
			};
			default: //Accumulate
			{
				// Accumulate String only to gcaRxStr with  Parser
				*gpcRxPars++ = (unsigned  char)(uiRxUart & 0x00FF);//Append rxByte
				*gpcRxPars = 0; //ZERO carry
				break;
			};
			};//switch
		}
		else  // To much data Rx, than clear simple all
		{
			//overflow of RX clears all
			ResetRxBuff();
		};
// ENDE DER RX EMPFANGSAUSWERTUNG
	};
};


/* Diese Funktion zerlegt den Befehls-String in einzelne Module */
// *******************************************************************
void TokensRXStr(char *pcStrOrder) //von gcaRxStr
// *******************************************************************
{
	//INPUT ERWARTET INDER FOR "BEFEHL,WERT" BSP: "SFR,1000"
	const char caDelimiter[]= ",";
	char * pStrTok; // für strtok_r
//char * pcCh = &pcStrOrder[ strlen(pcStrOrder) ]; // Stelle POINTER auf \0 Ende  // kein unsigned char*
//char * 	strtok_r (char *, const char *, char **)
//char * 	strtok (char *, const char *)
//if( (pStrTok = strtok_r( pcStrOrder, caDelimiter , &pcCh)) != NULL) // Trenne 1.Token in Command
	if( (pStrTok = strtok( pcStrOrder, caDelimiter)) != NULL) // Trenne 1.Token in Command
	{
		strcpy(gsCmd.ucaCmd, pStrTok);
	}
	else
	{
		*gsCmd.ucaCmd = 0;
	};
	gsCmd.ucaCmd[ COMAMNDSIZEMAX ] = 0; //Sicherheitsterminierung COMAMNDSIZEMAX=10
//für 2 fVal Variablen nach dem Kommand
	gsCmd.fCmdVal_1 = 0;
//gsCmd.i16CmdVal_2 = 0;
//HIER NULL POINTER
	if( (pStrTok = strtok( NULL, caDelimiter)) != NULL) // Trenne 1.Token in Command
	{
		gsCmd.fCmdVal_1 = atof(pStrTok);// Und dann TO Float
		//gsCmd.i16CmdVal_1 = atol(pStrTok);
	};
//HIER NULL POINTER
	/*
	if( (pStrTok = strtok( NULL, caDelimiter)) != NULL) // Trenne 1.Token in Command
		{
		//gsCmd.f16CmdVal_2 = atof(pStrTok);// Und dann TO Float
		gsCmd.i16CmdVal_2 = atoi(pStrTok);
		};
	*/
};
#endif
