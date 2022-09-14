/*[FILE NAME]   : buzzer.c
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 28, 2021
 *[DESCRIPTION] : Header file for buzzer sensor
 */

#include "buzzer.h"

#include"gpio.h"


void BUZZER_init(){
	/*Making the buzzer pin an output pin*/
	GPIO_setupPinDirection(BUZZER_PORT_ID,BUZZER_PIN_ID,PIN_OUTPUT);
}

void BUZZER_on(){
	/*turn on the buzzer*/
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_HIGH);
}

void BUZZER_off(){
	/*turn off the buzzer*/
	GPIO_writePin(BUZZER_PORT_ID,BUZZER_PIN_ID,LOGIC_LOW);
}
