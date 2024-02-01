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
#include <stdint.h>

void toggle_IMU_RST(uint8_t num);
void toggle_NSS(uint8_t num);
extern void nano_wait(int);

uint8_t tx_cargo[10] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A};
uint8_t rx_cargo[20] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//**********************************************
// function to configure GPIO as needed
// PB12 => SPI2_NSS [manually driven]
// PB13 => SPI2_SCK
// PB14 => SPI2_MOSI
// PB15 => SPI2_MISO
// PB8 => INT (input to micro)
// PB10 => RST (output from micro)
//
void setup_GPIO(void)
{
	RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB -> MODER &= ~ (GPIO_MODER_MODER8 & GPIO_MODER_MODER10 & GPIO_MODER_MODER12 & GPIO_MODER_MODER13 & GPIO_MODER_MODER14 & GPIO_MODER_MODER15);
	GPIOB -> MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1 | GPIO_MODER_MODER15_1 | GPIO_MODER_MODER10_0;
	GPIOB -> AFR[1] &= ~ (GPIO_AFRH_AFR13 & GPIO_AFRH_AFR14 & GPIO_AFRH_AFR15);

	// Perform IMU cleaning
	toggle_IMU_RST(1);
	nano_wait(10);
	toggle_IMU_RST(0);
	toggle_NSS(0);
}

//*********************************************
// function to assert IMU reset pin (active-low)
// num == 1 => PB10 becomes 0
// num == 0 => PB10 becomes 1
//
void toggle_IMU_RST(uint8_t num)
{
	if(num == 1)
		GPIOB -> BSRR |= GPIO_BSRR_BR_10;
	else if(num == 0)
		GPIOB -> BSRR |= GPIO_BSRR_BS_10;
}


void toggle_NSS(uint8_t num)
{
	if(num == 1)
		GPIOB -> BSRR |= GPIO_BSRR_BR_12;
	else if(num == 0)
		GPIOB -> BSRR |= GPIO_BSRR_BS_12;
}

//**********************************************
// function to configure SPI2 for use with IMU hub
//
void setup_SPI(void)
{
	RCC -> APB1ENR |= RCC_APB1ENR_SPI2EN;       // enable spi2 clock
	SPI2 -> CR1 &= ~SPI_CR1_SPE;                // clear enable bit (to update config)
	SPI2 -> CR1 |= SPI_CR1_MSTR;                // master mode
	SPI2 -> CR1 |= (SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2); // lowest baud rate (48Mhz / 256)
	SPI2 -> CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;    // 8 bit size of data
	SPI2 -> CR2 |= SPI_CR2_SSOE | SPI_CR2_NSSP; // output enable and nssp enable
	SPI2 -> CR2 |= SPI_CR2_TXDMAEN |  SPI_CR2_FRXTH; //| SPI_CR2_RXDMAEN | SPI_CR2_RXNEIE ;				// dma transfer on transmit buffer empty
}

//**********************************************
// function to setup DMA Channel 5 (used for TX data over SPI2)
//
void setup_DMA_tx(void)
{
    RCC -> AHBENR |= RCC_AHBENR_DMA1EN;     // clock of dma1 on

	// DMA Channel 5 puts data on TX buffer of SPI2
    DMA1_Channel5->CCR &= ~DMA_CCR_EN;      // turn off enable bit
    DMA1_Channel5->CMAR = (uint32_t) tx_cargo;    // cmar address (read from here)
    DMA1_Channel5->CPAR = (uint32_t) &(SPI2->DR); // cpar address (read to here)
    DMA1_Channel5->CNDTR = 10;              // number of bytes to transfer
    DMA1_Channel5->CCR |= DMA_CCR_DIR;      // read from mem
    DMA1_Channel5->CCR &= ~DMA_CCR_MSIZE;   // 8 bit (1B) m size
    DMA1_Channel5->CCR &= ~DMA_CCR_PSIZE;   // 8 bit (1B) p size
    DMA1_Channel5->CCR |= DMA_CCR_MINC;     // increment CMAR
//    DMA1_Channel5->CCR |= DMA_CCR_CIRC;
}

////**********************************************
//// function to setup DMA Channel 4 (used for RX data over SPI2)
////
void setup_DMA_rx(void)
{
	RCC -> AHBENR |= RCC_AHBENR_DMA1EN;	// clock of dma2 on

	// DMA Channel 4 pulls data from RX buffer of SPI2
    DMA1_Channel4->CCR &= ~DMA_CCR_EN;      // turn off enable bit
	DMA1_Channel4->CMAR = (uint32_t) rx_cargo;    // cmar address (read to here)
	DMA1_Channel4->CPAR = (uint32_t) &(SPI2->DR); // cpar address (read from here)
	DMA1_Channel4->CNDTR = 20;              // cndtr set to 8
	DMA1_Channel4->CCR &= ~DMA_CCR_DIR;     // read from perip
	DMA1_Channel4->CCR |= DMA_CCR_MSIZE_0;  // 16 bit m size
	DMA1_Channel4->CCR |= DMA_CCR_PSIZE_0;  // 16 bit p size
	DMA1_Channel4->CCR |= DMA_CCR_MINC;     // increment CMAR
}


void init_exti(void)
{
	RCC -> APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	SYSCFG -> EXTICR[2] |=  SYSCFG_EXTICR3_EXTI8_PB;
	EXTI -> IMR |= EXTI_IMR_MR8;
	EXTI -> RTSR |= EXTI_RTSR_TR8;
	NVIC -> ISER[0] |= (1 << EXTI4_15_IRQn);
}

void EXTI4_15_IRQHandler(void) {
    EXTI -> PR |= EXTI_PR_PR10;
    EXTI -> IMR &= ~(EXTI_IMR_MR8);
    toggle_NSS(1);
    SPI2 -> CR1 |= SPI_CR1_SPE;
    DMA1_Channel5->CCR |= DMA_CCR_EN;
	while((DMA1->ISR & DMA_ISR_TCIF5) == 0) {}
	nano_wait(1000000); //1ms
	SPI2 -> CR1 &= ~SPI_CR1_SPE;
	DMA1_Channel5->CCR &= ~DMA_CCR_EN;
	toggle_NSS(0);
}

int main(void)
{
	setup_GPIO();
	setup_SPI();
	init_exti();
	setup_DMA_tx();
//	setup_DMA_rx();


}

