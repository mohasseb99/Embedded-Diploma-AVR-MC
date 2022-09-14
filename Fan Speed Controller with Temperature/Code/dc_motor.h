/*
 * dc_motor.h
 *
 *  Created on: Oct 17, 2021
 *      Author: Mohasseb
 */

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

#include "std_types.h"

/*******************************************************************************
 *                                Definitions                                  *
 *******************************************************************************/
#define MOTOR_PORT_ID		  PORTB_ID
#define INPUT1_PIN_ID         PIN0_ID
#define INPUT2_PIN_ID         PIN1_ID
#define ENABLE_PIN_ID         PIN3_ID
/*******************************************************************************
 *                         Types Declaration                                   *
 *******************************************************************************/
typedef enum
{
	STOP,ACW,CW
}DcMotor_State;

/*******************************************************************************
 *                      Functions Prototypes                                   *
 *******************************************************************************/

/*
 * Description :
 * Function responsible for initialization of DC Motor.
 */
void DcMotor_init(void);

/*
 * Description :
 * Function responsible for rotating DC Motor with required speed and state.
 * Takes state and speed from 0 to 100
 */
void DcMotor_rotate(DcMotor_State state, uint8 speed);

#endif /* DC_MOTOR_H_ */
