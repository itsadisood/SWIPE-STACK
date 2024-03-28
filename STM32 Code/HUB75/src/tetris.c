#include "tetris.h"

# define TEST 0x3fffffffffc

coord_t positions [SHAPE_NUM_PIX];
coord_t p_positions [SHAPE_NUM_PIX];

// fix locked positions
// 24 x 44 (actually 24 x 64, but ignore upper 20 bits)
uint64_t locked_positions[NUM_ROWS_BOARD + 8] = 
{
	0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
	0x00000fffffffffff, 0x00000fffffffffff, 0x0000000000000003, 0x0000000000000003,
	0x0000000000000003, 0x0000000000000003, 0x0000000000000003, 0x0000000000000003,
	0x0000000000000003, 0x0000000000000003, 0x0000000000000003, 0x0000000000000003,
	0x0000000000000003, 0x0000000000000003, 0x0000000000000003, 0x0000000000000003,
	0x0000000000000003, 0x0000000000000003, 0x0000000000000003, 0x0000000000000003,
	0x0000000000000003, 0x0000000000000003, 0x00000fffffffffff, 0x00000fffffffffff,
	0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
};

//  uint64_t borders[NUM_ROWS_BOARD + 8]          = {0, 0, 0, 0, 0x00000fffffffffff, 0x00000fffffffffff, 0x00000c0000000003, 0x00000c0000000003,
// 		 	 	 	 	 	 	 	 	 	 	 0x00000c0000000003, 0x00000c0000000003, 0x00000c0000000003, 0x00000c0000000003,
// 												 0x00000c0000000003, 0x00000c0000000003, 0x00000c0000000003, 0x00000c0000000003,
// 												 0x00000c0000000003, 0x00000c0000000003, 0x00000c0000000003, 0x00000c0000000003,
// 												 0x00000c0000000003, 0x00000c0000000003, 0x00000c0000000003, 0x00000c0000000003,
// 												 0x00000c0000000003, 0x00000c0000000003, 0x00000c0000000003, 0x00000c0000000003, 0, 0, 0, 0};

/* 
suggestion
- locked positions : array to hold positions of locked blocks
- borders          : array to store game borders
*/

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
  new_piece.x_coord = 19; // start piece in middle-top of grid
  new_piece.y_coord = 47;
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
      if ((doubleword & 1))
      {
				if ((piece.y_coord - i) <= 41)
				{
					positions[pos].x = piece.x_coord - j;
					positions[pos].y = piece.y_coord - i;
				}
				else
				{
					positions[pos].x = 255;
					positions[pos].y = 255;
				}
        pos++;
      }
      doubleword >>= 1; 
    }
  }
}

void
move_shape (pixel_t * screen, Piece_t piece)
{
  sr_coord (screen, p_positions, piece.color, 0); // undo prev state
	sr_coord (screen, positions, piece.color, 1); // form new state
}

//*************************************************************************************************
// is_valid_space checks the piece's position against the locked_positions array which is an array
// of 64-bit unsigned integers, where the array index represents the row, while the bits of the
// integer elements represent whether a column is occupied or not
//*************************************************************************************************
bool
is_valid_space (uint64_t * locked_positions, Piece_t piece)
{
	// convert_shape_format (positions, piece);

	for(int i = 0; i < SHAPE_NUM_PIX; i++)
	{
		if((positions[i].x != 255) && (positions[i].y != 255))
		{
			if((locked_positions[positions[i].x] >> positions[i].y) & 1)
			{
				// block is intersecting with another block or the bottom border
				return false;
			}
		}
	}
	return true;
}

// iterate through column 6 to column 25 of some row, but quit when there is not a filled pixel
bool
check_row (uint8_t s, uint64_t mask)
{
	uint64_t prev, curr;
	for(int i = 0; i < NUM_ROWS_BOARD - 5; i++) // i is [0,18], so s + i is [6,24] and s + i + 1 is [7,25]
	{
		prev = locked_positions [s + i] & mask;
		curr = locked_positions [s + i + 1] & mask;

    if (!prev) return false;
    if (!curr) return false;

		if ((prev != curr))
		{
			return false;
		}
	}
	return true;
}

uint32_t
check_clear ()
{
	uint32_t cols = 0;
	uint64_t mask = 0xc;

	for (int i = 0; i < (NUM_COLS_BOARD - 4) / 2; i++)
	{
		if (check_row (6, mask))
		{
			cols |= 1 << i;
		}
		mask <<= 2;
	}

	return cols;

	// uint8_t * cols = malloc (size * sizeof(uint8_t));
	// for (int i = 0; i < NUM_COLS_BOARD; i++)
	// {
	// 	if (check_row (6, mask))
	// 	{
			
	// 		// cols |= 1 << i;
	// 	}
  // }
}

//*************************************************************************************************
// lock_pos will append the position of a stoppied piece into the locked_positions array
// The position will be locked until a row is cleared or the game ends
//*************************************************************************************************
void
lock_pos(uint64_t * locked_positions, Piece_t piece)
{
	// convert_shape_format (positions, piece);
	uint64_t shifter = 1;

	for(int i = 0; i < SHAPE_NUM_PIX; i++)
	{
		locked_positions[positions[i].x] |= shifter << positions[i].y;
	}
}

void 
update_lock_pos (int col)
{
	col *= 2;
  uint64_t upper, lower;
  for(int s = 6; s < 26; s++)
  {
    // // clear row
    // locked_positions[s] &= ~(0xc << col);

    // get tthe lower loc_pos
    lower = locked_positions[s] & ((TEST >> (40 - col)));

    // get the upper loc_pos
    upper = locked_positions[s] & (TEST << (col + 2));
    upper >>= 2; 

    locked_positions[s] = upper | lower; 
  }
}

bool
lose_cond ()
{
  // check [14], [15], [16], [17]
  for(int i = 14; i < 18; i++)
  {
    if(((locked_positions[i] & (0x20000000000)) >> 41))
	  {
    	return true;
	  }
  }
  return false;

}

void
tetris (pixel_t * screen)
{
  // initialize some game driver parameters
  fall_time = 0;
  KEY_LEFT  = false;
  KEY_RIGHT = false;
  KEY_ROT   = false;

  bool new_piece = true;
  game_init (screen);
  setup_tim3(1000, 10);
  Piece_t piece;
  int rowsCleared = 0;

  while (true)
  {
	  if(lose_cond())
	  {
	    return;
	  }
	  if (new_piece)
	  {
			// fetch new piece
		  piece = piece_init();

			// blit onto screen
			convert_shape_format (positions, piece);
		  sr_coord (screen, positions, piece.color, 1);

		  new_piece = false;
	  }
		// fall dowm
	  else if (fall_time >= 300)
	  {
			// new position
		  piece.y_coord -= 2;
			convert_shape_format(positions, piece);

		  if(is_valid_space (locked_positions, piece))
		  {
				// move shape to new position
				move_shape (screen, piece);
		  }
		  else
		  {
				// restore shape
			  piece.y_coord += 2;
				memcpy (positions, p_positions, sizeof (p_positions));

				// set for new piece fetch
				new_piece = true;
				lock_pos(locked_positions, piece);
				// check if any rows can be cleared
				uint32_t cols = check_clear ();
				if(cols != 0)
				{
          for(int i = 0; i < (NUM_COLS_BOARD - 4) / 2; i++)
          {
            if((cols >> i) & 1)
            {
              clear_row(screen, i - rowsCleared);
              update_lock_pos (i - rowsCleared); // i is col
              drop_rows (screen, i + 1 - rowsCleared);
              rowsCleared++;
            }
          }
				}

		  }

		  fall_time = 0;
		  rowsCleared = 0;
	  }
		// move left
	  else if (KEY_LEFT)
	  {
			// new position
		  piece.x_coord -= 2;
			convert_shape_format(positions, piece);

		  if(is_valid_space(locked_positions, piece))
		  {
			  // move shape to new position
				move_shape (screen, piece);
		  }
		  else
		  {
				// restore shape
        piece.x_coord += 2;
				memcpy (positions, p_positions, sizeof (p_positions));
		  }

		  KEY_LEFT = false;
	  }
		// move right
		else if (KEY_RIGHT)
	  {
			// new position
			if(piece.y_coord <= 49) // INCLUDE CROPPED SHAPE HEIGHT 
			{
				piece.x_coord += 2;
			}
			convert_shape_format(positions, piece);

		  if(is_valid_space(locked_positions, piece))
		  {
			  // move shape to new position
				move_shape (screen, piece);
		  }
		  else
		  {
				// restore shape
        piece.x_coord -= 2;
				memcpy (positions, p_positions, sizeof (p_positions));
		  }

		  KEY_RIGHT = false;
	  }
		// rotation
	  else if (KEY_ROT)
		{
			uint8_t prev_rotation = piece.rotation;

			// new rotation
			piece.rotation = (piece.rotation + 1) % piece.shape.max_rotation;
			convert_shape_format(positions, piece);

			if(is_valid_space (locked_positions, piece))
			{
				// move shape to new position
				move_shape (screen, piece);
			}
			else
			{
				// restore shape
			  piece.rotation = prev_rotation;
				memcpy (positions, p_positions, sizeof (p_positions));
			}

			KEY_ROT = false;
		}

		// update the previous
		memcpy (p_positions, positions, sizeof(positions));
  }
}
