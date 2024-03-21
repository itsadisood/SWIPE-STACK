#include "tetris.h"

static int 
gen_rand_int (int lower, int upper)
{
  // Seed the random number generator using the current time
  uint64_t curr_seed = rand ();
  srand(curr_seed);
  // uint64_t random_number = (rand() % (upper - lower + 1)) + lower;
  return ((rand() % (upper - lower + 1)) + lower);
}

Piece_t
piece_init (void)
{
  // get the random value
  int rand_idx = gen_rand_int (0, 6);

  // initialize the piece values
  Piece_t new_piece;
  new_piece.x_coord = 3;
  new_piece.y_coord = 19;
  new_piece.shape = get_shape (rand_idx);
  new_piece.color = get_shape_color (rand_idx);
  new_piece.rotation = 0;
  return new_piece; 
}
