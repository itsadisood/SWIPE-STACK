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
  nano_wait (5000000);
  GPIOA -> ODR |= 0x1 << 11;
  nano_wait (5000000);
  GPIOA -> ODR &= ~(0x1 << 11);
}

void
latch (void)
{
  nano_wait (5000000);
  GPIOA -> ODR |= 0x1 << 12;
  nano_wait (5000000);
  GPIOA -> ODR &= ~(0x1 << 12);
}

void
clear_disp (void)
{
  int offset;

  // set the black pin high
  GPIOB -> ODR |= 0x1;

  for (int i = 0; i < HUB75_H; i++)
  {
    if (i > 0x1f)
    {
      offset = 3;
      row    = row & 0x1f;
    }
    else
    {
      offset = 0;
    }
    
    // set the row address
    GPIOA -> ODR &= ~(0x1f << 6);
    GPIOA -> ODR |= (rpw << 6);

    for (int j = 0; j < HUB75_W; j++)
    {
      // clear the screen
      GPIOA -> ODR &= ~(BLACK << offset);

      clock ();
    }

    // latch the row
    latch ();
  }

 // set the black pin low
 GPIOB -> ODR &= ~0x1; 
}

void 
draw_font (int row, int col, int color, Font* font)
{
  int offset;
  
  // set the blank pin high
  GPIOB -> ODR |= 0x1;

  // Bit bang the font
  for (int i = 0; i < font->h; i++)
  {
    if (row > 0x1f)
    {
      offset = 3;
      row    = row & 0x1f;
    }
    else
    {
      offset = 0;
    }

    // set the row address
    GPIOA -> ODR &= ~(0x1f << 6);
    GPIOA -> OSR |= (row << 6);

    for (int j = 0; i < font->w; j++)
    {
      // set the color
      if ((font->pmap >> j) & 0x1)
      {
        GPIOA -> ODR |= color << offset;
      }

      // clock the shift register
      clock ();
    }
    
    // move to the right colomn
    for (int k = 0; k < col; k++)
    {
      clock ();
    }

    // latch the row
    latch ();

    // increment the row address
    row++;
  }
  // set the blank pin low
  GPIOB -> ODR &= ~0x1;
}

