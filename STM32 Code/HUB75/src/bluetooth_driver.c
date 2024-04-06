/**
 * @brief Includes coinfiguration of peripherals and drivers
 *        that are mainly written for the bluebooth.
 * @author Vishnu Lagudu (vlagudu@purdue.edu)
 *         Alex Chitsazzadeh (achitsaz@purdue.edu)
 *         Aditya Sood (sood12@purdue.edu)
**/

char MOVE;

#include "bluetooth_driver.h"

/* Coinfigure the peripherals required for UART transmission */

void
init_io_bt ()
{
  RCC -> AHBENR |= RCC_AHBENR_GPIOCEN;                
	GPIOC -> MODER |= GPIO_MODER_MODER0_1; 	
  GPIOC -> MODER |= GPIO_MODER_MODER1_1; 				
	GPIOC -> AFR[0] &= ~(0xF);								 	
	GPIOC -> AFR[0] |= (~0xF << 4); 	  
}

void 
setup_uart_bt ()
{
  RCC -> APB1ENR |= RCC_APB1ENR_USART4EN;   
  // Set the control register reset state             
	USART4 -> CR1 = 0;                                            
	USART4 -> BRR = 48000000 / 9600;					  
	USART4 -> CR1 |= USART_CR1_TE;				           
	USART4 -> CR1 |= USART_CR1_RE;				           
	USART4 -> CR1 |= USART_CR1_UE;   
	USART4 -> CR1 |= USART_CR1_RXNEIE;
	USART4 -> CR3 |= USART_CR3_DMAR;               
  // wait for TX and RX to be ready
	while(!((USART4 -> ISR) & (USART_ISR_TEACK | USART_ISR_REACK)));
}

// need to fix this
void
init_dma_bt ()
{
  DMA1_Channel6 -> CCR &= ~DMA_CCR_EN;
	DMA1_Channel6 -> CMAR = &MOVE;
	DMA1 -> RMPCR |= DMA_RMPCR2_CH2_USART4_RX;
	DMA1_Channel6 -> CPAR = (uint32_t) &(USART4->RDR);
	DMA1_Channel6 -> CNDTR = sizeof (char);
	DMA1_Channel6 -> CCR |= DMA_CCR_PL;
}

void
enable_dma_bt ()
{
	DMA1_Channel6 -> CCR |= DMA_CCR_EN;
}