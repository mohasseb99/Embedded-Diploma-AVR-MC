/*
 * dc_motor.c
 *
 *  Created on: Oct 17, 2021
 *      Author: Mohasseb
 */

#include "dc_motor.h"
#include "gpio.h"
#include "pwm.h"

/*******************************************************************************
 *                      Functions Definitions                                  *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for initialization of DC Motor.
 */
void DcMotor_init(void){
	/* setup pin connected to motor as output */
	GPIO_setupPinDirection(MOTOR_PORT_ID, INPUT1_PIN_ID, PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_PORT_ID, INPUT2_PIN_ID, PIN_OUTPUT);

	/* write 0 on pins */
	GPIO_writePin(MOTOR_PORT_ID, INPUT1_PIN_ID, STOP);
	GPIO_writePin(MOTOR_PORT_ID, INPUT2_PIN_ID, STOP);
}

/*
 * Description :
 * Function responsible for rotating DC Motor with required speed and state.
 * Takes state and speed from 0 to 100
 */
void DcMotor_rotate(DcMotor_State state, uint8 speed){
	/* write state to pins, we make and to get 1st bit or 2nd bit */
	GPIO_writePin(MOTOR_PORT_ID, INPUT1_PIN_ID, state & 1);
	GPIO_writePin(MOTOR_PORT_ID, INPUT2_PIN_ID, (state & 2) >> 1);

	/* send speed to pwm  */
	PWM_Timer0_Start(speed);
}
