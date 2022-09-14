/*
 * app.c
 *
 *  Created on: Oct 27, 2021
 *      Author: Mohasseb
 */

#include"std_types.h"
#include"lcd.h"
#include"Ultrasonic.h"
#include <avr/io.h> /* To use the SREG register */

int main(){
	uint16 distance;
	LCD_init();
	LCD_clearScreen();

	SREG |= (1<<7);  /* to enable ibit */
	LCD_displayString("Distance=");
	while(1){
		Ultrasonic_init();
		distance = Ultrasonic_readDistance();
		LCD_moveCursor(0, 9);
		LCD_intgerToString(distance);
		LCD_displayString("CM        ");
	}
}
