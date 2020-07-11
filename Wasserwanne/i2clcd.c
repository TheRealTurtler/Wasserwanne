/*
Michael Diedler 22.06.2020
Adjusted all communication with the HD44870 acording to its datasheet

Michael Diedler 22.06.2020
ADDED #ifdef I2C_ENABLED to easily turn off compilation of this file
ADDED #ifdef LCD_ENABLED to easily turn off compilation of this file
*/

/*****************************************************************************

 i2clcd.c - LCD over I2C library
		Designed for HD44780 based LCDs with I2C expander PCF8574X
		on Atmels AVR MCUs

 Copyright (C) 2006 Nico Eichelmann and Thomas Eichelmann

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 You can contact the authors at info@computerheld.de

*****************************************************************************/

/*
	Version 0.1
	Requires I2C-Library from Peter Fleury http://jump.to/fleury

	See i2clcd.h for description and example.
*/

#include "i2clcd.h"

#ifdef I2C_ENABLED
#ifdef LCD_ENABLED

//-	Display initialization sequence (according to HD44780 datasheet page 42, 46
void lcd_init( void )
{
	gbBacklight = true;
	
	lcd_wait_ms( 16 );					//-	Wait for more than 15ms after VDD rises to 4.5V
	lcd_write( LCD_D5 | LCD_D4 );		//-	Set interface to 8-bit
	lcd_wait_ms( 5 );					//-	Wait for more than 4.1ms
	lcd_write( LCD_D5 | LCD_D4 );		//-	Set interface to 8-bit
	lcd_wait_us( 101 );					//-	Wait for more than 100us
	lcd_write( LCD_D5 | LCD_D4 );		//-	Set interface to 8-bit
	
	lcd_write( LCD_D5 );				//-	Set interface to 4-bit
	
	//- From now on in 4-bit-Mode
	lcd_command( LCD_4BIT | LCD_2LINE | LCD_5X8 );						//-	2-Lines, 5x8-Matrix
	lcd_command( LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKINGOFF );		//- Turn display on, disable cursor, disable blinking
	lcd_command( LCD_CLEAR );											//-	Clear Screen
	lcd_command( LCD_INCREASE | LCD_DISPLAYSHIFTOFF );					//-	Entrymode (Display Shift: off, Increment Address Counter)
}


//-	Write data to i2c
void lcd_write_i2c( unsigned char value )
{
	i2c_start_wait( LCD_I2C_DEVICE + I2C_WRITE );	// Start I2C connection (write)
	i2c_write( value );								// Transfer data
	i2c_stop();										// Stop I2C connection
}


//-	Write byte to display with toggle of enable-bit
void lcd_write( unsigned char value )
{
	if ( gbBacklight )
	{
		lcd_write_i2c( value | LCD_E | LCD_BL );			//-	Set enable to high
		lcd_write_i2c( ( value | LCD_BL ) & ( ~LCD_E ) );	//-	Set enable to low
	}
	else
	{
		lcd_write_i2c( value | LCD_E );			//-	Set enable to high
		lcd_write_i2c( value & ( ~LCD_E ) );	//-	Set enable to low
	}
}


//-	Print string to cursor position
void lcd_print( unsigned char *string )
{
	unsigned char i = 0;
	while ( string[i] != 0x00 )
	{
		lcd_putchar( string[i] );
		i++;
	}
}


//-	Put char to cursor position
void lcd_putchar( unsigned char value )
{
	unsigned char lcddata;
	lcddata = value;
	lcddata &= 0xF0;				// Only select upper 4 bits
	lcddata |= LCD_RS;				// Add RS-Bit (write command)
	lcd_write( lcddata );			// Write data
	lcddata = value;
	lcddata <<= 4;					// Shift lower 4 bits into upper 4 bits
	lcddata |= LCD_RS;				// Add RS-Bit (write command)
	lcd_write( lcddata );			// Write data
}


//-	Issue a command to the display (use the defined commands above)
void lcd_command( unsigned char command )
{
	unsigned char lcddata;
	lcddata = command;
	lcddata &=  0xF0;				// Only select upper 4 bits
	lcd_write( lcddata );				// Write data
	lcddata = command;
	lcddata <<= 4;					// Shift lower 4 bits into upper 4 bits
	lcd_write( lcddata );				// Write data
	
	// Some commands need time to execute (only necessary for 100kHz I2C speed)
	lcd_wait_us( 800 );
}


//-	Go to position (x, y)
bool lcd_gotoxy( unsigned char x, unsigned char y )
{
	if ( y > LCD_LINES ) return false;					// Exit if display has not enough lines
	if ( x > LCD_COLS ) return false;					// Exit if display has not enough columns
	if ( ( y == 0 ) || ( x == 0 ) ) return false;		// Exit if line or column is 0
	
	
	if ( y == 1 )
	{
		lcd_command( LCD_D7 |  ( LCD_LINE1 + x - 1 ) );
		return true;
	}
	
#if LCD_LINES>=2
	if ( y == 2 )
	{
		lcd_command( LCD_D7 |  ( LCD_LINE2 + x - 1 ) );
		return true;
	}
#endif
#if LCD_LINES>=3
	if ( y == 3 )
	{
		lcd_command( LCD_D7 |  ( LCD_LINE3 + x - 1 ) );
		return true;
	}
#endif
#if LCD_LINES>=4
	if ( y == 4 )
	{
		lcd_command( LCD_D7 |  ( LCD_LINE4 + x - 1 ) );
		return true;
	}
#endif
	
	return false;
}


//-	Put char to position
bool lcd_putcharxy( unsigned char x, unsigned char y, unsigned char value )
{
	if ( !lcd_gotoxy( x, y ) ) return false;
	lcd_putchar( value );
	
	return true;
}


//-	Print string to position (x, y, string)
bool lcd_printxy( unsigned char x, unsigned char y, unsigned char *string )
{
	unsigned char i = 0;
	
	if ( !lcd_gotoxy( x, y ) ) return false;
	
	while ( string[i] != 0x00 )
	{
		lcd_putchar( string[i] );
		
		i++;
	}
	
	return true;
}


//-	Print string to position (x, y, string)
bool lcd_printxy_overwrite( unsigned char x, unsigned char y, unsigned char *string, uint8_t prevSize )
{
	unsigned char i = 0;
	
	if ( !lcd_gotoxy( x, y ) ) return false;
	
	while ( string[i] != 0x00 )
	{
		lcd_putchar( string[i] );
		
		i++;
	}
	
	uint8_t strSize = strlen( ( const char* )string );
	
	if ( strSize < prevSize )
	{
		for ( uint8_t i = 0; i < prevSize - strSize; i++ )
		{
			lcd_putchar( ' ' );
		}
	}
	
	return true;
}


//-	Print string to position (If string is longer than LCD_COLS overwrite first chars)(x, y, string)
bool lcd_printxyCR( unsigned char x, unsigned char y, unsigned char *string )
{
	unsigned char i = 0;
	
	if ( !lcd_gotoxy( x, y ) ) return false;
	
	while ( string[i] != 0x00 )
	{
		lcd_putchar( string[i] );
		x++;
		if ( x > LCD_COLS )
		{
			x = 1;
			if ( !lcd_gotoxy( x, y ) ) return false;
		}
		
		i++;
	}
	
	return true;
}


//-	Print string to position (If string is longer than LCD_ROWS continue in next line)(x, y, string)
bool lcd_printxyCRLF( unsigned char x, unsigned char y, unsigned char *string )
{
	unsigned char i = 0;
	
	if ( !lcd_gotoxy( x, y ) ) return false;
	
	while ( string[i] != 0x00 )
	{
		lcd_putchar( string[i] );
		x++;
		if ( x > LCD_COLS )
		{
			y++;
			x = 1;
			if ( !lcd_gotoxy( x, y ) ) return false;
		}
		if ( y > LCD_LINES )
		{
			y = 1;
			if ( !lcd_gotoxy( x, y ) ) return false;
		}
		
		i++;
	}
	
	return true;
}


//-	Wait some microseconds
void lcd_wait_us( unsigned short us )
{
	unsigned short i;
	for ( i = 0; i < us; i++ )
	{
		wait1us;
	}
}

//-	Wait some milliseconds
void lcd_wait_ms( unsigned short ms )
{
	unsigned short i;
	for ( i = 0; i < ms; i++ )
	{
		wait1ms;
	}
}


//- Set backlight on/off
void lcd_backlight( bool bl )
{
	gbBacklight = bl;
	lcd_write_i2c( LCD_E | LCD_BL );
}


#endif	// LCD_ENABLED
#endif	// I2C_ENABLED