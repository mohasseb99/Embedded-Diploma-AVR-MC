/*[FILE NAME]   : pwm.h
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 11, 2021
 *[DESCRIPTION] : this file contain the header file of the pwm
 */
#ifndef PWM_H_
#define PWM_H_
#include"std_types.h"
/*
 * Description :
 * Function responsible for start pwm in timer 0
 */
void PWM_Timer0_Start(uint8 duty_cycle);


#endif /* PWM_H_ */
