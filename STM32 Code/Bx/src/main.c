/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
#include <string.h>
			

// main game receiver characteristics for right glove (MASTER)
const char *RX_MACADDR_R = "E4E11295D8C6"; // known MAC address for Main RX BT on STM32F0 PCB
const char *RX_NAME_R   = "TetrisR";
const char *RX_ADVINT_R  = "100ms";

// right glove transmitter characteristics (SLAVE)
const char *TX_MACADDR_R = "609866F5B565";
const char *TX_NAME_R = "GloveR";
const char *TX_ADVINT_R  = "100ms";

int main(void)
{

//	for(;;);

	setupGPIO(); // Pa0, Pa1 key
<<<<<<< HEAD
	setupUART(); // utilizing USART5 to communicate with Bx
	sendATCheck(); // send AT handshake
=======
	setupUART(); // utilizing usart4 to communicate with Bx
//	sendATCheck(); // send AT handshake
>>>>>>> 4d2af454ef6cea91f6e97b731f0a21b01a5a459c
	//sendBxWake();  // send random long string to wakeup NOT WORKING
	//sendATAddr();  // send AT command to get MAC address
	//sendBxName();  // send AT command to rename
	//uint32_t adInt = getAdvInterval(); // return advertising interval parameter for module in ms
	//setATRole("1"); // set to master device
	sendATCon();
	//getATImme();
	//setATImme("1"); // dont start in WORK mode out of reset
	//sendATStart();
<<<<<<< HEAD
	//sendATDisc(); // let master discover peripherals

	for(int i = 0; i < 256; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = i;
	}


=======
//	sendATDisc(); // let master discover peripherals

	char startBuff[256] = {};
	for(uint32_t i = 0; i < 256; i++)
	{
		while(!(USART4->ISR & USART_ISR_RXNE)){}
		startBuff[i] = USART4 -> RDR;
	}
>>>>>>> 4d2af454ef6cea91f6e97b731f0a21b01a5a459c
}

void sendATStart()
{
	char * startTx = "AT+START";
	char startRx[8]   = {};

	for(uint32_t i = 0; startTx[i] != '\0'; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = startTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_RXNE)){}
		startRx[i] = USART5 -> RDR;
	}
}

void sendATCon()
{
	char * conTx = "AT+CON609866F5B565"; //FB?
	char   conRx[8] = {};

	for(uint32_t i = 0; conTx[i] != '\0'; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = conTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_RXNE)){}
		conRx[i] = USART5 -> RDR;
	}
}

void setATImme(char * mode)
{
	char immeTx[8] = "AT+IMME";
	strcat(immeTx, mode); // append parameter
	char immeRx[8] = {};

	for(uint32_t i = 0; immeTx[i] != '\0'; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = immeTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_RXNE)){}
		immeRx[i] = USART5 -> RDR;
	}
}

void getATImme()
{
	char *immeTx = "AT+IMME?";
	char immeRx[8] = {};
	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = immeTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_RXNE)){}
		immeRx[i] = USART5 -> RDR;
	}
}


void sendATDisc()
{
	char *discTx = "AT+DISC?";
	char discRx[100] = {}; //"OK+DISCS"
	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = discTx[i];
	}

	for(uint32_t i = 0; i < 100; i++)
	{
		while(!(USART5->ISR & USART_ISR_RXNE)){}
		discRx[i] = USART5 -> RDR;
	}
}

void setATRole(char *role)
{
	char roleTx[8] = "AT+ROLE";
	strcat(roleTx, role); // append parameter
	char roleRx[8] = {};

	for(uint32_t i = 0; roleTx[i] != '\0'; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = roleTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_RXNE)){}
		roleRx[i] = USART5 -> RDR;
	}
	//return strcmp(roleRx + 7, role); // broken comparison

}

int getAdvInterval()
{
	char* advTx = "AT+ADVI?";
	char advRx[8] = {};

	// send message to obtain advertising interval
	for(uint32_t i = 0; advTx[i] != '\0'; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = advTx[i];
	}

	// expect "OK+GET:[P]" from Bx
	for(int i = 0; i < 8; i++)
	{
		while (!(USART5->ISR & USART_ISR_RXNE)) {}
		advRx[i] = USART5->RDR;
	}
	return (uint32_t) advRx[7];
}

void sendBxName()
{
	  // set name of bluetooth device
	  char* nameTx = "AT+NAMETetrisR";
	  char nameRx[14]  = {};

	  // send "AT" test command
	  for(uint32_t i = 0; nameTx[i] != '\0'; i++)
	  {
		  while(!(USART5->ISR & USART_ISR_TXE)){}
		  USART5 -> TDR = nameTx[i];
	  }

	  for(int i = 0; i < 14; i++)
	  	{
	  		while (!(USART5->ISR & USART_ISR_RXNE)) {}
	  		nameRx[i] = USART5->RDR;
	  	}
	  char *temp = strchr(nameRx, 'T');
	  return strcmp(RX_NAME_R, temp);
}

void sendBxWake()
{
	  // send random strings >80 chars to wakie wakie
	  char wakeTx[1001];
	  for(uint32_t k = 0; k < 1001; k++)
	  {
		  wakeTx[k] = 0x42; // send in some 8 byte char
	  }
	  //char* wakeTx = "mA44crMralwkGswlWusRSkReoLCnvJU4xxoRKUNnkYRLU9HPlekPb1Eegu3HSlpTze3nphT1oW2PEfD4bsj5UMQZn2KRSLHRg0YZ";
	  char wakeRx[7] = {};
	  // send "AT" test command
	  for(uint32_t i = 0; i < 1001; i++)
	  {
		  while(!(USART5->ISR & USART_ISR_TXE)){}
		  USART5 -> TDR = wakeTx[i];
	  }

	  // expect "OK" from Bx
	  for(int i = 0; i < 7; i++)
	  {
		  while (!(USART5->ISR & USART_ISR_RXNE)) {}
		  wakeRx[i] = USART5->RDR;
	  }
	  return 0;
}

void sendATAddr()
{
	  uint8_t testAddrTx[8] = {'A','T','+','A','D','D','R','?'};
	  uint8_t testAddrRx[19] = {};

	  // send "AT+Addr?" command
	  for(uint32_t i = 0; i < 8; i++){
		  while(!(USART5->ISR & USART_ISR_TXE)){}
		  USART5 -> TDR = testAddrTx[i];
	  }

	  // expect "OK+Mac Addr" from Bx
	  for(int i = 0; i < 19; i++)
	  {
		  while (!(USART5->ISR & USART_ISR_RXNE)) {}
		  testAddrRx[i] = USART5->RDR;
	  }
	  char *temp = strchr(testAddrRx, 'E'); // extract MAC addr
	  return strcmp(RX_MACADDR_R, temp);
}

void sendATCheck()
{
	  uint8_t testTX[2] = {'A','T'};
	  uint8_t testRX[2];

	  // send "AT" test command
	  for(uint32_t i = 0; i < 2; i++){
		  while(!(USART5->ISR & USART_ISR_TXE)){}
		  USART5 -> TDR = testTX[i];
	  }

	  // expect "OK" from Bx
	  for(int i = 0; i < 2; i++)
	  {
		  while (!(USART5->ISR & USART_ISR_RXNE)) {}
		  testRX[i] = USART5->RDR;
	  }
	  return strcmp(testRX, "OK");
}

void setupGPIO(){
	RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;                    // Enable GPIO clock
	GPIOB -> MODER |= GPIO_MODER_MODER3_1; 					// PB3 is TX to Bluetooth (Alt Mode)
	GPIOB -> AFR[0] |= 0x4 << 12;								 	// PB3 selected for USART5_TX
	GPIOB -> MODER |= GPIO_MODER_MODER4_1; 					// PB4 is RX from Bluetooth (Alt Mode)
	GPIOB -> AFR[0] |= 0x4 << 16; 							// PB4 selected for USART5_RX
}

void setupUART(){
	RCC -> APB1ENR |= RCC_APB1ENR_USART5EN;                // clock on for USART5
	USART5 -> CR1 &= ~USART_CR1_UE;                        // turn off for config
	USART5 -> CR1 &= ~USART_CR1_M;                         // 8 bit word length
	USART5 -> CR2 &= ~USART_CR2_STOP;                      // 1 stop bits
	USART5 -> CR1 &= ~USART_CR1_PCE;                       // parity disable
	USART5 -> CR1 &= ~USART_CR1_OVER8;                     // 16x oversampling
	USART5 -> BRR = 48000000 / 9600;					   // 9600 baud rate
	USART5 -> CR1 |= USART_CR1_TE;				           // transmitter enable
	USART5 -> CR1 |= USART_CR1_RE;				           // receiver enable
	USART5 -> CR1 |= USART_CR1_UE;                         // enable usart

	while(!(((USART5 -> ISR) & USART_ISR_TEACK) && ((USART5 -> ISR) & USART_ISR_REACK))); // wait for TEACK and REACK to be set
}

