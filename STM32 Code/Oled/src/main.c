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
#include <stdio.h>

uint16_t score[34] = {
				0x002, // Command to set the cursor at the first position line 1
		        0x200+'S', 0x200+'C', 0x200+'O', 0x200+'R', 0x200+'E', + 0x200+':', 0x200+' ', 0x200+' ',
		        0x200+' ', 0x200+' ', 0x200+' ', 0x200+' ', + 0x200+' ', 0x200+' ', 0x200+' ', 0x200+' ',
		        0x0c0, // Command to set the cursor at the first position line 2
		        0x200+'0', 0x200+'0', 0x200+'0', 0x200+'0', 0x200+'0', + 0x200+'0', 0x200+'0', 0x200+'0',
		        0x200+'0', 0x200+'0', 0x200+'0', 0x200+'0', + 0x200+'0', 0x200+'0', 0x200+'0', 0x200+'0',
};

char scoreBuff[17] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void updt_score(int digitLen)
{
	int scoreIndex = 33;
	while(digitLen != 0)
	{
		score[scoreIndex] = 0x200 + scoreBuff[(digitLen - 1)];
		digitLen--;
		scoreIndex--;
	}
}

int main(void)
{
	uint64_t scoreNum = 0;
	setupGPIO_spi();
    init_spi1();
    spi1_init_display();
    spi1_setup_dma();
    scoreNum = 0;
    int len;

    for(;;)
    {
    	nano_wait(100000000);
    	scoreNum += 23;
    	sprintf(scoreBuff, "%llu", scoreNum);
    	len = strlen(scoreBuff);
    	updt_score(len);
    }
}




void setupGPIO_spi(){
	// PA5  SPI1_SCK
	// PA6  SPI1_MISO
	// PA7  SPI1_MOSI
	// PA15 SPI1_NSS

	// GPIOA config
	RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA -> MODER &= ~(GPIO_MODER_MODER5  | GPIO_MODER_MODER6 | GPIO_MODER_MODER7 | GPIO_MODER_MODER15);
	GPIOA -> MODER |= (GPIO_MODER_MODER5_1 | GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1 | GPIO_MODER_MODER15_1);
	// ports correspond to AF0 according to table
	GPIOA -> AFR[0] &= ~(0xf << (4 * 5));        // clr AFR5 -> using AF0
	GPIOA -> AFR[0] &= ~(0xf << (4 * 6));        // clr AFR6 -> using AF0
	GPIOA -> AFR[0] &= ~(0xf << (4 * 7));        // clr AFR7 -> using AF0
	GPIOA -> AFR[1] &= ~(0xf << (4 * (15 - 8))); // clr AFR15 -> using AF0
}

void init_spi1(){
	// PA5  SPI1_SCK
	// PA6  SPI1_MISO
	// PA7  SPI1_MOSI
	// PA15 SPI1_NSS

	// SPI1 config
	RCC -> APB2ENR |= RCC_APB2ENR_SPI1EN;
	SPI1 -> CR1 &= ~SPI_CR1_SPE; // disable SPI channel
	SPI1 -> CR1 |= SPI_CR1_BR; // BR = 111 -> fclk / 256
	SPI1 -> CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_3; // 10 bit word size
	SPI1 -> CR1 |= SPI_CR1_MSTR; // set spi channel to MASTER mode
	SPI1 -> CR2 |= SPI_CR2_SSOE; // set slave select output enable bit
	SPI1 -> CR2 |= SPI_CR2_NSSP; // automatically pulse NSS between data transfers
	SPI1 -> CR2 |= SPI_CR2_TXDMAEN; // when transmit buffer empty, trigger DMA
	SPI1 -> CR1 |= SPI_CR1_SPE; // enable SPI channel
}

void spi1_setup_dma(void)
{
    // Note: SPI1_TX works with DMA Channel 3
    // enable clock for DMA
    RCC -> AHBENR |= RCC_AHBENR_DMAEN;
    DMA1_Channel3 -> CCR &= ~DMA_CCR_EN; // turn off DMAch3
    // set CPAR to SPI1 -> DR address
    DMA1_Channel3 -> CPAR = (uint32_t) &(SPI1 -> DR);
    // set CMA to display array base address
    DMA1_Channel3 -> CMAR = (uint32_t) score;
    // set to transfer 34 'elements' total
    DMA1_Channel3 -> CNDTR = 34;
    // set DIR for mem -> peripheral
    DMA1_Channel3 -> CCR |= DMA_CCR_DIR; // read from mem
    // set MINC to increment CMAR upon each transfer
    DMA1_Channel3 -> CCR |= DMA_CCR_MINC;
    // set memory data size to 16-bit and peripheral size to 16 bit
    DMA1_Channel3 -> CCR &= ~DMA_CCR_MSIZE; // clr msize
    DMA1_Channel3 -> CCR &= ~DMA_CCR_PSIZE; // clr psize
    DMA1_Channel3 -> CCR |= 0x00000500; // set p and m to 16 bits
    // set to circular operation
    DMA1_Channel3 -> CCR |= DMA_CCR_CIRC;

    DMA1_Channel3 -> CCR |= DMA_CCR_EN; // turn on DMAch3
}

void spi_cmd(unsigned int data)
{
    while(!(SPI1->SR & SPI_SR_TXE)) {}
    SPI1->DR = data;
}

void spi1_init_display()
{
    nano_wait(1000000); // wait 1 ms
    spi_cmd(0x38); // set for 8-bit operation
    spi_cmd(0x08); // turn display off
    spi_cmd(0x01); // clear display
    nano_wait(2000000); // wait 2 ms
    spi_cmd(0x06); // set display to scroll
    spi_cmd(0x02); // move cursor to home position
    spi_cmd(0x0c); // turn display on
}

void spi1_display1(const char *string)
{
    spi_cmd(0x02); // move cursor to home position
    while(*string != '\0')
    {
        spi_cmd(*string | 0x200); // write char to DDRAM
        string++;
    }
}

void spi1_display2(const char *string)
{
	spi_cmd(0xc0); // move cursor to lower row (offset 0x40)
    while(*string != '\0')
    {
        spi_cmd(*string | 0x200); // write char to DDRAM
        string++;
    }
}

