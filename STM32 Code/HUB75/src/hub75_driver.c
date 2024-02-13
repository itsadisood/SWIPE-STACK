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
//  nano_wait (500);
  GPIOA -> ODR |= 0x1 << 11;
//  nano_wait (500);
  GPIOA -> ODR &= ~(0x1 << 11);
}

void
latch (void)
{
//  nano_wait (500);
  GPIOA -> ODR |= 0x1 << 12;
//  nano_wait (500);
  GPIOA -> ODR &= ~(0x1 << 12);
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
	  // int row = i & 0x3f;
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
draw_font (int row, int col, int color, Font font)
{
  int offset;

  // set the blank pin high
  int i = 0;

  // Bit bang the font
  for (;;)
  {
	int i_row = i % font.h;
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
    GPIOA -> ODR |= (row << 6);

    for (int j = 0; j < font.w; j++)
    {
      // set the color
      if ((font.pmap[i_row] >> j) & 0x1)
      {
        GPIOA -> ODR |= color << offset;
      }
      else
      {
        GPIOA -> ODR &= ~(WHITE << offset);
      }

      // clock the shift register
      clock ();
    }

    latch ();
    GPIOB -> ODR |= 0x1;
    GPIOB -> ODR &= ~0x1;
    row = (row+1)%(font.h);
  }
  // set the blank pin low
//  GPIOB -> ODR &= ~0x1;
}

//void draw_font(int row, int col, int color, Font font) {
//    int offset;
//
//    // Bit bang the font
//    for (;;) {
//        int i_row = row % font.h;
//
//        if (row > 0x1f) {
//            offset = 3;
//            row = row & 0x1f;
//        } else {
//            offset = 0;
//        }
//
//        // Set the row address
//        GPIOA->ODR &= ~(0x1f << 6);
//        GPIOA->ODR |= (row << 6);
//
//        for (int j = 0; j < font.w; j++) {
//            // Set the color
//            if ((font.pmap[i_row] >> j) & 0x1) {
//                GPIOA->ODR |= color << offset;
//            } else {
//                GPIOA->ODR &= ~(WHITE << offset);
//            }
//
//            // Clock the shift register
//            clock();
//        }
//
//        latch();
//        GPIOB->ODR |= 0x1;
//        // nano_wait (480000000);
//        GPIOB->ODR &= ~0x1;
//        // Increment the row address
//        row = (row + 1) % (font.h);
//    }
//}

