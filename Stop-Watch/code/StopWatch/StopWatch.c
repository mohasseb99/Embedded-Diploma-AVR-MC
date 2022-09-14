/*
 * StopWatch.c
 *
 *  Created on: Sep 21, 2021
 *      Author: MoHasseb
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/* make global variables for seconds, minutes and hours******************************/
unsigned char seconds = 0;
unsigned char minutes = 0;
unsigned char hours = 0;
/************************************************************************************/

/* function to increment seconds by 1 and adjust minutes and hours when exceed 59   */
void increment(void){
	seconds ++;
	if(seconds == 60){
		seconds = 0;
		minutes ++;
	}
	if(minutes == 60){
		minutes = 0;
		hours ++;
	}
}
/***********************************************************************************/

/* function to display seconds 1/2 on 7 segment ************************************/
void display_seconds1(void){
	PORTA &= 0xC0;              /* to disable all 7 segments ***********************/
	PORTA |= (1<<PA0);          /* to enable first 7 segment   *********************/
	char a7ad_seconds = seconds % 10;   /* to get 1st digit in seconds   ***********/
	 /* to make 1st 7 segment display 1st digit in seconds *************************/
	PORTC = (PORTC & 0xF0) + a7ad_seconds;
}
void display_seconds2(void){
	PORTA &= 0xC0;             /* to disable all 7 segments ************************/
	PORTA |= (1<<PA1);         /* to enable 2nd 7 segment **************************/
	char a7ad_seconds = seconds % 10;  /* to get 1st digit in seconds   ************/
	/* to get 2nd digit in seconds  ************************************************/
	char ashrat_seconds = (seconds - a7ad_seconds) / 10;
	/* to make 2nd 7 segment display 2nd digit in seconds **************************/
	PORTC = (PORTC & 0xF0) + ashrat_seconds;
}
/***********************************************************************************/

/* function to display minutes 1/2 on 7 segment ************************************/
void display_minutes1(void){
	PORTA &= 0xC0;              /* to disable all 7 segments ***********************/
	PORTA |= (1<<PA2);          /* to enable 3rd 7 segment   ***********************/
	char a7ad_minutes = minutes % 10;   /* to get 1st digit in minutes   ***********/
	/* to make 3rd 7 segment display 1st digit in minutes **************************/
	PORTC = (PORTC & 0xF0) + a7ad_minutes;
}
void display_minutes2(void){
	PORTA &= 0xC0;             /* to disable all 7 segments ************************/
	PORTA |= (1<<PA3);         /* to enable 4th 7 segment **************************/
	char a7ad_minutes = minutes % 10;  /* to get 1st digit in minutes   ************/
	/* to get 2nd digit in minutes  ************************************************/
	char ashrat_minutes = (minutes - a7ad_minutes) / 10;
	/* to make 4th 7 segment display 2nd digit in minutes **************************/
	PORTC = (PORTC & 0xF0) + ashrat_minutes;
}
/***********************************************************************************/

/* function to display hours 1/2 on 7 segment **************************************/
void display_hours1(void){
	PORTA &= 0xC0;              /* to disable all 7 segments ***********************/
	PORTA |= (1<<PA4);          /* to enable 5th 7 segment   ***********************/
	char a7ad_hours = hours % 10;   /* to get 1st digit in hours   *****************/
	/* to make 5th 7 segment display 1st digit in hours ****************************/
	PORTC = (PORTC & 0xF0) + a7ad_hours;
}
void display_hours2(void){
	PORTA &= 0xC0;             /* to disable all 7 segments ************************/
	PORTA |= (1<<PA5);         /* to enable 6th 7 segment **************************/
	char a7ad_hours = hours % 10;  /* to get 1st digit in hours   ******************/
	char ashrat_hours = (hours - a7ad_hours) / 10;  /* to get 2nd digit in hours  **/
	/* to make 6th 7 segment display 2nd digit in hours ****************************/
	PORTC = (PORTC & 0xF0) + ashrat_hours;
}
/***********************************************************************************/

/* TIMER1 Compare channel A   ******************************************************/
ISR(TIMER1_COMPA_vect){
/* call function that increment seconds by 1 and adjust seconds, minutes and hours */
	increment();
}
void Timer1A_init(void){
	SREG |= (1<<7);             /* to enable i-bit *********************************/
	TCNT1 = 0;                  /* to count from 0 *********************************/
	OCR1A = 977;                /* max value to compare and then interrupt timer 1 throw flag 977   */
	TCCR1A |= (1<<FOC1A);       /* to adjust mode of timer to non PWM we set FOC1A = 1, WGM10 = 0,
	WGM11 = 0, disable OC1A so COM1A0 = 0, COM1A1 = 0      **************************************/
	TCCR1B |= (1<<WGM12) | (1<<CS10) | (1<<CS12); /* to adjust mode of timer to CTC OCR1A so WGM12 = 1,
	WGM13 = 0, to adjust pre-scaler to Fcpu/1024 so CS10 = 1, CS12 = 1, CS11=0 *******************/
	TIMSK |= (1<<OCIE1A); /* to enable timer1 compare interrupt       ***************************/
}
/********************************************************************/

/*   INTERRUPT 0                  */
ISR(INT0_vect){
	seconds = 0;     			/* to reset 6 7-segment to 0  */
	minutes = 0;
	hours = 0;
}
void INT0_init(void){
	SREG |= (1<<7);  			/* to enable i-bit */
	DDRD &= ~(1<<PD2);  		/* to make PD2 as input of switch   */
	PORTD |= (1<<PD2);			/* to enable internal pull up resistor  */
	MCUCR |= (1<<ISC01);        /* adjust mode of int0 to failing edge ISC01 = 1, ISC00 = 0  */
	GICR |= (1<<INT0);          /* to enable interrupt 0 */
}
/**********************************/

/*   INTERRUPT 1                  */
ISR(INT1_vect){
	TCCR1B &= 0xF8;      /* to stop timer1 and make clock = 0 CS10 = 0, CS11 = 0, CS12 = 0  *****/
}
void INT1_init(void){
	SREG |= (1<<7);  						 /* to enable i-bit */
	DDRD &= ~(1<<PD3);  					 /* to make PD3 as input of switch   */
	MCUCR |= (1<<ISC11) | (1<<ISC10);        /* adjust mode of int0 to failing edge ISC11 = 1, ISC10 = 1  */
	GICR |= (1<<INT1);          			 /* to enable interrupt 1 */
}
/*********************************/

/*   INTERRUPT 2                  */
ISR(INT2_vect){
	TCCR1B |= 0x05;   /* to resume counting so we turn on clock CS10 = 1, CS11 = 0, CS12 = 1   ******/
}
void INT2_init(void){
	SREG |= (1<<7);  						 /* to enable i-bit */
	DDRB &= ~(1<<PB2);  					 /* to make PB2 as input of switch   */
	PORTB |= (1<<PB2);                       /* to enable internal pull up resistor   */
	MCUCSR &= ~(1<<ISC2);                    /* to make failing edge */
	GICR |= (1<<INT2);          			 /* to enable interrupt 2 */
}
/*********************************/

int main(void){
	/* configure PC0/1/2/3 as output to 7 segment decoder ********/
	DDRC |= 0x0F;
	PORTC &= 0xF0;
	/*************************************************************/

	/* configure PA0/1/2/3/4/5 as output to enable of 6 7segment */
	DDRA |= 0x3F;
	PORTA |= 0x3F;
	/*************************************************************/

	/* initialize interrupt 0/1/2 and timer1A      ***************************/
	INT0_init();
	INT1_init();
	INT2_init();
	Timer1A_init();
	/*************************************************************/

	/* call functions to display seconds, minutes and hours on 6 7-segments and make delay 3ms between them*/
	while(1){
		display_seconds1();
		_delay_ms(2);
		display_seconds2();
		_delay_ms(2);
		display_minutes1();
		_delay_ms(2);
		display_minutes2();
		_delay_ms(2);
		display_hours1();
		_delay_ms(2);
		display_hours2();
		_delay_ms(2);
	}
}
