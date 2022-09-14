/*[FILE NAME]   : lcd.C
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: Sep 29, 2021
 *[DESCRIPTION] : this file contain the source file of the LCD driver
 */
#include"lcd.h"
#include"gpio.h"
#include<util/delay.h>
#include<stdlib.h>

/*
 * This function used when we want to send a coomand*/
void LCD_sendCommand(uint8 command){
#if(LCD_DATA_BITS_MODE == 4)
	uint8 lcd_port_value=0;
#endif
	/* clear register select pin*/
	GPIO_writePin(LCD_RS_PORT_ID,LCD_RS_PIN_ID,LOGIC_LOW);
	/* clear read write pin because we want to send a coomand so we need to write*/
	GPIO_writePin(LCD_RW_PORT_ID,LCD_RW_PIN_ID,LOGIC_LOW);
	/*delay 1 microsecond because tas = 50 nanoseconds*/
	_delay_ms(1);
	/*set the enable pin */
	GPIO_writePin(LCD_ENABLE_PORT_ID,LCD_ENABLE_PIN_ID,LOGIC_HIGH);
	/*delay 1 microsecond because tpw - tdsw = 290 - 100 = 190 nanoseconds*/
	_delay_ms(1);
	/*incase of 4bits mode, we need to send the 4-high bits first then the 4-low bits but we need to take care of the connection
	 * of the 4pins data bus because the 4-pins data bus can connect from pin0-->pin3 or from pin4-->pin7 so, we need make
	 * a mask based on the connection type*/
#if(LCD_DATA_BITS_MODE == 4)
	/*we need to save the 4-bits values of the lcd data port that are not connected to the lcd*/
	lcd_port_value=GPIO_readPort(LCD_DATA_PORT_ID);
#ifdef LCD_LAST_PORT_PINS
	lcd_port_value=( lcd_port_value & 0x0F )|( command & 0xF0 );
#else
	lcd_port_value=( lcd_port_value & 0xF0 )|( ( command & 0xF0 )>>4);
#endif
	/*send the command to port that is connected to the data bus*/
	GPIO_writePort(LCD_DATA_PORT_ID,lcd_port_value);
	/*delay 1 microsecond because tdse=100 nanoseconds*/
	_delay_ms(1);
	/*to read the command the enable pin must be zero again*/
	GPIO_writePin(LCD_ENABLE_PORT_ID,LCD_ENABLE_PIN_ID,LOGIC_LOW); /*very important instructuon*/
	/*delay 1 microsecond beacuse tdh = 1 nanosecond*/
	_delay_ms(1);

	/* WHY make the enable pin high again ??
	 * ---> to read the lower 4-bits of the character*/
	GPIO_writePin(LCD_ENABLE_PORT_ID,LCD_ENABLE_PIN_ID,LOGIC_HIGH); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */

	/*we need to save the 4-bits values of the lcd data port that are not connected to the lcd
	 * why we do this twice??
	 * --> during this few clock cycle the 4-bits could vary so we need to get the updated values of the 4-bits*/
	lcd_port_value=GPIO_readPort(LCD_DATA_PORT_ID);
#ifdef LCD_LAST_PORT_PINS
	lcd_port_value=( lcd_port_value & 0x0F )|(( command & 0x0F )<<4);
#else
	lcd_port_value=( lcd_port_value & 0xF0 )|( command & 0x0F );
#endif
	/*send the command to port that is connected to the data bus*/
	GPIO_writePort(LCD_DATA_PORT_ID,lcd_port_value);
	/*delay 1 microsecond because tdse=100 nanoseconds*/
	_delay_ms(1);
	/*to read the command the enable pin must be zero again*/
	GPIO_writePin(LCD_ENABLE_PORT_ID,LCD_ENABLE_PIN_ID,LOGIC_LOW); /*very important instructuon*/
	/*delay 1 microsecond beacuse tdh = 1 nanosecond*/
	_delay_ms(1);
#elif(LCD_DATA_BITS_MODE == 8)
	/*send the command to port that is connected to the data bus*/
	GPIO_writePort(LCD_DATA_PORT_ID,command);
	/*delay 1 microsecond because tdse=100 nanoseconds*/
	_delay_ms(1);
	/*to read the command the enable pin must be zero again*/
	GPIO_writePin(LCD_ENABLE_PORT_ID,LCD_ENABLE_PIN_ID,LOGIC_LOW); /*very important instructuon*/
	/*delay 1 microsecond beacuse tdh = 1 nanosecond*/
	_delay_ms(1);
#endif
}


/*
 * this function to play the lcd
 * */

void LCD_init(void){
	/*set rs pin to be output pin*/
	GPIO_setupPinDirection(LCD_RS_PORT_ID,LCD_RS_PIN_ID,PIN_OUTPUT);
	/*set RW pin to be output pin*/
	GPIO_setupPinDirection(LCD_RW_PORT_ID,LCD_RW_PIN_ID,PIN_OUTPUT);
	/*set ENABLE pin to be output pin*/
	GPIO_setupPinDirection(LCD_ENABLE_PORT_ID,LCD_ENABLE_PIN_ID,PIN_OUTPUT);

#if(LCD_DATA_BITS_MODE == 4)
	/*set lcd data bus PINS to be output PINS*/
	GPIO_setupPinDirection(LCD_DATA_PORT_ID,LCD_FIRST_DATA_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID,LCD_FIRST_DATA_PIN_ID+1,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID,LCD_FIRST_DATA_PIN_ID+2,PIN_OUTPUT);
	GPIO_setupPinDirection(LCD_DATA_PORT_ID,LCD_FIRST_DATA_PIN_ID+3,PIN_OUTPUT);
	/*we will call the send command function and send 0x38 to play the lcd at the first time
	 * why 0x28 ???
	 *   we have (2-line,4-data bits) mode*/
	LCD_sendCommand(LCD_GO_TO_HOME);
	LCD_sendCommand(LCD_TWO_LINES_FOUR_BITS_MODE);

#elif(LCD_DATA_BITS_MODE == 8)
	/*set lcd data bus port to be output port*/
	GPIO_setupPortDirection(LCD_DATA_PORT_ID,PORT_OUTPUT);
	/*we will call the send command function and send 0x38 to play the lcd at the first time
	 * why 0x38 ???
	 *   we have (2-line,8-data bits) mode*/
	LCD_sendCommand(LCD_TWO_LINES_EIGHT_BITS_MODE);
#endif



	/*
	 * to turn off the cursor
	 * if we wand to turn on the cursor
	 * ----->> we will send a command ---> 0xE */
	LCD_sendCommand(LCD_CURSOR_OFF);
	/*to clear the screen*/
	LCD_sendCommand(LCD_CLEAR_COMMAND);
}


/*
 * whis function is used when we want to display character */
void LCD_displayCharacter(uint8 character){
#if(LCD_DATA_BITS_MODE == 4)
	uint8 lcd_port_value=0;
#endif
	/* set register select pin
	 * WHY LOGIC_HIGH ??
	 *  FROM THE data sheet, to send a data, the rs pin must be 1 */
	GPIO_writePin(LCD_RS_PORT_ID,LCD_RS_PIN_ID,LOGIC_HIGH);
	/* clear read write pin because we want to send a coomand so we need to write*/
	GPIO_writePin(LCD_RW_PORT_ID,LCD_RW_PIN_ID,LOGIC_LOW);
	/*delay 1 microsecond because tas = 50 nanoseconds*/
	_delay_ms(1);
	/*set the enable pin */
	GPIO_writePin(LCD_ENABLE_PORT_ID,LCD_ENABLE_PIN_ID,LOGIC_HIGH);
	/*delay 1 microsecond because tpw - tdsw = 290 - 100 = 190 nanoseconds*/
	_delay_ms(1);
	/*incase of 4bits mode, we need to send the 4-high bits first then the 4-low bits but we need to take care of the connection
	 * of the 4pins data bus because the 4-pins data bus can connect from pin0-->pin3 or from pin4-->pin7 so, we need make
	 * a mask based on the connection type*/
#if(LCD_DATA_BITS_MODE == 4)
	/*we need to save the 4-bits values of the lcd data port that are not connected to the lcd*/
	lcd_port_value=GPIO_readPort(LCD_DATA_PORT_ID);
#ifdef LCD_LAST_PORT_PINS
	lcd_port_value=( lcd_port_value & 0x0F )|( character & 0xF0 );
#else
	lcd_port_value=( lcd_port_value & 0xF0 )|( ( character & 0xF0 )>>4);
#endif
	/*send the command to port that is connected to the data bus*/
	GPIO_writePort(LCD_DATA_PORT_ID,lcd_port_value);
	/*delay 1 microsecond because tdsw=100 nanoseconds*/
	_delay_ms(1);
	/*to read the command the enable pin must be zero again*/
	GPIO_writePin(LCD_ENABLE_PORT_ID,LCD_ENABLE_PIN_ID,LOGIC_LOW); /*very important instructuon*/
	/*delay 1 microsecond beacuse th = 1 nanosecond*/
	_delay_ms(1);
	/* WHY make the enable pin high again ??
	 * ---> to read the lower 4-bits of the character*/
	GPIO_writePin(LCD_ENABLE_PORT_ID,LCD_ENABLE_PIN_ID,LOGIC_HIGH); /* Enable LCD E=1 */
	_delay_ms(1); /* delay for processing Tpw - Tdws = 190ns */

	/*we need to save the 4-bits values of the lcd data port that are not connected to the lcd
	 * why we do this twice??
	 * --> during this few clock cycle the 4-bits could vary so we need to get the updated values of the 4-bits*/
	lcd_port_value=GPIO_readPort(LCD_DATA_PORT_ID);
#ifdef LCD_LAST_PORT_PINS
	lcd_port_value=( lcd_port_value & 0x0F )|(( character & 0x0F )<<4);
#else
	lcd_port_value=( lcd_port_value & 0xF0 )|( character & 0x0F );
#endif
	/*send the command to port that is connected to the data bus*/
	GPIO_writePort(LCD_DATA_PORT_ID,lcd_port_value);
	/*delay 1 microsecond because tdse=100 nanoseconds*/
	_delay_ms(1);
	/*to read the command the enable pin must be zero again*/
	GPIO_writePin(LCD_ENABLE_PORT_ID,LCD_ENABLE_PIN_ID,LOGIC_LOW); /*very important instructuon*/
	/*delay 1 microsecond beacuse tdh = 1 nanosecond*/
	_delay_ms(1);
#elif(LCD_DATA_BITS_MODE == 8)
	/*send the command to port that is connected to the data bus*/
	GPIO_writePort(LCD_DATA_PORT_ID,character);
	/*delay 1 microsecond because tdse=100 nanoseconds*/
	_delay_ms(1);
	/*to read the command the enable pin must be zero again*/
	GPIO_writePin(LCD_ENABLE_PORT_ID,LCD_ENABLE_PIN_ID,LOGIC_LOW); /*very important instructuon*/
	/*delay 1 microsecond beacuse tdh = 1 nanosecond*/
	_delay_ms(1);
#endif
}



/*
 * this function is used to display string
 * whis function uses the LCD_displayCharacter() function to display all the characters in the string
 * C course Recall :
 * -----> to loop through a string we need pointer to character and increment the address to access the following character*/
void LCD_displayString(const uint8 *string){
	while( (*string) != '\0' ){
		LCD_displayCharacter(*string);
		string++;
	}
}


/*
 * this function used to clear the LCD*/

void LCD_clearScreen(void){
	LCD_sendCommand(LCD_CLEAR_COMMAND);

}

/*
 * to move the cursor to any location*/
void LCD_moveCursor(uint8 row,uint8 col){
	uint8 lcd_memory_address;
	switch(row){
	/*
	 * first row address start with 0x00*/
	case 0:
		lcd_memory_address=col+FIRST_ROW_ADDRESS;
		break;
		/*
		 * second row address start with 0x40*/
	case 1:
		lcd_memory_address=col+SECOND_ROW_ADDRESS;
		break;
		/*
		 * third row address start with 0x10*/
	case 2:
		lcd_memory_address=col+THIRD_ROW_ADDRESS;
		break;
		/*
		 * fourth row address start with 0x50*/
	case 3:
		lcd_memory_address=col+FOURTH_ROW_ADDRESS;
		break;
	}
	LCD_sendCommand(lcd_memory_address | LCD_SET_CURSOR_LOCATION);
}

/*
 * this function display string at any row and col u want*/

void LCD_displayStringRowColumn(uint8 row,uint8 col,uint8 *string){
	/*to set the cursor we call the movecursor function*/
	LCD_moveCursor(row,col);
	/*to display the string we call the displaystring function
	 * note :
	 * -----> we send the address not the * ( take care of this )
	 * why is that ??
	 * ----> the display string receive what we sent in pointer to character so must send an address*/
	LCD_displayString(string);

}


/*
 * to display decimal number which we don't know its value like a value from a sensor*/
void LCD_intgerToString(int data){
	/*uninitialized array with size 16 because the lcd is 16 in cols*/
	char buff[16];
	/*this function will convert the data to the ascii ( every number inside the data will be converted to
	 * the ascii belongs to it )*/
	/*ex : data = 1234
	 * --> so buff[0]='1'
	 * --> so buff[1]='2'
	 * --> so buff[2]='3'
	 * --> so buff[3]='4'
	 * --> so buff[4]='\0'
	 * */
	itoa(data,buff,10);
	/*display the string after the conversion ended*/
	LCD_displayString((uint8 *)buff);
}
