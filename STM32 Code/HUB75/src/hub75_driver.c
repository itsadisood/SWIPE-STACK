/*
 * hub75_driver.c
 *
 * Vishnu Lagudu
 * vlagudu@purdue.edu
 *
 * Driver for HUB75 matrix
 */

#include "hub75_driver.h"

// asm function to waste
// CPU clocks
void
nano_wait(unsigned int n)
{
  asm("        mov r0,%0\n"
      "repeat: sub r0,#83\n"
      "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

// initialize the gpio ports 
// for HUB75 matrix
void
init_io (void)
{
	// Start the RCC clock for ports A and B
  RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
  RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;
  
  // Coinfigure pins to outputs
  GPIOA -> MODER &= 0xfC000000;
  GPIOA -> MODER |= 0x01555555;
  GPIOB -> MODER &= ~0x3;
  GPIOB -> MODER |= 0x1;
}

void
clock (void)
{
  GPIOA -> BSRR = 0x1 << 11;
  GPIOA -> BRR  = (0x1 << 11);
}

void
latch (void)
{
//  nano_wait (500);
  GPIOA -> BSRR = 0x1 << 12;
//  nano_wait (500);
  GPIOA -> BRR  = (0x1 << 12);
}

/*
 * Function to fill display with a solid color
 */
void
fill_disp (uint8_t color)
{
  int offset, row, i;

  i = 0;
  for (;;)
  {
	int row = i & 0x3f;
    offset = 0;
    if (row > 0x1f)
    {
      offset = 3;
      row    = row & 0x1f;
    }
    
    // set the row address
    GPIOA -> ODR &= ~(0x1f << 6);
    GPIOA -> ODR |= (row << 6);

    for (int j = 0; j < HUB75_W; j++)
    {
      GPIOA -> ODR |= color << offset;
      clock ();
    }

    latch ();

    GPIOB -> ODR |= 0x1;
    GPIOB -> ODR &= ~0x1;

    i++;
  }
}

void
writebyte (uint8_t byte, uint8_t color)
{
  for (int i = 0; i < 64; i++)
  {
	  // set the default to blank
	  GPIOA -> BRR = (0x3f);
	  if (i < 8)
	  {
		  GPIOA -> BRR = (0x3f);
		  if (byte & 0x80)
		  {
			  // if pixel present set to a color
			  GPIOA -> BSRR = color;
		  }
		  // change the pixel at msb
		  byte<<=1;
	  }
	  // clock in pixel by pixel
	  clock ();
  }
}

void
showchar (uint8_t row, uint8_t color, uint8_t* char_map)
{
	uint8_t offset = 0;
	for (int i = 0; i < 8; i++)
	{
		// wrap the row
		if (row > 15)
		{
			row    = row & 0xf;
			offset = 3;
		}
		// Enable blank
//		GPIOB -> BSRR = 0x1;
		// set the row index
		GPIOA -> BRR  = (0x1f << 6);
		GPIOA -> BSRR = (row << 6);
		// write byte
		writebyte (char_map[i], color << offset);
		// latch the row
		GPIOB -> BSRR = 0x1;
		latch ();
		// disable blank
		GPIOB -> BRR = 0x1;
		// wait to increase duty cycle
		nano_wait(50000 - 34000);
//		nano_wait(5000000 -3400000);
		// decrement the row index
		row--;
	}
}

void setup_dma (void* addr)
{
	RCC->AHBENR |= RCC_AHBENR_DMAEN;
	// Total size of trnasfer
	DMA1_Channel1->CNDTR = 64 * 16;
	// memory address
	DMA1_Channel1->CMAR = (uint32_t) addr;
	// Peripheral Destination
	DMA1_Channel1->CPAR = (uint32_t) (&(GPIOB->ODR));
	//set memory access size to 32 bits
	DMA1_Channel1->CCR |= DMA_CCR_MSIZE_1;

  //set peripheral access size to 16 bits
  DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;

  //set minc to 1 and pinc to 0
  DMA1_Channel1->CCR &= ~DMA_CCR_PINC;
  DMA1_Channel1->CCR |= DMA_CCR_MINC;

  //set to circular
  DMA1_Channel1->CCR |= DMA_CCR_CIRC;
  //set to mem to peripheral direction
  DMA1_Channel1->CCR |= DMA_CCR_DIR;

  //TIM2 is the default mapping for DMA1 Channel1
  //DMA1->RMPCR |= DMA_RMPCR1_CH3_TIM2;


  //enable the channel
  DMA1_Channel1->CCR |= DMA_CCR_EN;



}

