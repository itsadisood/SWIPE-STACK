#include "tetris.h"

coord_t positions [SHAPE_NUM_PIX];
uint64_t locked_positions[NUM_ROWS_BOARD + 8] = {0, 0, 0, 0, 0x00000fffffffffff, 0x00000fffffffffff, 0x0000000000000003, 0x0000000000000003,
											 	 0x0000000000000003, 0x0000000000000003, 0x0000000000000003, 0x0000000000000003,
												 0x0000000000000003, 0x0000000000000003, 0x0000000000000003, 0x0000000000000003,
												 0x0000000000000003, 0x0000000000000003, 0x0000000000000003, 0x0000000000000003,
												 0x0000000000000003, 0x0000000000000003, 0x0000000000000003, 0x0000000000000003,
												 0x0000000000000003, 0x0000000000000003, 0x00000fffffffffff, 0x00000fffffffffff, 0, 0, 0, 0}; // 24 x 44 (actually 24 x 64, but ignore upper 20 bits)

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
  new_piece.x_coord = 12; // start piece in middle-top of grid
  new_piece.y_coord = 46;
  new_piece.shape = get_shape (rand_idx);
  new_piece.color = get_shape_color (rand_idx);
  new_piece.rotation = 0;
  return new_piece;
}

void
convert_shape_format (coord_t * positions, Piece_t piece)
{
  uint8_t pos = 0;
  map_t format = piece.shape.pmap[piece.rotation];
  for (int i = 0; i < format.height; i++)
  {
    uint64_t doubleword = format.pmap[i];
    for (int j = 0; j < format.width; j++)
    {
      if (doubleword & 1)
      {
        positions[pos].x = piece.x_coord + i;
        positions[pos].y = piece.y_coord + j;
        pos++;
      }
      doubleword >>= 1; 
    }
  }
  // experimental
//  for (int i= 0; i < SHAPE_NUM_PIX; i++)
//  {
//    positions[i].x = positions[i].x - 2;
//    positions[i].y = positions[i].y - 4;
//  }
}

//*************************************************************************************************
// is_valid_space checks the piece's position against the locked_positions array which is an array
// of 64-bit unsigned integers, where the array index represents the row, while the bits of the
// integer elements represent whether a column is occupied or not
//*************************************************************************************************
bool
is_valid_space(uint64_t * locked_positions, Piece_t piece)
{
	convert_shape_format (positions, piece);

	for(int i = 0; i < SHAPE_NUM_PIX; i++)
	{
		if((locked_positions[positions[i].x] >> positions[i].y) & 1)
		{
			// block is living in invalid space
			return false;
		}
	}
	return true;
}

//*************************************************************************************************
// lock_pos will append the position of a stoppied piece into the locked_positions array
// The position will be locked until a row is cleared or the game ends
//*************************************************************************************************
void
lock_pos(uint64_t * locked_positions, Piece_t piece)
{
	convert_shape_format (positions, piece);
	uint64_t shifter = 1;

	for(int i = 0; i < SHAPE_NUM_PIX; i++)
	{
		locked_positions[positions[i].x] |= shifter << positions[i].y;
	}
}

void
tetris (pixel_t * screen)
{
  bool new_piece = true;
  game_init (screen);
  Piece_t piece;

  while (1)
  {
    /* code */

	  if(new_piece)
	  {

		  piece = piece_init();
		  sr_font (screen, piece.x_coord, piece.y_coord, piece.shape.pmap[piece.rotation], piece.color, 1);
		  new_piece = false;
	  }
	  //nano_wait(50000000);
	  piece.y_coord--;
	  if(is_valid_space(locked_positions, piece))
	  {
		  convert_shape_format(positions, piece);
		  sr_font (screen, piece.x_coord, piece.y_coord + 1, piece.shape.pmap[piece.rotation], piece.color, 0); // undo prev state
		  sr_font (screen, piece.x_coord, piece.y_coord, piece.shape.pmap[piece.rotation], piece.color, 1); // form new state


	  }
	  else
	  {
		  piece.y_coord++;
		  new_piece = true;

		  lock_pos(locked_positions, piece);
	  }
  }
  
}
