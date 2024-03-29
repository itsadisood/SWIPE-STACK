/*
 * main.c
 *
 * Vishnu Lagudu
 * vlagudu@purdue.edu
 *
 * Drive the LED matrix
*/

#include "tetris.h"
#include <sys/queue.h>

pixel_t screen [MEMSIZE];

int
main ()
{
  // set the random seed
  set_random_seed ();

  tetris (screen);

  return EXIT_SUCCESS;
}
