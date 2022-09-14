/*[FILE NAME]   : eeprom.h
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 11, 2021
 *[DESCRIPTION] : Header file of the external EEPROM
 */
#ifndef EEPROM_H_
#define EEPROM_H_

#include"std_types.h"
#define SUCCESS (1)
#define ERROR   (0)
/*
 * Description :
 * Function responsible for writing only one byte in the EEPROM
 */
uint8 EEPROM_writeByte(uint16 address,uint8 data);
/*
 * Description :
 * Function responsible for reading only one byte from the EEPROM
 */
uint8 EEPROM_readByte(uint16 address,uint8 *data);
/*
 * Description :
 * Function responsible for writing array in the EEPROM based on its length
 */
uint8 EEPROM_writeArray(uint16 address,uint8 *data,uint8 length);
/*
 * Description :
 * Function responsible for reading array from the EEPROM based on its length
 */
uint8 EEPROM_readArray(uint16 address,uint8 *data,uint8 length);
#endif /* EEPROM_H_ */
