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
  ss_bluetooth    ();

  // REMEMBER: DO NOT ZERO OUT PA AFR IN OLED (alr done in bt)

  tetris (screen);

  return EXIT_SUCCESS;
}
