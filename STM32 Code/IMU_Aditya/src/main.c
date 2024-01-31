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

void toggle_IMU_RST(uint8_t num);
void toggle_IMU_NSS(uint8_t num);
void readNow(void);

uint8_t tx_cargo[10] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t rx_cargo[20] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//**********************************************
// function to configure GPIO as needed
// PB12 => SPI2_NSS
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
	GPIOB -> MODER |= GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1 | GPIO_MODER_MODER15_1 | GPIO_MODER_MODER10_0;
	GPIOB -> AFR[1] &= ~ (GPIO_AFRH_AFR13 & GPIO_AFRH_AFR14 & GPIO_AFRH_AFR15);
	toggle_IMU_RST(0);
	toggle_IMU_NSS(0);
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

//**********************************************
// function to assert NSS for IMU (active-low)
// num == 1 => PB12 becomes 0
// num == 0 => PB12 becomes 1
//
void toggle_IMU_NSS(uint8_t num)
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
	SPI2 -> CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;                 // highest baud rate (48Mhz / 2)
	SPI2 -> CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2 | SPI_CR2_DS_3;   // 16 bit size of data
	SPI2 -> CR1 |= SPI_CR1_MSTR;                // master mode
	SPI2 -> CR2 |= SPI_CR2_SSOE | SPI_CR2_NSSP; // output enable and nssp enable
	SPI2 -> CR1 |= SPI_CR1_SPE;                 // enable channel
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
    DMA1_Channel5->CNDTR = 2;               // cndtr set to 8
    DMA1_Channel5->CCR |= DMA_CCR_DIR;      // read from mem
    DMA1_Channel5->CCR |= DMA_CCR_MSIZE_0;  // 16 bit m size
    DMA1_Channel5->CCR |= DMA_CCR_PSIZE_0;  // 16 bit p size
    DMA1_Channel5->CCR |= DMA_CCR_MINC;     // increment CMAR
}

//**********************************************
// function to setup DMA Channel 4 (used for RX data over SPI2)
//
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

void send_Now(void)
{
	// turn on transmission feature
	SPI2 -> CR2 |= SPI_CR2_TXDMAEN;
//	DMA1_Channel5->CCR |= DMA_CCR_EN;
	SPI2 -> DR = 0x00FF;
//	while(~(DMA1->ISR & DMA_ISR_TCIF5)) {};
}

void read_Now(void)
{
	while((GPIOB->IDR & (1<<8)) == 0) {};
	SPI2 -> CR2 |= SPI_CR2_RXDMAEN;
	DMA1_Channel4->CCR |= DMA_CCR_EN;
	toggle_IMU_NSS(1);
	while(~(DMA1->ISR & DMA_ISR_TCIF4)) {};
}

int main(void)
{
	setup_GPIO();
	setup_SPI();
//	setup_DMA_rx();
//	read_Now();
//	setup_DMA_tx();
//	toggle_IMU_NSS(1);
//	send_Now();
//	toggle_IMU_NSS(0);

	for(int i =0;i <1;i++)
	{
		toggle_IMU_NSS(1);
		SPI2 -> DR = 0x7E00;
	}
	for(int i = 0; i < 300;i++){};
	toggle_IMU_NSS(0);

}

