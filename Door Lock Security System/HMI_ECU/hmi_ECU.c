/*[FILE NAME]   : hmi_ECU.h
 *[AUTHOR]      : Mahmoud Mohasseb
 *[DATE CREATED]: OCT 28, 2021
 *[DESCRIPTION] : The CONTROL ECU is responsible for monitoring the CONTROL ECU decisions and implement it.
 *                It control the LCD and KEYPAD based on the CONTROL ECU decision based on the UART communication protocol.The
 *                synchronization between the two ECUs is important to make the system reliable so, the speed of the UARTs in
 *                both ECUs must be the same
 */

#include"lcd.h"
#include"keypad.h"
#include"uart.h"
#include"util/delay.h"
#include"std_types.h"
#include"timer.h"
#include<avr/io.h>

/*Password size */
#define PASSWORD 5

/* The four #s below for the UART to the communication between the HMI ECU & CONTROL ECU
 * */
#define MC2_READY 0xFF
#define ARE_MC2_READY 0x55
#define ARE_MC1_READY 0x10
#define MC1_READY 0x20
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
#define PASSWORD_IS_RIGHT TRUE
#define PASSWORD_IS_WRONG FALSE

uint8 choice=FALSE;

/* Variable will be incremented each timer the ISR is implemented */
volatile uint8 g_timerCount=0;

/*
 * Description :
 * Function responsible for sending the choice of the user to the CONTROL ECU
 */
void mc1SendChoice(void);
/*
 * Description :
 * Function responsible for receiving the CONTROL ECU decision
 */
void mc1ReceiveChoice(void);
/*
 * Description :
 * Function responsible for receiving the user choice based on the displayed menu
 */
void getChoiceFromUser(void);
/*
 * Description :
 * Function responsible for displaying the menu on the screen
 */
void displayMenu(void);
/*
 * Description :
 * Function responsible for adjusting the password. This function will call getSendPassword() function 2-times
 */
void adjustPassword(void);
/*
 * Description :
 * Function responsible for get a password from the user and send it to the CONTROL ECU
 */
void getSendPassword(void);
/*
 * Description :
 * This function will be called when the ISR code of the timer is happened. This function will increment the variable
 * responsible for unlock the system.
 */
void timerCounter(void);


void getSendPassword(){
	/* For the for loop */
	uint8 count=0;
	/* To save the password in it temporary*/
	uint8 password[PASSWORD];
	/*Clearing the screen */
	LCD_clearScreen();
	LCD_displayStringRowColumn(0,0,(uint8 *)"Please Ent Pass:");
	LCD_moveCursor(1,5);
	for(count=0 ; count<PASSWORD;count++){
		password[count]=KEYPAD_getPressedKey();
		LCD_displayCharacter('*');
		/* to not input more than one character
		 * */
		_delay_ms(400);
	}
	/* '=' is equivalent to enter */
	while(KEYPAD_getPressedKey() != 13);
	_delay_ms(400);
	/* send all the characters to the CONTROL ECU*/
	for(count=0;count<PASSWORD;count++){
		/* check if the HMI ECU is ready */
		UART_sendByte(ARE_MC2_READY);
		/* until HMI ECU send that it is ready which is MC2_RREADY*/
		while(UART_receiveByte() != MC2_READY);
		/* Sending the characters of the password one by one*/
		UART_sendByte(password[count]);
	}

}

void adjustPassword(){
	uint8 flag=FALSE;
	while(flag == FALSE){
		/* Get the fist password from the user and send it to the CONTROL ECU*/
		getSendPassword();
		LCD_clearScreen();
		LCD_displayStringRowColumn(0,0,(uint8 *)"Please Re-Enter");
		LCD_displayStringRowColumn(1,0,(uint8 *)"The Password");
		/* delay to display the message on the screen (to take its time)*/
		_delay_ms(1000);
		/* Get the fist password from the user and send it to the CONTROL ECU*/
		getSendPassword();
		/* Looping until CONTROL_ECU send ARE_MC1_READY*/
		while(UART_receiveByte() != ARE_MC1_READY);
		/* HMI ECU sends to the CONTROL ECU i am ready which is MC1_ECU*/
		UART_sendByte(MC1_READY);
		/* Receive the compare state of the 2-passwords from the CONTROL ECU*/
		flag=UART_receiveByte();
		/* fist state if the password is right, we will display on the screen the message below for 1 second */
		if(flag == PASSWORD_IS_RIGHT)
		{
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,(uint8 *)"    Matching    ");
			LCD_displayStringRowColumn(1,0,(uint8 *)"Saving Pass.....");
			_delay_ms(1000);
		}
		/* fist state if the password is wrong, we will display on the screen the message below for 1 second and loop again */
		else if(flag == PASSWORD_IS_WRONG)
		{
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,(uint8 *)"  Dis-Matching  ");
			LCD_displayStringRowColumn(1,0,(uint8 *)"Please try Again");
			_delay_ms(1000);
		}
	}
}

void displayMenu(){
	/* Clearing the screen*/
	LCD_clearScreen();
	/* displaying the menu screen in different rows*/
	LCD_displayStringRowColumn(0,0,(uint8 *)"(+)   Open Door");
	LCD_displayStringRowColumn(1,0,(uint8 *)"(-)  Change Pass");
}


void getChoiceFromUser(void){
	/* getting the choice from the user */
	choice=KEYPAD_getPressedKey();
	_delay_ms(400);
}
/* This function is responsible for sending a choice to CONTROL_ECU
 * */
void mc1SendChoice(){
	UART_sendByte(ARE_MC2_READY);
	while(UART_receiveByte() != MC2_READY);
	UART_sendByte(choice);
}
/* This function is responsible for receiving a choice from CONTROL_ECU
 * */
void mc1ReceiveChoice(){
	while(UART_receiveByte() != ARE_MC1_READY);
	UART_sendByte(MC1_READY);
	choice=UART_receiveByte();
}

void timerCounter(){
	/* increment the timer counter to controlling the time of door and lock */
	g_timerCount++;
}

int main(){
	/* To enable the global interrupt */
	SREG|=(1<<7);
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
	/*initialize the UART with a specific UART frame based on structure*/
	UART_ConfigType configtype={PARITY_DISABLED,ONE_BIT,EIGHT_BITS,115200};
	/*sending the structure to the UART for initialization*/
	UART_init(&configtype);
	/* initialize the LCD */
	LCD_init();
	/*adjusting the password
	 * note: HMI ECU is responsible only for display on LCD and getting a character from keypad ..
	 * all of this is happening based on the CONTROL ECU instruction
	 * */
	while(1){
		/* HMI ECU receives decision from the CONTROL ECU and based on this decision, the HMI ECU will implement whatever the CONTROL
		 * ECU wants */
		mc1ReceiveChoice();

		/* First Case : if the CONTROL ECU send adjust the password */
		if(choice == ADJUST_PASSWORD){
			adjustPassword();
		}

		/* Second Case : if the CONTROL ECU send password is saved, a message will display on the screen based on that state for only
		 * one second */
		else if(choice == PASSWORD_IS_SAVED){
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,(uint8 *)"Password Is ");
			LCD_displayStringRowColumn(1,0,(uint8 *)"Saved ");
			/*This delay is made for the lcd to take its time for displaying*/
			_delay_ms(1000);
		}

		/* Third Case : if the CONTROL ECU send password is wrong, a message will display on the screen based on that state for only
		 * one second */
		else if(choice == PASSWORD_IS_WRONG){
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,(uint8 *)"Please Enter the");
			LCD_displayStringRowColumn(1,0,(uint8 *)"Correct Password");
			_delay_ms(1000);
		}

		/* Fourth Case : if the CONTROL ECU send password is right(open door), a message will display on the screen based on that state
		 *  for only one second */
		else if(choice == PASSWORD_IS_RIGHT_OPEN){
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,(uint8 *)"Pass. is right");
			LCD_displayStringRowColumn(1,0,(uint8 *)"Door will open ");
			_delay_ms(1000);
		}

		/* Fifth Case : if the CONTROL ECU send password is right(setting a new password), a message will display on the screen based
		 *  on that state for only one second */
		else if(choice == PASSWORD_IS_RIGHT){
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,(uint8 *)"Pass. is right");
			LCD_displayStringRowColumn(1,0,(uint8 *)"Setting new pass");
			_delay_ms(1000);
		}

		/* Sixth Case : if the CONTROL ECU send display the menu, the HMI ECU will call the display menu function */
		else if(choice== DISPLAY_MENU){
			/* Displaying the menu to ask the user which choice he want*/
			displayMenu();
		}

		/* Seventh Case : if the CONTROL ECU send get choice from user, the HMI ECU will call the get choice from user function*/
		else if(choice == GET_CHOICE){
			getChoiceFromUser();
			mc1SendChoice();
		}

		/* Eighth Case : if the CONTROL ECU send not valid choice , a message will display on the screen based on that state for only
		 * one second */
		else if(choice == NOT_VALID_CHOICE){
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,(uint8 *)"Not Valid Choice");
			LCD_displayStringRowColumn(1,0,(uint8 *)"Re-Enter again  ");
			_delay_ms(1000);

		}

		/* Ninth Case : if the CONTROL ECU send get choice , the HMI ECU will call the get and send password function */
		else if (choice == GET_PASSWORD){
			getSendPassword();
		}

		/* Tenth Case : if the CONTROL ECU send lock the system , a message will display on the screen for 1-minute */
		else if(choice == LOCK_THE_SYSTEM){
			/* put zero in the g_timerCount to start from zero (for safety) */
			g_timerCount=0;
			/* Clearing the screen*/
			LCD_clearScreen();
			/* Display the lock message for 1 minute*/
			LCD_displayStringRowColumn(0,0,(uint8 *)"System Is Locked");
			/* Initialize the TIMER1 by the structure that control the lock time*/
			TIMER_init(&configtype1);
			/* while loop until the timer reach a 1 minute*/
			while(g_timerCount<10);
			/* De-initializing the timer */
			TIMER_DeInit(TIMER1);
			/* put zero in the g_timerCount for the next time*/
			g_timerCount=0;
		}

		/* Eleventh case : if the CONTROL ECU door is opening , a message will display on the screen based on that state for a
		 * 15 seconds */
		else if(choice == DOOR_IS_OPENING){
			/* put zero in the g_timerCount to start from zero (for safety) */
			g_timerCount=0;
			/* Initialize the TIMER1 by the structure that control the door time (opening and closing) */
			TIMER_init(&configtype2);
			/* Clearing the screen*/
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,(uint8 *)"DOOR IS OPENING");
			/* looping until it reach 15 seconds (Opening time = 15 seconds)*/
			while(g_timerCount < 30);
		}

		/* 	Twelfth case : if the CONTROL ECU door is opened , a message will display on the screen based on that state for a
		 *  3 seconds */
		else if(choice == DOOR_IS_OPENED){
			/* Clearing the screen for a new message */
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,(uint8 *)"DOOR IS OPENED");
			/* looping until it reach 18 seconds (Holding time = 3 seconds)
			 * Why 18 seconds ??
			 * --> 15 seconds for the Opening time and 3 second for the holding time
			 * */
			while(g_timerCount < 39);
		}

		/*  Thirteenth case : if the CONTROL ECU door is closing , a message will display on the screen based on that state for a
		 *  15 seconds */
		else if(choice == DOOR_IS_CLOSING){
			/* Clearing the screen for a new message */
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,(uint8 *)"DOOR IS CLOSING");
			/* looping until it reach 33 seconds (Closing time = 15 seconds)
			 * Why 33 seconds ??
			 * --> 15 seconds for the Opening time, 3 second for the holding time and 15 seconds for the closing
			 *  time
			 * */
			while(g_timerCount < 66);
		}

		/*  Fourteenth case : if the CONTROL ECU door is closed , a message will display on the screen based on that state for a
		 *  2 seconds */
		else if(choice == DOOR_IS_CLOSED){
			/* Clearing the screen for a new message */
			LCD_clearScreen();
			LCD_displayStringRowColumn(0,0,(uint8 *)"DOOR IS CLOSED");
			/*waiting 2-second to make the message display on the screen*/
			while(g_timerCount < 70);
			/* De-init the TIMER1 */
			TIMER_DeInit(TIMER1);
			/* put zero into the g_timerCount for the next time */
			g_timerCount=0;
		}
	}




}
