/*[FILE NAME]   : uart.c
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 15, 2021
 *[DESCRIPTION] : this file contain the source of the uart driver
 */

#include"uart.h"
#include"avr/io.h"
#include"common_macros.h"

void UART_init(const UART_ConfigType *Config_Ptr){
	/*Variable for calculating the UBRR value*/
	uint16 ubrrValue;

	UCSRA = (1<<U2X);
	/*
	 * To enbale the transmitter and receiver*/
	UCSRB = (1<<RXEN) | (1<<TXEN);
	/* URSEL must be 1 to write in the UCSRC register
	 * */
	UCSRC = (1<<URSEL) | ( (Config_Ptr->parity) <<4) | ( (Config_Ptr->stopBit) <<3) | ((Config_Ptr->dataBits) <<1);
	/*To calculate the ubrr value based on the baud rate given in the structure*/
	ubrrValue =(uint16)((F_CPU/( ( Config_Ptr->baudRate ) *8 ))-1);
	/*put the first eight bits in the UBRRL*/
	UBRRL=ubrrValue;
	/*to put the next 4 bits of the ubbrValue we need to shift right by 8*/
	UBRRH=ubrrValue>>8;
}


void UART_sendByte(uint8 data){
	/*if the UDRE flag is 0 that's mean that the sending process is not finished yet
	  */
	while(BIT_IS_CLEAR(UCSRA,UDRE));
	/*if the while loop is false that's mean UDR is empty so we will put the data
	 * in the UDR register*/
	UDR=data;
}


uint8 UART_receiveByte(){
	/*The RXC flag will be 1 when the receiving process is completed */
	while(BIT_IS_CLEAR(UCSRA,RXC));
	/*returning the UDR value if the flag is 1 and clear the flag in this
	 * instruction too*/
	return UDR;
}

void UART_sendString(uint8 *string){
	/*while till u find the null character*/
	while(*string != '\0'){
		/*sending the first byte*/
		UART_sendByte(*string);
		/*increment the pointer to the next byte location*/
		string++;
	}
}

void UART_receiveString(uint8 *string){
	/*receiving the first byte and then check if byte is equal to the special character */
	*string = UART_receiveByte();
	while(*string != '#'){
		/*increment the pointer to the next byte location to put the next byte in the next location*/
		string++;
		/*receive the next byte and put it in the next location of the string */
		*string = UART_receiveByte();
	}
	*string='\0';
}
