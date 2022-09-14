/*[FILE NAME]   : timer.h
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 22, 2021
 *[DESCRIPTION] : Header file for Full implemented timer driver with TIMER0,TIMER1
 */

#ifndef TIMER_H_
#define TIMER_H_

#include"std_types.h"

typedef enum{
	TIMER0,TIMER1
}TIMER_ID;

/* for timer 0 mode normal = 0, compare = 2
 * for timer 1 mode normal = 0, compare = 4
 * */
typedef enum{
	NORMAL_MODE,COMPARE_MODE=2,COMPARE_MODE_TIMER1=4
}TIMER_modeType;


typedef enum{
	NO_CLOCK,F_CPU_CLOCK,F_CPU_8,F_CPU_64,F_CPU_256,F_CPU_1024
}TIMER_prescaler;

typedef struct{
	TIMER_ID timer_id;
	TIMER_modeType mode;
	TIMER_prescaler prescaler;
	uint16 timerStartValue;
	uint16 compareValue;
}TIMER_configType;



/*
 * Description :
 * Function responsible for initializing the timer
 */
void TIMER_init(const TIMER_configType *Config_Ptr);
/*
 * Description :
 * Function responsible for setting the call back function of timer0 if the Interrupt happened
 */
void TIMER0_setCallBack(void (*a_ptr)(void));
/*
 * Description :
 * Function responsible for setting the call back function of timer1 if the Interrupt happened
 */
void TIMER1_setCallBack(void (*a_ptr)(void));
/*
 * Description :
 * Function responsible for de-initializing the timer
 * 	This function takes the timer id to de-init the timer based on its id
 */
void TIMER_DeInit(TIMER_ID timer_id);




#endif /* TIMER_H_ */
