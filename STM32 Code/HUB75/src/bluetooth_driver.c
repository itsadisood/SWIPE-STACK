/**
 * @brief Includes coinfiguration of peripherals and drivers
 *        that are mainly written for the bluebooth.
 * @author Vishnu Lagudu (vlagudu@purdue.edu)
 *         Alex Chitsazzadeh (achitsaz@purdue.edu)
 *         Aditya Sood (sood12@purdue.edu)
**/

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
  RCC -> APB2ENR |= RCC_APB2ENR_USART7EN;   
  // Set the control register reset state             
	USART7 -> CR1 = 0;                                            
	USART7 -> BRR = 48000000 / 9600;					  
	USART7 -> CR1 |= USART_CR1_TE;				           
	USART7 -> CR1 |= USART_CR1_RE;				           
	USART7 -> CR1 |= USART_CR1_UE;                         
  // wait for TX and RX to be ready
	while(!((USART5 -> ISR) & (USART_ISR_TEACK | USART_ISR_REACK)));
}

// need to fix this
void
init_dma_bt ()
{
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	DMA1_Channel3->CCR &= ~DMA_CCR_EN;      			
	DMA1_Channel3->CMAR = (uint32_t) data_fifo;    	 	
	DMA1_Channel3->CPAR = (uint32_t) &(USART1->RDR);
	DMA1_Channel3->CNDTR = FIFOSIZE;        			
	DMA1_Channel3->CCR &= ~DMA_CCR_DIR;     			
	DMA1_Channel3->CCR &= ~DMA_CCR_MSIZE;   			
	DMA1_Channel3->CCR &= ~DMA_CCR_PSIZE;   			
	DMA1_Channel3->CCR |= DMA_CCR_MINC;     			
	DMA1_Channel3->CCR |= DMA_CCR_CIRC;					
	DMA1_Channel3->CCR |= DMA_CCR_PL_0 | DMA_CCR_PL_1;  
	DMA1_Channel3->CCR |= DMA_CCR_TCIE; 				
	DMA1_Channel3->CCR |= DMA_CCR_EN;	
	NVIC -> ISER[0] |= (1 << DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
}