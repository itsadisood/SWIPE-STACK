#include "tetris.h"

// fixed size arrays
coord_t positions   [SHAPE_NUM_PIX];
coord_t p_positions [SHAPE_NUM_PIX];
Piece_t shape_queue [QUEUE_SIZE];

// locked positions array
// 24 x 44 (actually 24 x 64, but ignore upper 20 bits)
uint64_t locked_positions[HUB75_H] = 
{
  0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000,
  0xffffffffffffffff, 0xffffffffffffffff, 0x0000000000000003, 0x0000000000000003,
  0x0000000000000003, 0x0000000000000003, 0x0000000000000003, 0x0000000000000003,
  0x0000000000000003, 0x0000000000000003, 0x0000000000000003, 0x0000000000000003,
  0x0000000000000003, 0x0000000000000003, 0x0000000000000003, 0x0000000000000003,
  0x0000000000000003, 0x0000000000000003, 0x0000000000000003, 0x0000000000000003,
  0x0000000000000003, 0x0000000000000003, 0xffffffffffffffff, 0xffffffffffffffff,
  0x0000000000000000, 0x0000000000000000, 0x0000000000000000, 0x0000000000000000
};

static void
move_shape (pixel_t * screen, Piece_t piece)
{
  // undo prev state
  sr_coord_board (screen, p_positions, piece.color, 0);
  // form new state
  sr_coord_board (screen, positions, piece.color, 1);
}


//*************************************************************************************************
// is_valid_space checks the piece's position against the locked_positions array which is an array
// of 64-bit unsigned integers, where the array index represents the row, while the bits of the
// integer elements represent whether a column is occupied or not
//*************************************************************************************************
static bool
is_valid_space (uint64_t * locked_positions, Piece_t piece)
{
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
static bool
check_row_fill (uint8_t s, uint64_t mask)
{
  uint64_t block;

  for(int i = 0; i < BOARD_R; i++)
  {
    block = locked_positions [s + i] & mask;

    // check if it is filled
    if(!block) return false;
  }
  return true;
}

//static bool
//check_row_fill (uint8_t s, uint64_t mask)
//{
//	uint64_t prev, curr;
//	for(int i = 0; i < NUM_ROWS_BOARD - 5; i++)
//	{
//		prev = locked_positions [s + i] & mask;
//		curr = locked_positions [s + i + 1] & mask;
//    if (!prev) return false;
//    if (!curr) return false;
//		if ((prev != curr))
//		{
//			return false;
//		}
//	}
//	return true;
//}

static uint32_t
check_rows_clear ()
{
  uint32_t cols = 0;
  uint64_t mask = 0xc;

  for (int i = 0; i < BOARD_C / STEP; i++)
  {
    if (check_row_fill (6, mask))
    {
      cols |= 1 << i;
    }
    mask <<= STEP;
  }
  return cols;
}

static void 
update_lock_pos (int col)
{
  col *= 2;
  uint64_t upper, lower;
  for (int s = 6; s < 26; s++)
  {
    // get tthe lower loc_pos
    lower = locked_positions[s] & ((ROW_MASK >> (40 - col)));

    // get the upper loc_pos
    upper = locked_positions[s] & (ROW_MASK << (col + 2));
    upper >>= STEP; 

    locked_positions[s] = upper | lower; 
  }
}

static void
row_check (pixel_t * screen)
{
  int rows_cleared = 0;
  uint32_t cols = check_rows_clear ();

  if (cols != 0)
  {
    for (int i = 0; i < BOARD_C / STEP; i++)
    {
      if ((cols >> i) & 1)
      {
        clear_row(screen, i - rows_cleared);
        update_lock_pos (i - rows_cleared); 
        drop_rows (screen, i + 1 - rows_cleared);
        rows_cleared++;
      }
    }
  }
}

//*************************************************************************************************
// lock_pos will append the position of a stoppied piece into the locked_positions array
// The position will be locked until a row is cleared or the game ends
//*************************************************************************************************
static void
lock_pos(uint64_t * locked_positions, Piece_t piece)
{
  // convert_shape_format (positions, piece);
  uint64_t one = 1;

  for (int i = 0; i < SHAPE_NUM_PIX; i++)
  {
    locked_positions[positions[i].x] |= one << positions[i].y;
  }
}

static Piece_t
update_shape_queue (pixel_t * screen)
{
  Piece_t piece = dequeue_shape (shape_queue);
  enqueue_shape (shape_queue);
  disp_shape_queue (shape_queue, screen);
  return piece;
}

/**
 * @brief  Checks to see if the game is lost, positions of a
 *         piece exceeds `BOARD_TOP`
 * @param  None
 * @retval boolean (true or false) determing loss
*/
static bool
check_loss ()
{
  for (int i = 0; i < SHAPE_NUM_PIX; i++)
  {
    if (positions[i].y > BOARD_TOP)
    {
      return true;
    }
  }
  return false;
}

/**
 * @brief  Main game loop that runs the game
 * @param  screen 
 * @retval boolean (true or false) determing loss
*/
void
tetris (pixel_t * screen)
{
  // initialize game pieces
  Piece_t piece;
  // initialize some game driver parameters
  fall_time = 0;

  bool new_piece = true;
  game_init (screen);
  setup_tim3(1000, 10);

  init_shape_queue (shape_queue);
  piece = update_shape_queue (screen);

  while (true)
  {
    if (new_piece)
    {
      piece = update_shape_queue (screen);
      // blit onto screen
      convert_shape_format (positions, piece);
      sr_coord_board (screen, positions, piece.color, 1);

      new_piece = false;
    }
    // fall dowm
    else if (fall_time >= 3000)
    {
      // new position
      piece.y_coord -= STEP;
      convert_shape_format(positions, piece);

      if(is_valid_space (locked_positions, piece))
      {
        // move shape to new position
        move_shape (screen, piece);
      }
      else
      {
        // restore shape
        piece.y_coord += STEP;
        memcpy (positions, p_positions, sizeof (p_positions));

        if (check_loss ()) break;

        // set for new piece fetch
        new_piece = true;
        lock_pos(locked_positions, piece);

        // check if any rows can be cleared
        row_check (screen);
      }

      fall_time = 0;
    }
    // move left
    else if (MOVE == 'R')
    {
      // new position
      if (piece.y_coord <= (47 - piece.shape.pmap->height))
      {
        piece.x_coord -= STEP;
        convert_shape_format(positions, piece);

        if(is_valid_space(locked_positions, piece))
        {
          // move shape to new position
          move_shape (screen, piece);
        }
        else
        {
          // restore shape
          piece.x_coord += STEP;
          memcpy (positions, p_positions, sizeof (p_positions));
        }
      }

      MOVE = 'N';
    }
    // move right
    else if (MOVE == 'L')
    {
      // new position
      if(piece.y_coord <= (47 - piece.shape.pmap->height)) // INCLUDE CROPPED SHAPE HEIGHT
      {
        piece.x_coord += STEP;
        convert_shape_format(positions, piece);

        if(is_valid_space(locked_positions, piece))
        {
          // move shape to new position
          move_shape (screen, piece);
        }
        else
        {
          // restore shape
          piece.x_coord -= STEP;
          memcpy (positions, p_positions, sizeof (p_positions));
        }
      }

      MOVE = 'N';
    }
    // rotation
    else if (MOVE == 'U')
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

      MOVE = 'N';
    }

    // Quick Drop
    else if (MOVE == 'D')
    {
      while(is_valid_space (locked_positions, piece))
      {
        piece.y_coord -= STEP; 
        convert_shape_format (positions, piece);
      }

      // restore shape 
      piece.y_coord += STEP; 
      convert_shape_format (positions, piece); // restore proper positions for drawing
      move_shape (screen, piece);
      // memcpy (positions, p_positions, sizeof (p_positions));

      if (check_loss ()) break; 

      // set for new piece fetch 
      new_piece = true; 
      lock_pos(locked_positions, piece);

      // check if any rows can be cleared
      row_check (screen);

      MOVE = 'N';
    }

    // update the previous
    memcpy (p_positions, positions, sizeof(positions));
  }

  init_screen (screen, BLACK);
}
