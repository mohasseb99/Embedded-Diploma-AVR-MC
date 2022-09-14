/*[FILE NAME]   : pwm.c
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 11, 2021
 *[DESCRIPTION] : this file contain the source file of the pwm
 */

#include"pwm.h"
#include<avr/io.h>
#include"std_types.h"
/*
 * F_CPU = 1 Mhz
 * F_TIMER = F_CPU / (256*8) = 488.28125 = 500 Hz
 * */
void PWM_Timer0_Start(uint8 duty_cycle){
	DDRB|=(1<<3); /*To make the oc0 pin an output pin*/
	TCNT0=0; // clear the timer register
	/*
	 * duty_cycle is char in type so it will be from 0 to 255 when we divide it by 100 it will take
	 * only the intger part
	 * ex: duty_cycle = 50
	 *   --> so 50/100 = 0.5 it will take the zero and that's not what we need to de
	 * with casting :
	 * same example :
	 *   --> 50 will be 50.0 and that's a float number when we divide it by 100 , it will be 0.5 not 0.0
	 *       and that's what we need .. and then we made casting back to the uint8
	 * */
	OCR0=(uint8)(((float32)duty_cycle/100)*255);
	TCCR0=(1<<WGM01)|(1<<WGM00)|(1<<COM01)|(1<<CS01)|(1<<CS00);


}
