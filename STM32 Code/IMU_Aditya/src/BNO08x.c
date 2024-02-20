/*
 * BNO08x.c
 *
 *  Created on: Feb 1, 2024
 *      Author: Aditya
 */

#include "BNO08x.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

extern void nano_wait(int);


//********************************
// GPIO Functions
//********************************
void toggle_SPI(uint8_t op)
{
	if(op)
		SPI2 -> CR1 |= SPI_CR1_SPE;
	else
		SPI2 -> CR1 &= ~SPI_CR1_SPE;
}

void toggle_RST(uint8_t op)
{
	if(op)
		GPIOB -> BSRR |= GPIO_BSRR_BR_10;
	else
		GPIOB -> BSRR |= GPIO_BSRR_BS_10;
}

void toggle_NSS(uint8_t op)
{
	if(op)
		GPIOB -> BSRR |= GPIO_BSRR_BR_12;
	else
		GPIOB -> BSRR |= GPIO_BSRR_BS_12;
}

void toggle_EXTI(uint8_t op)
{
	if(op)
		EXTI -> IMR |= EXTI_IMR_MR8;
	else
		EXTI -> IMR &= ~EXTI_IMR_MR8;
}

//*********************************
// DMA Functions
//*********************************
void toggle_DMA_tx(uint8_t op)
{
	if(op)
		DMA1_Channel5 -> CCR |= DMA_CCR_EN;
	else
		DMA1_Channel5 -> CCR &= ~DMA_CCR_EN;
}

void toggle_DMA_rx(uint8_t op)
{
	if(op)
		DMA1_Channel4 -> CCR |= DMA_CCR_EN;
	else
		DMA1_Channel4 -> CCR &= ~DMA_CCR_EN;
}

void setup_DMA_tx(uint8_t* cmar_addr, uint32_t transfer_size)
{
    RCC -> AHBENR |= RCC_AHBENR_DMA1EN;     // clock of dma1 on
    DMA1_Channel5->CCR &= ~DMA_CCR_EN;      // turn off enable bit
    DMA1_Channel5->CMAR = (uint32_t) cmar_addr;    // cmar address (read from here)
    DMA1_Channel5->CPAR = (uint32_t) &(SPI2->DR); // cpar address (read to here)
    DMA1_Channel5->CNDTR = transfer_size;
    DMA1_Channel5->CCR |= DMA_CCR_DIR;      // read from mem
    DMA1_Channel5->CCR &= ~DMA_CCR_MSIZE;   // 8 bit (1B) m size
    DMA1_Channel5->CCR &= ~DMA_CCR_PSIZE;   // 8 bit (1B) p size
    DMA1_Channel5->CCR |= DMA_CCR_MINC;     // increment CMAR
}

void setup_DMA_rx(uint8_t* cmar_addr, uint32_t reception_size)
{
	RCC -> AHBENR |= RCC_AHBENR_DMA1EN;	// clock of dma2 on
    DMA1_Channel4->CCR &= ~DMA_CCR_EN;      // turn off enable bit
	DMA1_Channel4->CMAR = (uint32_t) cmar_addr;    // cmar address (read to here)
	DMA1_Channel4->CPAR = (uint32_t) &(SPI2->DR); // cpar address (read from here)
	DMA1_Channel4->CNDTR = reception_size;
	DMA1_Channel4->CCR &= ~DMA_CCR_DIR;     // read from perip
	DMA1_Channel4->CCR &= ~DMA_CCR_MSIZE;   // 8 bit (1B) m size
	DMA1_Channel4->CCR &= ~DMA_CCR_PSIZE;   // 8 bit (1B) p size
	DMA1_Channel4->CCR |= DMA_CCR_MINC;     // increment CMAR
}

// *****************************
// SHTP Functions
// *****************************
uint8_t* set_header(uint8_t channel_num, uint8_t data_len)
{
	uint16_t packet_length = data_len + 4;
	shtp_header[0] = (packet_length & 0xFF); // LSB
	shtp_header[1] = (packet_length >> 0x8); // MSB
	shtp_header[2] = (channel_num);		// channel number
	shtp_header[3] = (channel_seq[channel_num]++); // increment sequence number
	return shtp_header;
}

void send_packet(uint8_t* tx_buffer, uint8_t* shtp)
{
	// TX buffer
	tx_buffer[0] = shtp[0];
	tx_buffer[1] = shtp[1];
	tx_buffer[2] = shtp[2];
	tx_buffer[3] = shtp[3];

	DMA1_Channel5 -> CNDTR = sizeof(tx_buffer);
	SPI2 -> CR1 |= SPI_CR1_SPE;
	DMA1_Channel5->CCR |= DMA_CCR_EN;
	while((DMA1->ISR & DMA_ISR_TCIF5) == 0) {}
}

void get_startup_adv(uint8_t* tx_buffer, uint8_t* rx_buffer)
{
	toggle_EXTI(0);
	setup_DMA_tx(tx_buffer, 300);
	setup_DMA_rx(rx_buffer, 300);
	toggle_DMA_tx(1);
	toggle_DMA_rx(1);
	toggle_NSS(1);
	nano_wait(30000000); //30ms (enough time for SPI to complete and DMA to copy)
	toggle_DMA_rx(0);
	toggle_DMA_tx(0);
	toggle_NSS(0);
	toggle_SPI(0);
}


