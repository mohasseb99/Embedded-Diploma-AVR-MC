/*[FILE NAME]   : common_macros.h
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: Sep 29, 2021
 *[DESCRIPTION] : this file contain the common macros which will be used in all applications
 */

#ifndef COMMON_MACROS_H_

#define COMMON_MACROS_H_

#define SET_BIT(REG,BIT_NUM)   ((REG)=(REG)|((1<<(BIT_NUM))))

#define CLEAR_BIT(REG,BIT_NUM)   ((REG)=(REG)&(~(1<<(BIT_NUM))))

#define TOGGLE_BIT(REG,BIT_NUM)    ((REG)=( REG )^(1<<(BIT_NUM)))

#define BIT_IS_SET(REG,BIT_NUM)  ((REG)&((1<<(BIT_NUM))))

#define BIT_IS_CLEAR(REG,BIT_NUM) (!((REG)&(1<<(BIT_NUM))))

#define ROR(REG,BIT_NUM)  ((REG)=(((REG)>>(BIT_NUM))|((REG)<<((8*sizeof((REG)))-(BIT_NUM)))))

#define ROL(REG,BIT_NUM)  ((REG) = (((REG)<<(BIT_NUM) )|((REG)>>((8*sizeof((REG)))-(BIT_NUM)))))

#define GET_BIT(REG,BIT_NUM)  (((REG)&(1<<(BIT_NUM)))>>(BIT_NUM))



#endif /* COMMON_MACROS_H_ */
