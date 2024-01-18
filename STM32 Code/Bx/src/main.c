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
			

int main(void)
{

	for(;;);

	setupGPIO(); // Pa0, Pa1 key
	setupUART(); // utilizing usart4 to communicate with Bx
	sendATCheck();
}

void sendATCheck()
{
	  char* testTX[2] = {'A','T'};
	  char* testRX[2] = {};

	  // send "AT" test command
	  for(uint32_t i = 0; i < 2;i++){
		  while(!(USART4->ISR & USART_ISR_TXE)){}
		  USART4 -> TDR = testTX[i];
	  }
	  // expect "OK" from Bx
	  for(int i = 0; i < 2; i++)
	  {
		  while (!(USART4->ISR & USART_ISR_RXNE)) {}
		  testRX[i] = USART4->RDR;
	  }
}
void setupGPIO(){
	RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;                    // Enable GPIO clock
	GPIOA -> MODER |= GPIO_MODER_MODER0_1; 					// PA0 is TX to Bluetooth (Alt Mode)
	GPIOA -> AFR[0] |= 0x4;								 	// PA0 selected for USART4_TX
	GPIOA -> MODER |= GPIO_MODER_MODER1_1; 					// PA1 is RX from Bluetooth (Alt Mode)
	GPIOA -> AFR[0] |= 0x4 << 4; 							// PA0 selected for USART4_RX
}

void setupUART(){
	RCC -> APB1ENR |= RCC_APB1ENR_USART4EN;                // clock on for usart5
	USART4 -> CR1 &= ~USART_CR1_UE;                        // turn off for config
	USART4 -> CR1 &= ~USART_CR1_M;                         // 8 bit word length
	USART4 -> CR2 &= ~USART_CR2_STOP;                      // 1 stop bits
	USART4 -> CR1 &= ~USART_CR1_PCE;                       // parity disable
	USART4 -> CR1 &= ~USART_CR1_OVER8;                     // 16x oversampling
	USART4 -> BRR = 48000000 / 9600;					   // 9600 baud rate
	USART4 -> CR1 |= USART_CR1_TE;				           // transmitter enable
	USART4 -> CR1 |= USART_CR1_RE;				           // receiver enable
	USART4 -> CR1 |= USART_CR1_UE;                         // enable usart
}
