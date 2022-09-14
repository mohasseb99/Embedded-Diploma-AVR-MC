/*[FILE NAME]   : buzzer.h
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 28, 2021
 *[DESCRIPTION] : Source file for buzzer sensor
 */
#ifndef BUZZER_H_
#define BUZZER_H_
#include"std_types.h"

#define BUZZER_PORT_ID PORTD_ID
#define BUZZER_PIN_ID  PIN6_ID

/*
 * Description :
 * Function responsible for initializing the buzzer to make the pin an output pin
 */
void BUZZER_init();
/*
 * Description :
 * Function responsible for turn on the buzzer
 */
void BUZZER_on();
/*
 * Description :
 * Function responsible for turn off the buzzer
 */
void BUZZER_off();

#endif /* BUZZER_H_ */
