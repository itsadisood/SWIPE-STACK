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
#include "BNO08x.h" // library for IMU functions
#include <stdint.h>

uint8_t tx_cargo[300];
uint8_t rx_cargo[300];

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
	toggle_RST(1);
	nano_wait(10000000); //10ms
	toggle_RST(0);
	toggle_NSS(0);
}


void setup_SPI(void)
{
	RCC -> APB1ENR |= RCC_APB1ENR_SPI2EN;       // enable spi2 clock
	SPI2 -> CR1 &= ~SPI_CR1_SPE;                // clear enable bit (to update config)
	SPI2 -> CR1 |= SPI_CR1_MSTR;                // master mode
	SPI2 -> CR1 |= (SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2); // lowest baud rate (48Mhz / 256)
	SPI2 -> CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;    // 8 bit size of data
	SPI2 -> CR2 |= SPI_CR2_SSOE | SPI_CR2_NSSP; // output enable and nssp enable
	SPI2 -> CR2 |= SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN;
	SPI2 -> CR1 |= SPI_CR1_SPE;
}

void init_exti(void)
{
	RCC -> APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
	SYSCFG -> EXTICR[2] |=  SYSCFG_EXTICR3_EXTI8_PB;
	EXTI -> IMR |= EXTI_IMR_MR8;
	EXTI -> FTSR |= EXTI_FTSR_TR8;
	NVIC -> ISER[0] |= (1 << EXTI4_15_IRQn);
}

void EXTI4_15_IRQHandler(void) {
    EXTI -> PR |= EXTI_PR_PR8;
    get_startup_adv(tx_cargo, rx_cargo);
}

int main(void)
{
	memset(tx_cargo, 0, 300);
	memset(rx_cargo, 0, 300);
	setup_GPIO();
	setup_SPI();
	init_exti();

	while(1)
	{
		asm("wfi");
	}
}

