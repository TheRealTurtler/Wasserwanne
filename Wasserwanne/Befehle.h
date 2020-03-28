

// Verison 05.02.2019
#ifndef BEFEHLE_HEADER
	#define  BEFEHLE_HEADER

#ifdef UART_USE_ENABLED	
//#define ASC_DELIMITER	','
//const char MCA_DELIMTERSTR[] PROGMEM = { ',',0 };
//const char MCA_DELIMITER_MSG[] PROGMEM	= ",";

// TYPEN DEKLARATION
typedef struct
	{
	unsigned char UartCREventFlag : 1;
	char ucaCmd[COMAMNDSIZEMAX + 1];
	int16_t i16CmdVal_1;	//DATENTYP der Übergeben werden kann ist 16 BIT unsigend
	float fCmdVal_1;	//DATENTYP der Übergeben werden kann ist 16 BIT unsigend
	}COMMAND_TYPE;
	//gsCmd.i16CmdVal_1

// const char MCA_DELIMITER_MSG[] PROGMEM	= ",";
// GLOBALE VARIABLEN
extern COMMAND_TYPE gsCmd;
extern unsigned char gu8BefehlEventFlag;

extern char gcaStr[STRMAX + 1]; //+1 for the 0
extern char gcaNumStr[NUMMAX + 1]; //+1 for the 0

extern char gcaRxStr[RX_STRMAX + 1]; //+1 for the 0
//extern char* gpcRxPars;
extern void ResetRxBuff(void);
extern void UART_RX_Check(void);
extern void CheckOrder(void); // von Main

#endif //UART_USE_ENABLED

#endif // BEFEHLE_HEADER