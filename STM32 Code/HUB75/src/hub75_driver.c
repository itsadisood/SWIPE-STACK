/*
 * hub75_driver.c
 *
 * Vishnu Lagudu
 * vlagudu@purdue.edu
 *
 * Driver for HUB75 matrix using the DMA
 * and timers 
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
/*
	Communication with the DMA through the OLED
*/

/*
	Initalize the IO
	[0,6],[11,15] as output
	10 as alternate function for TIM2 channel 3
*/
void
init_io (void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	// clear for the pins
	GPIOB->MODER &= 0x000fc000;
	// set the modes
	GPIOB->MODER |= 0x55601555;
	// set the alternate function
	GPIOB->AFR[1] &= ~(0xf << (4 * 2));
	GPIOB->AFR[1] |= 0x2 << (4 * 2);
}

/* 
	DMA to send data from memory to the ODR at the 
	@negedge of the clk.
*/
void 
setup_dma (void * addr)
{
	RCC->AHBENR |= RCC_AHBENR_DMAEN;
	// set CCR to reset value;
	DMA1_Channel1->CCR = 0;
	// Total size of transfer 64 cols * 32 rows
	DMA1_Channel1->CNDTR = MEMSIZE;
	// Memory Location
	DMA1_Channel1->CMAR = (uint32_t) addr;
	// Peripheral Destination
	DMA1_Channel1->CPAR = (uint32_t) (&(GPIOB->ODR));
	// mem access : 32 bits
	// periph access : 16 bits
	DMA1_Channel1->CCR |= (DMA_CCR_MSIZE_1 | DMA_CCR_PSIZE_0);
	// Allow memory increment
	DMA1_Channel1->CCR |= DMA_CCR_MINC;
	// Set up for circular transfers
	DMA1_Channel1->CCR |= DMA_CCR_CIRC;
	// Direction: MEMORY -> PERIPHERAL
	DMA1_Channel1->CCR |= DMA_CCR_DIR;
	// Set the Priority Level to Very High
	DMA1_Channel1->CCR |= DMA_CCR_PL;
}

void
ed_dma (bool DMA_FLG)
{
	if (DMA_FLG)
	{
		DMA1_Channel1->CCR |= DMA_CCR_EN;
	}
	else
	{
		DMA1_Channel1->CCR &= ~DMA_CCR_EN;
	}
}

/* 
	Upcounting Timer 2 to produce a PWM wave at channel 3. 
	DMA is triggered by the timer.
*/
void 
setup_tim2 (uint32_t psc, uint32_t arr, uint32_t ccr)
{
	// Turn on the clock for timer 2
	RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;
	// set the clk freq
	TIM2->PSC = psc-1;
	TIM2->ARR = arr-1;
	// Set for Upcounting
	TIM2->CR1 &= ~TIM_CR1_DIR;
	// Set the Channel 3 for PWM mode 2
	TIM2->CCMR2 |= (TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2| TIM_CCMR2_OC3M_0);
	// set the CCR value to produce PWM of desired duty cycle
	TIM2->CCR3 = ccr;
	// Enable DMA transfer at the @posedge of the clk
	TIM2->DIER |= TIM_DIER_CC3DE;
	TIM2->CCER |= TIM_CCER_CC3E;
	// Turn on timer
	TIM2->CR1 |= 0x1;
}

/*
	Initialize screen to the desired color
*/

void
init_screen (pixel_t * screen, hub75_color_t color)
{
	// volatile int size = sizeof(Pixel);
	for (uint8_t x = 0; x < HUB75_R; x++)
	{
		for (uint8_t y = 0; y < HUB75_C; y++)
		{
			if (y == (HUB75_C - 1))
			{
				screen[x * HUB75_C + y].latch = 1;
			}
			else
			{
				screen[x * HUB75_C + y].latch = 0;
			}
			screen[x * HUB75_C + y].color = (color << 3) + color;
			screen[x * HUB75_C + y].raddr = (x-1) & 0xf;
		}
	}
}

void
sr_font (pixel_t * screen, uint8_t row, uint8_t col, const map_t typeface, hub75_color_t color, bool set)
{
	// check if the values are within the range
	if ((row + typeface.height) > HUB75_H) return;
	if ((col + typeface.width) > HUB75_C) return;

	uint8_t offset = 0, i = 0;
	for (uint8_t x = row; x < row+typeface.height; x++)
	{
		uint64_t doubleword = typeface.pmap[i];
		if (x > 0xf)
		{
			offset = 3;
		}
		for (uint8_t y = col; y < col+typeface.width; y++)
		{
			if (doubleword & 1)
			{
				screen[(x & 0xf) * HUB75_C + y].color &= ~(WHITE << offset);
				if (set) screen[(x & 0xf) * HUB75_C + y].color |= color << offset;
			}
			doubleword >>= 1;
		}
		i++;
	}
}

void
sr_coord (pixel_t * screen, coord_t * positions, hub75_color_t color, bool set)
{
	uint8_t offset;
	for (int i = 0; i < SHAPE_NUM_PIX; i++)
	{
		if (positions[i].x > 0xf)
		{
			offset = 3;
		}
		else
		{
			offset = 0;
		}
		screen[positions[i].x * HUB75_C + positions[i].y].color &= ~(WHITE << offset);
		if (set) screen[positions[i].x * HUB75_C + positions[i].y].color |= color << offset;
	}
}
