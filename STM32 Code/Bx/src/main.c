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
	setupUART(); // utilizing usart4 to communicate with Bx
//	sendATCheck(); // send AT handshake
	//sendBxWake();  // send random long string to wakeup NOT WORKING
	//sendATAddr();  // send AT command to get MAC address
	//sendBxName();  // send AT command to rename
	//uint32_t adInt = getAdvInterval(); // return advertising interval parameter for module in ms
	//setATRole("1"); // set to master device
	sendATCon();
	//getATImme();
	//setATImme("1"); // dont start in WORK mode out of reset
	//sendATStart();
//	sendATDisc(); // let master discover peripherals

	char startBuff[256] = {};
	for(uint32_t i = 0; i < 256; i++)
	{
		while(!(USART4->ISR & USART_ISR_RXNE)){}
		startBuff[i] = USART4 -> RDR;
	}
}

void sendATStart()
{
	char * startTx = "AT+START";
	char startRx[8]   = {};

	for(uint32_t i = 0; startTx[i] != '\0'; i++)
	{
		while(!(USART4->ISR & USART_ISR_TXE)){}
		USART4 -> TDR = startTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART4->ISR & USART_ISR_RXNE)){}
		startRx[i] = USART4 -> RDR;
	}
}

void sendATCon()
{
	char * conTx = "AT+CON609866F5B565"; //FB?
	char   conRx[8] = {};

	for(uint32_t i = 0; conTx[i] != '\0'; i++)
	{
		while(!(USART4->ISR & USART_ISR_TXE)){}
		USART4 -> TDR = conTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART4->ISR & USART_ISR_RXNE)){}
		conRx[i] = USART4 -> RDR;
	}
}

void setATImme(char * mode)
{
	char immeTx[8] = "AT+IMME";
	strcat(immeTx, mode); // append parameter
	char immeRx[8] = {};

	for(uint32_t i = 0; immeTx[i] != '\0'; i++)
	{
		while(!(USART4->ISR & USART_ISR_TXE)){}
		USART4 -> TDR = immeTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART4->ISR & USART_ISR_RXNE)){}
		immeRx[i] = USART4 -> RDR;
	}
}

void getATImme()
{
	char *immeTx = "AT+IMME?";
	char immeRx[8] = {};
	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART4->ISR & USART_ISR_TXE)){}
		USART4 -> TDR = immeTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART4->ISR & USART_ISR_RXNE)){}
		immeRx[i] = USART4 -> RDR;
	}
}


void sendATDisc()
{
	char *discTx = "AT+DISC?";
	char discRx[100] = {}; //"OK+DISCS"
	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART4->ISR & USART_ISR_TXE)){}
		USART4 -> TDR = discTx[i];
	}

	for(uint32_t i = 0; i < 100; i++)
	{
		while(!(USART4->ISR & USART_ISR_RXNE)){}
		discRx[i] = USART4 -> RDR;
	}
}

void setATRole(char *role)
{
	char roleTx[8] = "AT+ROLE";
	strcat(roleTx, role); // append parameter
	char roleRx[8] = {};

	for(uint32_t i = 0; roleTx[i] != '\0'; i++)
	{
		while(!(USART4->ISR & USART_ISR_TXE)){}
		USART4 -> TDR = roleTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART4->ISR & USART_ISR_RXNE)){}
		roleRx[i] = USART4 -> RDR;
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
		while(!(USART4->ISR & USART_ISR_TXE)){}
		USART4 -> TDR = advTx[i];
	}

	// expect "OK+GET:[P]" from Bx
	for(int i = 0; i < 8; i++)
	{
		while (!(USART4->ISR & USART_ISR_RXNE)) {}
		advRx[i] = USART4->RDR;
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
		  while(!(USART4->ISR & USART_ISR_TXE)){}
		  USART4 -> TDR = nameTx[i];
	  }

	  for(int i = 0; i < 14; i++)
	  	{
	  		while (!(USART4->ISR & USART_ISR_RXNE)) {}
	  		nameRx[i] = USART4->RDR;
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
		  while(!(USART4->ISR & USART_ISR_TXE)){}
		  USART4 -> TDR = wakeTx[i];
	  }

	  // expect "OK" from Bx
	  for(int i = 0; i < 7; i++)
	  {
		  while (!(USART4->ISR & USART_ISR_RXNE)) {}
		  wakeRx[i] = USART4->RDR;
	  }
	  return 0;
}

void sendATAddr()
{
	  uint8_t testAddrTx[8] = {'A','T','+','A','D','D','R','?'};
	  uint8_t testAddrRx[19] = {};

	  // send "AT+Addr?" command
	  for(uint32_t i = 0; i < 8; i++){
		  while(!(USART4->ISR & USART_ISR_TXE)){}
		  USART4 -> TDR = testAddrTx[i];
	  }

	  // expect "OK+Mac Addr" from Bx
	  for(int i = 0; i < 19; i++)
	  {
		  while (!(USART4->ISR & USART_ISR_RXNE)) {}
		  testAddrRx[i] = USART4->RDR;
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
		  while(!(USART4->ISR & USART_ISR_TXE)){}
		  USART4 -> TDR = testTX[i];
	  }

	  // expect "OK" from Bx
	  for(int i = 0; i < 2; i++)
	  {
		  while (!(USART4->ISR & USART_ISR_RXNE)) {}
		  testRX[i] = USART4->RDR;
	  }
	  return strcmp(testRX, "OK");
}

void setupGPIO(){
	RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;                    // Enable GPIO clock
	GPIOA -> MODER |= GPIO_MODER_MODER0_1; 					// PA0 is TX to Bluetooth (Alt Mode)
	GPIOA -> AFR[0] |= 0x4;								 	// PA0 selected for USART4_TX
	GPIOA -> MODER |= GPIO_MODER_MODER1_1; 					// PA1 is RX from Bluetooth (Alt Mode)
	GPIOA -> AFR[0] |= 0x4 << 4; 							// PA0 selected for USART4_RX
}

void setupUART(){
	RCC -> APB1ENR |= RCC_APB1ENR_USART4EN;                // clock on for usart4
	USART4 -> CR1 &= ~USART_CR1_UE;                        // turn off for config
	USART4 -> CR1 &= ~USART_CR1_M;                         // 8 bit word length
	USART4 -> CR2 &= ~USART_CR2_STOP;                      // 1 stop bits
	USART4 -> CR1 &= ~USART_CR1_PCE;                       // parity disable
	USART4 -> CR1 &= ~USART_CR1_OVER8;                     // 16x oversampling
	USART4 -> BRR = 48000000 / 9600;					   // 9600 baud rate
	USART4 -> CR1 |= USART_CR1_TE;				           // transmitter enable
	USART4 -> CR1 |= USART_CR1_RE;				           // receiver enable
	USART4 -> CR1 |= USART_CR1_UE;                         // enable usart

	while(!(((USART4 -> ISR) & USART_ISR_TEACK) && ((USART4 -> ISR) & USART_ISR_REACK))); // wait for TEACK and REACK to be set
}

