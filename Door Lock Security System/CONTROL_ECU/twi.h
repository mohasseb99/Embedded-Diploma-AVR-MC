/*[FILE NAME]   : twi.h
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 22, 2021
 *[DESCRIPTION] : Header file for two wire interface (I2C)
 */
#include"std_types.h"




/*Start bit has been sent to the SDA bus*/
#define TWI_START 0x08
/*Repeated start has been sent to the SDA bus*/
#define TWI_REPEATED_START 0x10
/*Master transmit the slave address and the write control bit to the SDA bus
 *  and received the ACK from the slave*/
#define TWI_MT_SLA_W_ACK 0x18
/*Master transmit the slave address and the read control bit to the SDA bus
 * and received the ACK from the slave*/
#define TWI_MT_SLA_R_ACK 0x40
/*Master transmit the data to the SDA bus and received the ACK from the slave*/
#define TWI_MT_DATA_ACK 0x28
/*Master received the data from the slave and transmit ACK to the slave*/
#define TWI_MR_DATA_ACK 0x50
/*Master received the data from the slave but didn't respond with the ACK
 * this condition will be useful when i need to tell the slave: enough and don't send again*/
#define TWI_MR_DATA_NACK  0x58

/*enum is used to differentiate between the normal mode and fast mode*/
typedef enum{
	NORMAL_MODE_100=100000,FAST_MODE_400=400000
}TWI_modeType;

/* Structure members :
 * 1- TWI_modeType mode ----> for the bit rate mode
 * 2- uint8 address     ----> to give the master an address because it can be slave at any time because the
 * two wire interface protocol is multi slave communication protocol
 * */
typedef struct{
	TWI_modeType mode;
	uint8 address;
}TWI_configType;


/*
 * Description :
 * Function responsible for initializing the I2C based on the structure given
 */
void TWI_init(TWI_configType *Config_Ptr);
/*
 * Description :
 * Function responsible for sending a start bit
 */
void TWI_start();
/*
 * Description :
 * Function responsible for sending a stop bit
 */
void TWI_stop();
/*
 * Description :
 * Function responsible for writing a byte to the SDA bus
 */
void TWI_writeByte(uint8 data);
/*
 * Description :
 * Function responsible for reading a byte to the SDA bus and respond with the ACK
 */
uint8 TWI_readByteWithACK();
/*
 * Description :
 * Function responsible for reading a byte to the SDA bus and not respond with the ACK
 */
uint8 TWI_readByteWithNACK();

/*
 * Description :
 * Function responsible for getting the status of the TWI
 */
uint8 TWI_getStatus();
