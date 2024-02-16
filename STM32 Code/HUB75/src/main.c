/*
 * main.c
 *
 * Vishnu Lagudu
 * vlagudu@purdue.edu
 *
 * Drive the LED matrix
*/


#include "hub75_driver.h"


int
main ()
{
  // coinfigure GPIO ports
  init_io ();

  // clear the display
// fill_disp (WHITE);

  // drive the font
  uint8_t h [] = {0, 0x7f, 0x08, 0x08, 0x08, 0x7f, 0, 0};
  uint8_t t [] = {0, 0x60, 0x40, 0x7f, 0x40, 0x60, 0, 0};
  uint8_t e [] = {0, 0x7f, 0x49, 0x49, 0x49, 0x41, 0, 0};
  // draw_font (0, 0, WHITE, fonts[2]);
   GPIOA -> ODR &= ~(0x1f << 6);
   for (;;)
   {
	   showchar (31, TEAL, h);
	   showchar (23, WHITE, t);
	   showchar (15, YELLOW, e);
	   showchar (7, RED, t);
   }
}
