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
  int8_t rotation;
} Piece_t;

Piece_t piece_init (void);

void tetris (pixel_t * screen);

void convert_shape_format (coord_t * positions, Piece_t piece);

bool is_valid_space (uint64_t * locked_positions, Piece_t piece);

void lock_pos (uint64_t * lockes_position, Piece_t piece);
#endif
