/*[FILE NAME]   : control_ECU.c
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 28, 2021
 *[DESCRIPTION] : The CONTROL ECU is responsible for controlling the whole system such as controlling the DC motor of the
 *                door and the buzzer. It also controls the LCD & KEYPAD of the HMI ECU by using the UART. The
 *                synchronization between the two ECUs is important to make the system reliable so, the speed of the
 *                UARTs in both ECUs must be the same
 */
#include"uart.h"
#include"twi.h"
#include"eeprom.h"
#include"std_types.h"
#include"timer.h"
#include<avr/io.h>

#include "buzzer.h"
#include"dcmotor.h"

 /*Password size */
#define PASSWORD_SIZE 5

#define MAX_POSSIBLE_WRONG_PASSWORD 3

#define MC2_READY 0xFF
#define ARE_MC2_READY 0x55
#define ARE_MC1_READY 0x10
#define MC1_READY 0x20

/*#define of the password location in EEPROM*/
#define PASSWORD_LOCATION 0x0000
/*#define for user choice between open the door or change the password*/
#define OPEN_THE_DOOR '+'
#define CHANGE_THE_PASSWORD '-'

#define GET_PASSWORD       0x02
#define LOCK_THE_SYSTEM    0x03
#define ADJUST_PASSWORD    0x04
#define DISPLAY_MENU       0x05
#define GET_CHOICE         0x06
#define DOOR_IS_OPENING    0x07
#define DOOR_IS_OPENED     0x08
#define DOOR_IS_CLOSING    0x09
#define DOOR_IS_CLOSED     0x11
#define PASSWORD_IS_SAVED  0x12
#define PASSWORD_IS_RIGHT_OPEN 0x13
#define NOT_VALID_CHOICE   0x14
/*Those are made for the compare function*/
#define PASSWORD_IS_RIGHT TRUE
#define PASSWORD_IS_WRONG FALSE


uint8 g_password[PASSWORD_SIZE];

/* To receive the matched password in it*/
uint8 g_pass[PASSWORD_SIZE];

uint8 g_count=0;

uint8 decision=FALSE;

/* Variable to count the number of times the user enters the password in wrong way */
uint8 g_wrongPassCount=0;

/* Variable will be incremented each timer the ISR is implemented */
volatile uint8 g_timerCount=0;





/*
 * Description :
 * Function responsible for receiving the password from the HMI ECU and save it in g_password global variable
 */
void receivePassword(void);
/*
 * Description :
 * Function responsible for receiving the matched password from the HMI ECU and save it in g_pass global variable
 */
void receiveMatchPassword(void);
/*
 * Description :
 * Function responsible for comparing the password and the matched password. if they are the same, it will return 1 and if
 * not, it will return 0.
 */
uint8 comparePassword(void);
/*
 * Description :
 * Function responsible for saving the password into the external EEPROM
 */
void savePasswordToEEPROM(void);
/*
 * Description :
 * Function responsible for getting the password from the external EEPROM
 */
void getPasswordFromEEPROM(void);
/*
 * Description :
 * Function responsible for adjusting the password. This function will call three functions which are:
 *                                                  1- receivePassword()
 *                                                  2- receiveMatchPassword()
 *                                                  3- comparePassword()
 */
void adjustPassword(void);
/*
 * Description :
 * Function responsible for receiving the user's choice from the HMI ECU
 */
void mc2ReceiveDecision(void);
/*
 * Description :
 * Function responsible for sending the decision of the CONTROL ECU to the HMI ECU
 */
void mc2SendDecision(void);
/*
 * Description :
 * This function will be called when the ISR code of the timer is happened. This function will increment the variable
 * responsible for unlock the system.
 */
void timerCounter(void);
/*
 * Description :
 * This function responsible for locking the system. This function receives the structure of the timer to init the timer
 * module responsible for locking the system
 */
void LockTheSystem(TIMER_configType *configtype1);



void receivePassword(){
	/* Looping for the password size to receive all character from the HMI ECU*/
	for(g_count=0;g_count<PASSWORD_SIZE;g_count++){
		/* while loop until HMI ECU send ARE_MC2_READY to the CONTROL ECU */
		while(UART_receiveByte() != ARE_MC2_READY);
		/*CONTROL ECU will respond with i am ready which is MC2_READY*/
		UART_sendByte(MC2_READY);
		/*Then HMI ECU will send the character to the CONTROL ECU and the CONTOL ECU will save it in the g_password*/
    	g_password[g_count] = UART_receiveByte();
	}
}
void receiveMatchPassword(){
	/* Looping for the password size to receive all character from the HMI ECU*/
	for(g_count=0;g_count<PASSWORD_SIZE;g_count++){
		/* while loop until HMI ECU send ARE_MC2_READY to the CONTROL ECU */
		while(UART_receiveByte() != ARE_MC2_READY);
		/*CONTROL ECU will respond with i am ready which is MC2_READY*/
		UART_sendByte(MC2_READY);
		/*Then HMI ECU will send the character to the CONTROL ECU and the CONTOL ECU will save it in the g_pass*/
    	g_pass[g_count] = UART_receiveByte();
	}
}

uint8 comparePassword(){
	/* Looping for the password size */
	for(g_count=0;g_count<PASSWORD_SIZE;g_count++){
		/*if only one character is not the same the for loop will be terminated by returning PASSWORD_IS_WRONG */
		if(g_password[g_count] != g_pass[g_count]){
			return PASSWORD_IS_WRONG;
		}
	}
	/*if the for loop is not terminated, the password will be correct and will return PASSWORD_IS_RIGHT */
	return PASSWORD_IS_RIGHT;
}

void savePasswordToEEPROM(){
	/* The password is an array so we will call the function of saving an array
	 * note : look at this function inside the source file of EEPROM (eeprom.c)
	 * */
	EEPROM_writeArray(PASSWORD_LOCATION,g_password,PASSWORD_SIZE);
}

void getPasswordFromEEPROM(){
	/* The password is an array so we will call the function of saving an array
	 * note : look at this function inside the source file of EEPROM (eeprom.c)
	 * */
	EEPROM_readArray(PASSWORD_LOCATION,g_password,PASSWORD_SIZE);
}

void adjustPassword(){
	/* The password will be adjusted only if the 2-passwords are right so we made the variable below and initialize it
	 * by false and looping for the false condition*/
	uint8 flag=FALSE;
	while(flag == FALSE){
		/* receive the password */
		receivePassword();
		/* recceive the matched password*/
		receiveMatchPassword();
		/* compare the 2-password and put the return value inside the flag*/
		flag = comparePassword();
		/* CONTROL ECU will send the HMI ECU are u ready which is ARE_MC1_READY*/
   	    UART_sendByte(ARE_MC1_READY);
   	    /* CONTROL ECU will not send the flag unless the HMI ECU respond with MC1_READY*/
	    while(UART_receiveByte() != MC1_READY);
	    /* Sending the flag to the HMI ECU*/
	    UART_sendByte(flag);
	}
}


void mc2ReceiveDecision(){
	/* HMI ECU will send to the CONTROL ECU are u ready which is ARE_MC2_READY so this is a looping until CONTROL ECU
	 * receives this byte
	 * */
	while(UART_receiveByte() != ARE_MC2_READY);
	/* HMO ECU will not send the decision unless the CONTROL ECU respond back with MC2_READY*/
	UART_sendByte(MC2_READY);
	/* receiving the decision from the HMI ECU*/
	decision=UART_receiveByte();
}

void mc2SendDecision(){
	/* CONTROL ECU wants to send the decision to the HMI ECU but we must make synchronization between them first so,
	 * CONTROL ECU will ask if the HMI ECY is ready
	 * */
    UART_sendByte(ARE_MC1_READY);
    /* while loop until the HMI_ECU send i am ready which is MC1_READY*/
    while(UART_receiveByte() != MC1_READY);
    /* sending the decision to the HMI ECU*/
    UART_sendByte(decision);
}

void timerCounter(void){
	/* increment the timer counter to controlling the time of door and lock */
	g_timerCount++;
}
void LockTheSystem(TIMER_configType *configtype1){
	/* put zero in the g_timerCount to start from zero (for safety) */
	g_timerCount=0;
	/* we need to put 0 in the variable g_wrongPassCount for the next time */
	g_wrongPassCount=0;
	/* Initialize the TIMER1 by the structure that control the lock time*/
	TIMER_init(configtype1);
	/*turn on the buzzer */
    BUZZER_on();
    /* sending lock the system to the HMI ECU to be display into the LD */
    decision=LOCK_THE_SYSTEM;
    /* Sending the decision to the HMI ECU */
    mc2SendDecision();
    /* while loop until the timer reach a 1 minute*/
    while(g_timerCount<10);
    /* De-initializing the timer */
    TIMER_DeInit(TIMER1);
    /*turn off the buzzer*/
	BUZZER_off();
	/* put zero in the g_timerCount for the next time*/
	g_timerCount=0;
}




int main(){
	uint8 decisionFlag;
	/* To enable the global interrupt */
	SREG|=(1<<7);
	/*To initialize the DC MOTOR : making the 2-pins an output pins*/
	DcMotor_Init();
	/* Structure for adjusting TIMER1 to control the Lock time of the system
	 * We will use timer TIMER1 : F_CPU = 8MHz , Prescaler = 1024
	 *                            Ftimer =  7812.5
	 *                            Ttimer = 1/7812.5 = 0.000128
	 *                            with compare value of 46875
	 *                            Ttimer = 46875 * 0.000128 = 6 second
	 *                            to make the Lock time is 1 minute we need 10 overflow so, we will while loop until
	 *                            g_timerCount is equal 10
	 * */
	TIMER_configType configtype1={TIMER1,COMPARE_MODE_TIMER1,F_CPU_1024,0,46875};
	/* Structure for adjusting TIMER1 to control the door time (opening and closing)
	 * We will use timer TIMER1 : F_CPU = 8MHz , Prescaler = 64
	 *                            Ftimer =  125000
	 *                            Ttimer = 1/125000 = 0.000008
	 *                            with compare value of 62500
	 *                            Ttimer = 62500 * 0.000008 = 0.5 second
	 *                          1- To make the door opening time is 15 second we need 30 overflow so, we will while loop
	 *                             until g_timerCount is equal 15
	 *                          2- To make the door holding time is (15+3) seconds we need 39 overflow so,
	 *                             we will while loop until g_timerCount is equal 39
	 *                          3- To make the door holding time is (15+3+15) seconds we need 66 overflow so,
	 *                             we will while loop until g_timerCount is equal 66
	 *
	 * */
	TIMER_configType configtype2={TIMER1,COMPARE_MODE_TIMER1,F_CPU_64,0,62500};
	/* Set the call back of the timer interrupt in order to make the timer not depends on the application*/
	TIMER1_setCallBack(timerCounter);
	/* To make the buzzer pin an output pin*/
	BUZZER_init();
	/*initialize the UART with a specific UART frame based on structure*/
	UART_ConfigType uartConfigType={PARITY_DISABLED,ONE_BIT,EIGHT_BITS,115200};
	/*Initialize the UART based on the upper structure */
	UART_init(&uartConfigType);
	/*initialize the TWI with a specific speed and address based on structure*/
	TWI_configType twiConfigType={FAST_MODE_400,1};
	/*Initialize the TWI based on the upper structure */
	TWI_init(&twiConfigType);

	/*Here, the CONTROL ECU will make the decisions to the HMI ECU*/

	/* First : in case of new system the user must set the password so the CONTROL ECU need to set the password that's
	 * why it send that decision to the HMI ECU to control the keypad for setting the Password */
	decision=ADJUST_PASSWORD;
	/* Sending the decision to the HMI ECU */
	mc2SendDecision();
	/* Call the function of adjusting the password */
	adjustPassword();
	/* Second : sending order the password is saved to the HMI to display that on the LCD */
	decision = PASSWORD_IS_SAVED;
	/* sending the decision to the HMI ECU*/
	mc2SendDecision();
	/* Saving the password in the external EEPROM */
	savePasswordToEEPROM();


	while(1){
		/* Third : CONTROL ECU sends this decision to the HMI ECU to display the menu on the screen by the HMI ECU*/
		decision = DISPLAY_MENU;
		/* sending the decision to the HMI ECU*/
		mc2SendDecision();
		/* Fourth : To control the keypad to get a choice from the user, we set the choice to be that */
		decision=GET_CHOICE;
		/* sending the decision to the HMI ECU*/
		mc2SendDecision();
		/* FIFTH  : To receive the choice from the HMI ECU, we call this function below */
		mc2ReceiveDecision();

		/* In case the user enter '+' (OPEN_THE_DOOR) or '-' (CHANGE_THE_PASSWORD) */
		if( (decision == OPEN_THE_DOOR) || (decision == CHANGE_THE_PASSWORD) ){
			/* To not lose the user choice, we put its value in the variable below */
			decisionFlag=decision;
			/* Get the password from External EEPROM and save it inside the g_password global variable */
			getPasswordFromEEPROM();
			/* Sixth : in order to open the door or change the password, the user must enter the old password
			 *         so, we need to get the password from the user and compare it with the password in the EEPROM
			 *         */
			decision=GET_PASSWORD;
			/* sending the decision to the HMI ECU*/
			mc2SendDecision();
			/* Looping until the g_wrongPassCount less than 3 and decision==GET_PASSWORD
			 * The display menu will be displayed again after this loop is terminated
			 *  */
			while((g_wrongPassCount<3) && (decision==GET_PASSWORD)){
				/* Receive the password from the HMI ECU */
			    receiveMatchPassword();
			    /* Call the compare function..
			     * This function return 1 or 0 : 1 if--> password is right, 0 if--> password is wrong
			     * */
			    decision=comparePassword();
			    /* First case : if the password is right and the user choice was changing the password */
			    if(decision == PASSWORD_IS_RIGHT && decisionFlag==CHANGE_THE_PASSWORD){
			    	/* sending password is right to the HMI ECU to be display into the LD */
			    	decision = PASSWORD_IS_RIGHT;
			    	/* sending the decision to the HMI ECU*/
			    	mc2SendDecision();
			    	/* we need to put 0 in the variable g_wrongPassCount for the next time */
			    	g_wrongPassCount=0;
			    	/* sending that decision to the HMI ECU to control the keypad & LCD for setting the Password */
			    	decision=ADJUST_PASSWORD;
			    	/* sending the decision to the HMI ECU*/
			    	mc2SendDecision();
			    	/* Call the function of adjusting the password */
			    	adjustPassword();
			    	/* Second : sending order the password is saved to the HMI to display that on the LCD */
			    	decision = PASSWORD_IS_SAVED;
			    	/* sending the decision to the HMI ECU*/
			    	mc2SendDecision();
			    	/* Saving the password in the external EEPROM */
			    	savePasswordToEEPROM();
			    }
			    /* Second case : if the password is right and the user choice was changing the password */
			    else if(decision == PASSWORD_IS_RIGHT && decisionFlag==OPEN_THE_DOOR){
			    	/* sending password is right to the HMI ECU to be display into the LD */
			    	decision = PASSWORD_IS_RIGHT_OPEN;
			    	/* sending the decision to the HMI ECU*/
			    	mc2SendDecision();
			    	/* we need to put 0 in the variable g_wrongPassCount for the next time */
			    	g_wrongPassCount=0;
			    	/* put zero inside the timer counter to start from zero (for safety)*/
			    	g_timerCount=0;
			    	/* Initialize the TIMER1 by the structure that control the door time (opening and closing) */
			    	TIMER_init(&configtype2);

			    	/* Second : sending order the door is opening to the HMI to display that on the LCD */
			    	decision = DOOR_IS_OPENING;
			    	/* sending the decision to the HMI ECU*/
			    	mc2SendDecision();
			    	/* rotate the dc motor clock wise*/
			    	DcMotor_Rotate(MOTOR_CW,100);
			    	/* looping until it reach 15 seconds (Opening time = 15 seconds)*/
			    	while(g_timerCount < 30);

			    	/* Second : sending order the door is opened to the HMI to display that on the LCD */
			    	decision = DOOR_IS_OPENED;
			    	/* sending the decision to the HMI ECU*/
			    	mc2SendDecision();
			    	/* Stop the dc motor to make the user enters the door*/
			    	DcMotor_Rotate(MOTOR_OFF,0);
			    	/* looping until it reach 18 seconds (Holding time = 3 seconds)
			    	 * Why 18 seconds ??
			    	 * --> 15 seconds for the Opening time and 3 second for the holding time
			    	 * */
			    	while(g_timerCount < 39);

			    	/* Second : sending order the door is closing to the HMI to display that on the LCD */
			    	decision = DOOR_IS_CLOSING;
			    	/* sending the decision to the HMI ECU*/
			    	mc2SendDecision();
			    	/* rotate the dc motor counter clock wise*/
			    	DcMotor_Rotate(MOTOR_CCW,100);
			    	/* looping until it reach 33 seconds (Closing time = 15 seconds)
			    	 * Why 33 seconds ??
			    	 * --> 15 seconds for the Opening time, 3 second for the holding time and 15 seconds for the closing
			    	 *  time
			    	 * */
			    	while(g_timerCount < 66);

			    	/* Second : sending order the door is closed to the HMI to display that on the LCD */
			    	decision = DOOR_IS_CLOSED;
			    	/* sending the decision to the HMI ECU*/
			    	mc2SendDecision();
			    	/* Stop the dc motor ( door is closed )*/
			    	DcMotor_Rotate(MOTOR_OFF,0);
			    	/*waiting 2-second to make the message display on the screen*/
			    	while(g_timerCount < 70);
			    	/* De-init the TIMER1 */
			    	TIMER_DeInit(TIMER1);
			    	/* put zero into the g_timerCount for the next time */
			    	g_timerCount=0;

			    }
			    /* Third case : if the password is right and the user choice was changing the password */
			    else if(decision == PASSWORD_IS_WRONG){
			    	/* Increment the g_wrongPassCount every time the user enters the password wrong */
			    	g_wrongPassCount++;
			    	/* If g_wrongPassCount equal the maximum possible wrong password, the system must be locked*/
			    	if(g_wrongPassCount == MAX_POSSIBLE_WRONG_PASSWORD)
			    	{
			    		/* Calling the Lock function*/
			    	    LockTheSystem(&configtype1);
			    	}
			    	/* if the g_wrongPassCount is less than 3, the following must be happened*/
			    	else
			    	{
			    		/* sending password is wrong to the HMI ECU to be display into the LD */
			   	        decision=PASSWORD_IS_WRONG;
			   	        /* sending the decision to the HMI ECU*/
			    	    mc2SendDecision();
			    	    /* Get the password again */
			    	    decision=GET_PASSWORD;
			    	    /* sending the decision to the HMI ECU*/
			    	    mc2SendDecision();
			    	   }
			    }
			}
		}
		/* In case the user enters another character rather than '-' and '+'*/
		else if( (decision != CHANGE_THE_PASSWORD) && (decision != OPEN_THE_DOOR) ){
			/* This is not valid option so, we will ask the user to enter the again*/
			decision = NOT_VALID_CHOICE;
			/* sending the decision to the HMI ECU*/
			mc2SendDecision();
		}


	}
}
