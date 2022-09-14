/*[FILE NAME]   : uart.h
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 15, 2021
 *[DESCRIPTION] : this file contain the header of the uart driver
 */

#ifndef UART_H_
#define UART_H_
#include"std_types.h"
/*To choose between the parity modes*/
typedef enum{
	PARITY_DISABLED,EVEN_PARITY=2,ODD_PARITY
}UART_ParityMode;

/*To choose between the stop bits number*/
typedef enum{
	ONE_BIT,TWO_BIT
}UART_StopBit;

/*To choose the data bits numbers
 */
typedef enum{
	FIVE_BITS,SIX_BITS,SEVEN_BITS,EIGHT_BITS
}UART_CharacterSize;

/*This structure will be passed to the init function
 */
typedef struct{
	UART_ParityMode parity;
	UART_StopBit stopBit;
	UART_CharacterSize dataBits;
	uint32 baudRate;
}UART_ConfigType;

/************************ Function Prototypes ***************************/
/*
 * Description :
 * Function responsible for sending the byte
 */
void UART_sendByte(uint8 data);
/*
 * Description :
 * Function responsible for receiving a byte so the return will be uint8
 */
uint8 UART_receiveByte();
/*
 * Description :
 * Function responsible for initialize the uart driver based on the structure given
 */
void UART_init(const UART_ConfigType *Config_Ptr);
/*
 * Description :
 * Function responsible for sending a string
 */
void UART_sendString(uint8 *string);
/*
 * Description :
 * Function responsible for receiving a string based on a special character which is '#'
 */
void UART_receiveString(uint8 *string);

#endif /* UART_H_ */
