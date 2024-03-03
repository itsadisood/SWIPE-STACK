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

// ***************** data structure sizing constants ****************
#define FIFOSIZE 					19								// # of bytes to receive from IMU into temp buffer before processing
#define PREDICTIONS_PER_SECOND 	4 								// # of times the entire toolchain runs to display a output
#define PACKET_SIZE				100 / PREDICTIONS_PER_SECOND 	// # of samples that must be filled in before calculating an output

// ***************** gesture senstivity constants ****************
#define DEADZONE_INT 				5
#define ROLL_THRESHOLD 			30

uint8_t data_fifo[FIFOSIZE];

struct Packet
{
	float roll  [PACKET_SIZE];
	float x_acl [PACKET_SIZE];
	float z_acl [PACKET_SIZE];
};
struct Packet curr_packet;

int curr_sample_num = 0;

float convertToFloat(uint8_t lsb, uint8_t msb)
{
	int16_t result = ((int16_t)msb << 8) | lsb;
	int8_t decimal_part = result % 100;
	int16_t integer_part = result / 100;
	float final_number = integer_part + (float)decimal_part / 100.0;
	return final_number;
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

void debugSendInt(int value) {
	char buffer[20];
	sprintf(buffer, "%d\r\n", value); // Format integer value as a string
	for (int i = 0; buffer[i] != '\0'; i++) {
		debugSend(buffer[i]); // Send each character of the string
	}
}

void printVal()
{
	char* output = malloc(sizeof(char) * 70);
	sprintf(output, "Roll:%6.2f,   X-acl:%6.2f,   Z-acl:%6.2f \n\r",
				curr_packet.roll[curr_sample_num], curr_packet.x_acl[curr_sample_num], curr_packet.z_acl[curr_sample_num]);
	debugSendString(output);
	free(output);
}


void detect_roll(float sampled_roll_values[]) {
    int left_roll_count = 0;
    int right_roll_count = 0;

    for (int i = 0; i < PACKET_SIZE; i++)
    {
        // Check if roll is outside deadzones and thresholds
        if (sampled_roll_values[i] < -(DEADZONE_INT + ROLL_THRESHOLD))
        {
            left_roll_count++; // Increment roll count for significant roll samples
        }
        else if(sampled_roll_values[i] > DEADZONE_INT + ROLL_THRESHOLD)
        {
        	right_roll_count++;
        }
    }

    // Check if the number of significant roll samples exceeds a threshold (e.g., 5 out of 10)
    if (left_roll_count >= PACKET_SIZE/2)
    {
        debugSendString("******LEFT ROLL DETECTED *****\n\r"); // Send message indicating roll detection

    }
    else if (right_roll_count >= PACKET_SIZE/2)
	{
		debugSendString("******RIGHT ROLL DETECTED *****\n\r"); // Send message indicating roll detection

	}
}

void DMA1_CH2_3_DMA2_CH1_2_IRQHandler()
{
	//only have 10 ms for this processing

	if(data_fifo[0] == 0xAA && data_fifo[1] == 0XAA) // enforce header check
	{
		if(curr_sample_num == PACKET_SIZE)
		{
			// do prediction call
			curr_sample_num = 0;
			debugSendString("Ready to predict\n\r");
			detect_roll(curr_packet.roll);
//			printVal();
		}
		else
		{
			// fill packet buffer
			DMA1 -> IFCR |= DMA_IFCR_CTCIF3; // clear flag
			curr_packet.roll[curr_sample_num] = convertToFloat(data_fifo[7], data_fifo[8]);
			curr_packet.x_acl[curr_sample_num] = convertToFloat(data_fifo[9], data_fifo[10]);
			curr_packet.z_acl[curr_sample_num] = convertToFloat(data_fifo[13], data_fifo[14]);
			curr_sample_num += 1;
//			printVal();
		}
	}
	else // got mis-aligned, cleanup and start again.
	{
		debugSendString("Misaligned. Fixing now.");
		DMA1_Channel3->CCR &= ~DMA_CCR_EN;
		setup_DMA();
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
	//PB5 -> Active-low IMU reset (output)
	//PB6 -> UART TX (floating)
	//PB7 -> UART RX

	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
	GPIOB->MODER |= GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1; // AFR mode (UART)
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFRL6 | GPIO_AFRL_AFRL7); // AF0
	nano_wait(1000000);

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

int main(void)
{
  setup_serial();
  setup_DMA();
  setup_IMU_UART();

  while(1)
  {
	  asm("wfi");
  }

}
