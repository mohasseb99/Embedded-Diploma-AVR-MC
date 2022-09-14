/*[FILE NAME]   : timer.c
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 22, 2021
 *[DESCRIPTION] : Source file for Full implemented timer driver with TIMER0,TIMER1 and TIMER2
 */

#include"timer.h"
#include"common_macros.h"
#include<avr/io.h>
#include<avr/interrupt.h>


static volatile void (*g_timer0CallBackPtr)(void)=NULL_PTR;
static volatile void (*g_timer1CallBackPtr)(void)=NULL_PTR;


ISR(TIMER0_OVF_vect){
	if(g_timer0CallBackPtr != NULL_PTR){
		(*g_timer0CallBackPtr)();
	}
}

ISR(TIMER0_COMP_vect){
	if(g_timer0CallBackPtr != NULL_PTR){
		(*g_timer0CallBackPtr)();
	}
}

ISR(TIMER1_OVF_vect){
	if(g_timer1CallBackPtr != NULL_PTR){
		(*g_timer1CallBackPtr)();
	}
}

ISR(TIMER1_COMPA_vect){
	if(g_timer1CallBackPtr != NULL_PTR){
		(*g_timer1CallBackPtr)();
	}
}


void TIMER_init(const TIMER_configType *Config_Ptr){
	/*In case of timer0*/
	if(Config_Ptr->timer_id == TIMER0){
		TCCR0 = (1<<FOC0);
		/*if normal mode : WGM00=0 , WGM01=0
		 *if compare mode : WGM00=0 , WGM01=1
		 * */
		TCCR0 = (TCCR0 & 0xB7) | (Config_Ptr->mode<<2);
		/* The next line is to adjust the prescaler
		 * */
		TCCR0 = (TCCR0 & 0xF8) | (Config_Ptr->prescaler);
		/*The below line is for initial value of the timer register */
		TCNT0 = (uint8)(Config_Ptr->timerStartValue);
		/*Enabling the interrupt of overflow and compare mode in order to execute the ISR code*/
		if(Config_Ptr->mode == NORMAL_MODE){
			TIMSK |=(1<<TOIE0);
		}
		else if(Config_Ptr->mode == COMPARE_MODE){
			OCR0  = (uint8)(Config_Ptr->compareValue);
			TIMSK |=(1<<OCIE0);
		}
	}
	/*In case of timer1*/
	else if(Config_Ptr->timer_id == TIMER1){
		/*The below line is for initial value of the timer register */
		TCNT1  = Config_Ptr->timerStartValue;

		TCCR1A = (1<<FOC1A);
		TCCR1B=0;

		TCCR1B = (TCCR1B & 0xE7) | (Config_Ptr->mode<<1);
		/* The next line is to adjust the prescaler
		 * */
		TCCR1B = (TCCR1B & 0xF8) | (Config_Ptr->prescaler);
		/*Enabling the interrupt of overflow and compare mode in order to execute the ISR code*/
		if(Config_Ptr->mode == NORMAL_MODE){
			TIMSK |=(1<<TOIE1);
		}
		else if(Config_Ptr->mode == COMPARE_MODE_TIMER1){
			/* To adjust the compare value */
			OCR1A  = Config_Ptr->compareValue;
			TIMSK |=(1<<OCIE1A);
		}
	}
}


void TIMER0_setCallBack(void (*a_ptr)(void)){
	g_timer0CallBackPtr=a_ptr;
}

void TIMER1_setCallBack(void (*a_ptr)(void)){
	g_timer1CallBackPtr=a_ptr;
}

void TIMER_DeInit(TIMER_ID timer_id){
	/*In case of timer0*/
	if(timer_id == TIMER0){
	    TCCR0=0;
	    TCNT0=0;
	    OCR0=0;
	    /*disable the interrupt module of timer0*/
	    TIMSK &=~(1<<TOIE0) &~(1<<OCIE0);
	}
	/*In case of timer1*/
	else if(timer_id == TIMER1){
	    TCCR1A=0;
	    TCCR1B=0;
	    TCNT1=0;
	    OCR1A=0;
	    /*disable the interrupt module of timer1*/
	    TIMSK &=~(1<<TOIE1) &~(1<<OCIE1A);
	}
}

