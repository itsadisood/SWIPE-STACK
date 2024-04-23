/*
 * main.c
 *
 * Vishnu Lagudu
 * vlagudu@purdue.edu
 *
 * Drive the LED matrix
*/

#include "tetris.h"

pixel_t screen [MEMSIZE];


int
main ()
{
  // set the random seed
  set_random_seed ();
  ss_bluetooth    ();
  setup_oled      ();


//	  splash_screen (screen);
  init_hub(screen);
  while(1)
  {
	  splash_screen (screen);
	  int score = tetris (screen);
	  lose_screen (screen);

  }

  return EXIT_SUCCESS;
}
