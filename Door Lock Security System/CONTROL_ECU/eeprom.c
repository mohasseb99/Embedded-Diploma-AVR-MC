/*[FILE NAME]   : eeprom.c
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 11, 2021
 *[DESCRIPTION] : this file contain the header file of the DC motor
 */

#include"twi.h"
#include"eeprom.h"


uint8 EEPROM_writeByte(uint16 address,uint8 data){
	/* First : we send the start bit
	 * */
	TWI_start();
	/*Check of sending the start bit*/
	if(TWI_getStatus() != TWI_START){
		return ERROR;
	}
	/* In the device address, we need to send the last 3-bits of the location which we want to write in
	 * */
	TWI_writeByte((uint8)((((address & 0x0700)>>7) | 0xA0 )));
	/*Check of sending the slave address and responded with ACK*/
	if(TWI_getStatus() != TWI_MT_SLA_W_ACK){
		return ERROR;
	}
	/*sending the first 8-bits in the location address*/
	TWI_writeByte((uint8)(address));
	/*Check of sending the location address and responded with ACK*/
	if(TWI_getStatus() != TWI_MT_DATA_ACK){
		return ERROR;
	}
	/*sending the data in the location address*/
	TWI_writeByte(data);
	/*Check of sending the data and responded with ACK*/
	if(TWI_getStatus() != TWI_MT_DATA_ACK){
		return ERROR;
	}
	TWI_stop();
	/*return success operation */
	return SUCCESS;
}

uint8 EEPROM_readByte(uint16 address,uint8 *data){
	/* First : we send the start bit
	 * */
	TWI_start();
	/*Check of sending the start bit*/
	if(TWI_getStatus() != TWI_START){
		return ERROR;
	}
	/* In the device address, we need to send the last 3-bits of the location which we want to write in
	 * */
	TWI_writeByte((uint8)((((address & 0x0700)>>7) | 0xA0 )));
	/*Check of sending the slave address and responded with ACK*/
	if(TWI_getStatus() != TWI_MT_SLA_W_ACK){
		return ERROR;
	}
	/*sending the first 8-bits in the location address*/
	TWI_writeByte((uint8)(address));
	/*Check of sending the location address and responded with ACK*/
	if(TWI_getStatus() != TWI_MT_DATA_ACK){
		return ERROR;
	}
	/*Sending the repeated start bit*/
	TWI_start();
	if(TWI_getStatus() != TWI_REPEATED_START){
		return ERROR;
	}
	/*Here, we need to send read control bit so we will make or operation with 1*/
	TWI_writeByte((uint8)((((address & 0x0700)>>7) | 0xA0 | 1 )));
	/*Check of sending the slave address and responded with ACK*/
	if(TWI_getStatus() != TWI_MT_SLA_R_ACK){
		return ERROR;
	}
	/*reading the byte*/
	*data=TWI_readByteWithNACK();
    if (TWI_getStatus() != TWI_MR_DATA_NACK){
        return ERROR;
       }
    TWI_stop();
	return SUCCESS;
}


uint8 EEPROM_writeArray(uint16 address,uint8 *data,uint8 length){
	uint8 i=0;
	/* First : we send the start bit
		 * */
	TWI_start();
	/*Check of sending the start bit*/
	if(TWI_getStatus() != TWI_START){
		return ERROR;
	}
	/* In the device address, we need to send the last 3-bits of the location which we want to write in
	 * */
	TWI_writeByte((uint8)((((address & 0x0700)>>7) | 0xA0 )));
	/*Check of sending the slave address and responded with ACK*/
	if(TWI_getStatus() != TWI_MT_SLA_W_ACK){
		return ERROR;
	}
	/*sending the first 8-bits in the location address*/
	TWI_writeByte((uint8)(address));
	/*Check of sending the location address and responded with ACK*/
	if(TWI_getStatus() != TWI_MT_DATA_ACK){
		return ERROR;
	}
	/* For loop to the length of the array to write in the memmory
	 * */
	for(i=0;i<length;i++){
	/*sending the data in the location address*/
	TWI_writeByte(data[i]);
	/*Check of sending the data and responded with ACK*/
	if(TWI_getStatus() != TWI_MT_DATA_ACK){
		return ERROR;
	}

	}
	/*sending the stop bit*/
	TWI_stop();
	return SUCCESS;
}



uint8 EEPROM_readArray(uint16 address,uint8 *data,uint8 length){
	    uint8 i=0;
		/* First : we send the start bit
			 * */
		TWI_start();
		/*Check of sending the start bit*/
		if(TWI_getStatus() != TWI_START){
			return ERROR;
		}
		/* In the device address, we need to send the last 3-bits of the location which we want to write in
		 * */
		TWI_writeByte((uint8)((((address & 0x0700)>>7) | 0xA0 )));
		/*Check of sending the slave address and responded with ACK*/
		if(TWI_getStatus() != TWI_MT_SLA_W_ACK){
			return ERROR;
		}
		/*sending the first 8-bits in the location address*/
		TWI_writeByte((uint8)(address));
		if(TWI_getStatus() != TWI_MT_DATA_ACK){
			return ERROR;
		}
		/*sending another start bit to change the control bit from write to read because we want to read*/
		TWI_start();
		if(TWI_getStatus() != TWI_REPEATED_START){
			return ERROR;
		}
		/*Here, we need to send read control bit so we will make or operation with 1*/
		TWI_writeByte((uint8)((((address & 0x0700)>>7) | 0xA0 | 1 )));
		/*Check of sending the slave address and responded with ACK*/
		if(TWI_getStatus() != TWI_MT_SLA_R_ACK){
			return ERROR;
		}
		for(i=0;i<length-1;i++){

		data[i]=TWI_readByteWithACK();
		if (TWI_getStatus() != TWI_MR_DATA_ACK){
		      return ERROR;
		   }

		}
		data[i]=TWI_readByteWithNACK();
		if (TWI_getStatus() != TWI_MR_DATA_NACK){
	      return ERROR;
		   }

		TWI_stop();
		return SUCCESS;
}
