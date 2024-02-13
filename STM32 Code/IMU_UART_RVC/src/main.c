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
#include "stdlib.h"
#include "stdarg.h"
#include "stdint.h"
#include "stdio.h"

extern void nano_wait(int);

struct Packet
{
	float yaw;
	float pitch;
	float roll;
	float x_acl;
	float y_acl;
	float z_acl;
};

struct Packet curr_packet;

#define FIFOSIZE 19
uint8_t data_fifo[FIFOSIZE];
int fifo_offset = 0;

float convertToFloat(uint8_t lsb, uint8_t msb)
{
	int16_t result = ((int16_t)msb << 8) | lsb;
	int8_t decimal_part = result % 100;
	int16_t integer_part = result / 100;
	float final_number = integer_part + (float)decimal_part / 100.0;
	return final_number;
}

void printVal()
{
	char* output = malloc(sizeof(char) * 70);
	sprintf(output, "Y:%6.2f, P:%6.2f, R:%6.2f, ",
//			"X-acl:%6.2f, Y-acl:%6.2f, Z-acl:%6.2f \n\r",
//			curr_packet.yaw, curr_packet.pitch, curr_packet.roll,
			curr_packet.x_acl, curr_packet.y_acl, curr_packet.z_acl);

//
//	sprintf(output, "%6.3f, %6.3f, %6.3f, %6.3f, %6.3f, %6.3f \n",
//			curr_packet.yaw, curr_packet.pitch, curr_packet.roll,
//			curr_packet.x_acl, curr_packet.y_acl, curr_packet.z_acl);
	debugSendString(output);
	free(output);
}

void DMA1_CH2_3_DMA2_CH1_2_IRQHandler()
{
	DMA1 -> IFCR |= DMA_IFCR_CTCIF3; // clear flag
	curr_packet.yaw = convertToFloat(data_fifo[3], data_fifo[4]);
	curr_packet.pitch = convertToFloat(data_fifo[5], data_fifo[6]);
	curr_packet.roll = convertToFloat(data_fifo[7], data_fifo[8]);
	curr_packet.x_acl = convertToFloat(data_fifo[9], data_fifo[10]);
	curr_packet.y_acl = convertToFloat(data_fifo[11], data_fifo[12]);
	curr_packet.z_acl = convertToFloat(data_fifo[13], data_fifo[14]);
	printVal();
}

void debugSend(char txdata) {
  while(!(USART5->ISR & USART_ISR_TXE)){}
      USART5->TDR = txdata;
}

void debugSendString(char* data) {
  int i = 0;
  while(data[i] != '\0') {
    debugSend(data[i]);
    i++;
  }
}

void setup_serial(void)
{
    RCC->AHBENR |= 0x00180000;
    GPIOC->MODER  |= 0x02000000;
    GPIOC->AFR[1] |= 0x00020000;
    GPIOD->MODER  |= 0x00000020;
    GPIOD->AFR[0] |= 0x00000200;
    RCC->APB1ENR |= 0x00100000;
    USART5->CR1 &= ~0x00000001;
    USART5->CR1 |= 0x00008000;
    USART5->BRR = 0x340;
    USART5->CR1 |= 0x0000000c;
    USART5->CR1 |= 0x00000001;
}

void setup_IMU_UART(void)
{
	//PB5 -> Active-low IMU reset
	//PB6 -> UART TX (floating)
	//PB7 -> UART RX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
	GPIOB->MODER |= GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1; // AFR mode (UART)
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFRL6 | GPIO_AFRL_AFRL7); // AF0
	nano_wait(1000000);
	GPIOB->BSRR |= GPIO_BSRR_BS_5; // Make PB5 high to avoid reset

	RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;                // clock on for usart1
	USART1 -> CR1 &= ~USART_CR1_UE;                        // turn off for config
	USART1 -> CR1 &= ~USART_CR1_M;                         // 8 bit word length
	USART1 -> CR2 &= ~USART_CR2_STOP;                      // 1 stop bits
	USART1 -> CR1 &= ~USART_CR1_PCE;                       // parity disable
	USART1 -> CR1 &= ~USART_CR1_OVER8;                     // 16x oversampling
	USART1 -> BRR = 48000000 / 115200;					   // 115200 baud rate
	USART1 -> CR1 |= USART_CR1_RE;				           // receiver enable
	USART1 -> CR3 |= USART_CR3_DMAR;					   // come dma steal my data
	USART1 -> CR1 |= USART_CR1_UE;                         // enable usart
}

void setup_DMA()
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	DMA1_Channel3->CCR &= ~DMA_CCR_EN;      // turn off enable bit
	DMA1_Channel3->CMAR = (uint32_t) data_fifo;    	 // cmar address (read to here)
	DMA1_Channel3->CPAR = (uint32_t) &(USART1->RDR); // cpar address (read from here)
	DMA1_Channel3->CNDTR = FIFOSIZE;        // cndtr set to 8
	DMA1_Channel3->CCR &= ~DMA_CCR_DIR;     // read from perip
	DMA1_Channel3->CCR &= ~DMA_CCR_MSIZE;   // 16 bit m size
	DMA1_Channel3->CCR &= ~DMA_CCR_PSIZE;   // 16 bit p size
	DMA1_Channel3->CCR |= DMA_CCR_MINC;     // increment CMAR
	DMA1_Channel3->CCR |= DMA_CCR_CIRC;		// circular mode
	DMA1_Channel3->CCR |= DMA_CCR_PL_0 | DMA_CCR_PL_1;  // highest priority
	DMA1_Channel3->CCR |= DMA_CCR_TCIE; 				// int on transfer complete
	DMA1_Channel3->CCR |= DMA_CCR_EN;
	NVIC -> ISER[0] |= (1<<DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
}

void printInt(int val, char* pin) {
  char* output = malloc(sizeof(char)*30);
  sprintf(output, "%s value is %d\r\n", pin, val);
  debugSendString(output);
  free(output);
}


int main(void)
{
  setup_serial();
  setup_DMA();
  setup_IMU_UART();

  for(int i = 0; i<10000000;)
  {
	  i++;
  }
}



