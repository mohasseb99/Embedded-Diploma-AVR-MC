/*
 * pwm.h
 *
 *  Created on: Oct 17, 2021
 *      Author: Mohasseb
 */

#ifndef PWM_H_
#define PWM_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define PWM_PORT_ID            PORTB_ID
#define PWM_PIN_ID             PIN3_ID

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for trigger the Timer0 with the PWM Mode.
 * Setup the direction for OC0 as output pin through the GPIO driver.
 */
void PWM_Timer0_Start(uint8 duty_cycle);

#endif /* PWM_H_ */
