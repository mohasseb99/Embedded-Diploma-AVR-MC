/*
 * app.c
 *
 *  Created on: Oct 17, 2021
 *      Author: Mohasseb
 */

#include "adc.h"
#include "lcd.h"
#include "lm35_sensor.h"
#include "dc_motor.h"

int main(void){
	uint8 temp;

	/* make config structure to initialize ADC to internal volt and pre scaler 8 */
	ADC_ConfigType config = {INTERNAL, PRE_SCALER_8};

	/* initialization of LCD, Dc Motor and ADC */
	ADC_init(&config);
	LCD_init();
	DcMotor_init();

	LCD_displayStringRowColumn(0,2,"Fan is ");
	LCD_displayStringRowColumn(1,2,"Temp =    ");
	while(1){
		temp = LM35_getTemperature();

		if(temp < 30){
			LCD_displayStringRowColumn(0,10,"OFF.");
			LCD_moveCursor(1, 8);
			LCD_intgerToString(temp);
			LCD_displayString("C   ");
			DcMotor_rotate(STOP, 0);
		}
		else if(temp < 60){
			LCD_displayStringRowColumn(0,10,"ON. ");
			LCD_moveCursor(1, 8);
			LCD_intgerToString(temp);
			LCD_displayString("C   ");
			DcMotor_rotate(CW, 25);
		}
		else if(temp < 90){
			LCD_displayStringRowColumn(0,10,"ON. ");
			LCD_moveCursor(1, 8);
			LCD_intgerToString(temp);
			LCD_displayString("C   ");
			DcMotor_rotate(CW, 50);
		}
		else if(temp < 120){
			LCD_displayStringRowColumn(0,10,"ON. ");
			LCD_moveCursor(1, 8);
			LCD_intgerToString(temp);
			LCD_displayString("C   ");
			DcMotor_rotate(CW, 75);
		}
		else{
			LCD_displayStringRowColumn(0,10,"ON. ");
			LCD_moveCursor(1, 8);
			LCD_intgerToString(temp);
			LCD_displayString("C   ");
			DcMotor_rotate(CW, 100);
		}
	}

}
