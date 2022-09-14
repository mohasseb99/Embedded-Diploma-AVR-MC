/*[FILE NAME]   : dcmotor.h
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 11, 2021
 *[DESCRIPTION] : this file contain the header file of the DC motor
 */


#ifndef DCMOTOR_H_
#define DCMOTOR_H_
#include"std_types.h"

#define MOTOR_IN1_PORT_ID PORTC_ID
#define MOTOR_IN1_PIN_ID  PIN5_ID

#define MOTOR_IN2_PORT_ID PORTC_ID
#define MOTOR_IN2_PIN_ID  PIN6_ID

typedef enum{
	MOTOR_OFF,MOTOR_CW,MOTOR_CCW
}DcMotor_State;


void DcMotor_Init(void);
void DcMotor_Rotate(DcMotor_State state,uint8 speed);


#endif /* DCMOTOR_H_ */
