/*[FILE NAME]   : dcmotor.c
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 11, 2021
 *[DESCRIPTION] : this file contain the source file of the DC motor
 */

#include"dcmotor.h"
#include"gpio.h"
#include"pwm.h"
#include"common_macros.h"


void DcMotor_Init(void){
	/*setup the direction of the 2-pins of the motor */
	GPIO_setupPinDirection(MOTOR_IN1_PORT_ID,MOTOR_IN1_PIN_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(MOTOR_IN2_PORT_ID,MOTOR_IN2_PIN_ID,PIN_OUTPUT);
	/*to write 0*/
	GPIO_writePin(MOTOR_IN1_PORT_ID,MOTOR_IN1_PIN_ID,LOGIC_LOW);
	GPIO_writePin(MOTOR_IN2_PORT_ID,MOTOR_IN2_PIN_ID,LOGIC_LOW);
}

void DcMotor_Rotate(DcMotor_State state,uint8 speed){
	if(state == MOTOR_CW){
	GPIO_writePin(MOTOR_IN1_PORT_ID,MOTOR_IN1_PIN_ID,LOGIC_HIGH);
	GPIO_writePin(MOTOR_IN2_PORT_ID,MOTOR_IN2_PIN_ID,LOGIC_LOW);
	}
	else if(state == MOTOR_CCW){
	GPIO_writePin(MOTOR_IN1_PORT_ID,MOTOR_IN1_PIN_ID,LOGIC_LOW);
	GPIO_writePin(MOTOR_IN2_PORT_ID,MOTOR_IN2_PIN_ID,LOGIC_HIGH);
	}
	else{
	GPIO_writePin(MOTOR_IN1_PORT_ID,MOTOR_IN1_PIN_ID,LOGIC_LOW);
	GPIO_writePin(MOTOR_IN2_PORT_ID,MOTOR_IN2_PIN_ID,LOGIC_LOW);
	}
	PWM_Timer0_Start(speed);

}
