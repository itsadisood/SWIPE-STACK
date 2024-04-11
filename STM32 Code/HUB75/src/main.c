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

  // REMEMBER: DO NOT ZERO OUT PA AFR IN OLED (alr done in bt)
//  while(1)
//  {
//	  // splash screen. Exit if MOVE == X
//	  tetris(screen);
//	  // LOSE SCREEN. Exit if MOVE == X
//  }


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
