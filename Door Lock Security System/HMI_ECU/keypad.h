/*[FILE NAME]   : keypad.h
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: Oct 6, 2021
 *[DESCRIPTION] : this file contain the header of the keypad module
 */
#ifndef KEYPAD_H_
#define KEYPAD_H_
#include"std_types.h"


#define NUM_OF_ROWS 4 /*number of rows in the keypad*/
#define NUM_OF_COLS 4 /*number of cols in the keypad*/

#define KEYPAD_PORT_ID PORTA_ID /*the port which the keypad is connected to*/

#define KEYPAD_FIRST_ROW_PIN_ID PIN0_ID /*first pin connected to the first row*/
#define KEYPAD_FIRST_COL_PIN_ID PIN4_ID /*first pin connected to the first col*/
/*
 * pull up   : logic low
 * pull down : logic high*/
#define KEYPAD_BUTTON_PRESSED LOGIC_LOW

uint8 KEYPAD_getPressedKey(void);
#endif /* KEYPAD_H_ */
