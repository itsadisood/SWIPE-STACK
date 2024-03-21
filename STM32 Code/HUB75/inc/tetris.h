#ifndef __TETRIS__
#define __TETRIS__

#include "game_driver.h"
#include <math.h>
// #include "stab.h"

typedef struct
{
  uint8_t x_coord;
  uint8_t y_coord;
  shape_t shape;
  hub75_color_t color;
  uint8_t rotation;
} Piece_t;

Piece_t piece_init (void);

#endif
