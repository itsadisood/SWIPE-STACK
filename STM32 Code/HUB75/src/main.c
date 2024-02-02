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
  clear_disp ();

  // drive the font
  draw_font (4, 5, WHITE, fonts[0]);
  
}
