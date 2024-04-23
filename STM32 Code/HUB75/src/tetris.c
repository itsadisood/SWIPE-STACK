#include "tetris.h"
#include "font.h"
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

char scoreBuff[10] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
char levelBuff[3] = {0x0, 0x0, 0x0};

uint16_t level    = 0;
uint64_t scoreNum = 0;


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

  switch(rows_cleared)
  {
    case 1: scoreNum += 40   * (level + 1); // Single
            break;
    case 2: scoreNum += 100  * (level + 1); // Double
            break;
    case 3: scoreNum += 300  * (level + 1); // Triple
            break;
    case 4: scoreNum += 1200 * (level + 1); // Tetris
            break;
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

// return game variables to initial state for next run
void clean_up()
{
	uint64_t og_positions[HUB75_H] =
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
	scoreNum = 0;
	level    = 0;
	init_score();

	memcpy(locked_positions, og_positions, sizeof(og_positions));
}

/**
 * @brief  Main game loop that runs the game
 * @param  screen 
 * @retval integer containing score
*/
int
tetris (pixel_t * screen)
{
  // clean up from last game
  clean_up();
  int levelUpThreshold = (level + 1) * 200;

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
    else if (fall_time >= (BASE_TIME - (level * 10)))
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
    else if (MOVE == 'L')
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
    else if (MOVE == 'R')
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
        scoreNum += 1; // award bonus
      }

      // restore shape 
      piece.y_coord += STEP; 
      scoreNum -= 1;
      // restore proper positions for drawing
      convert_shape_format (positions, piece);
      move_shape (screen, piece);

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

    if(scoreNum > levelUpThreshold)
    {
    	level++;
    	levelUpThreshold += ((level + 1) * 200);
    }

    // update scoreBuffer
    sprintf(scoreBuff, "%llu", scoreNum);
    sprintf(levelBuff, "%hu", level);
    updt_score(scoreBuff, levelBuff);
  }

  init_screen (screen, BLACK);
  return scoreNum;
}

/**
 * @brief  End Screen for Game
 * @param  screen
*/
void
lose_screen (pixel_t * screen)
{
	Font_t G = font_init(6, 0);
	sr_font (screen, G.x_coord, G.y_coord, G.letter.pmap[0], RED, 1);

	Font_t A = font_init(0, 0);
	sr_font (screen, A.x_coord, A.y_coord + 8, A.letter.pmap[0], RED, 1);

	Font_t M = font_init(12, 0);
	sr_font (screen, M.x_coord, M.y_coord + 16, M.letter.pmap[0], RED, 1);

	Font_t E = font_init(4, 0);
	sr_font (screen, E.x_coord, E.y_coord + 24, E.letter.pmap[0], RED, 1);

	Font_t O = font_init(14, 1);
	sr_font (screen, O.x_coord, O.y_coord, O.letter.pmap[0], RED, 1);

	Font_t V = font_init(21, 1);
	sr_font (screen, V.x_coord, V.y_coord + 8, V.letter.pmap[0], RED, 1);

	Font_t E_2 = font_init(4, 1);
	sr_font (screen, E_2.x_coord, E_2.y_coord + 16, E_2.letter.pmap[0], RED, 1);

	Font_t R = font_init(17, 1);
	sr_font (screen, R.x_coord, R.y_coord + 24, R.letter.pmap[0], RED, 1);

	while(1)
	{
		if(MOVE == 'X')
		{
			clean_up(); // clear score and other game vars
			MOVE = 'N';
			return;
		}
	}

}

/**
 * @brief  End Screen for Game
 * @param  screen
*/
void
splash_screen (pixel_t * screen)
{
	init_screen (screen, BLACK);

	Font_t S = font_init(18, 0);
	sr_font (screen, S.x_coord, S.y_coord, S.letter.pmap[0], S.color, 1);

	Font_t W = font_init(22, 0);
	sr_font (screen, W.x_coord, W.y_coord + 8, W.letter.pmap[0], W.color, 1);

	Font_t I = font_init(8, 0);
	sr_font (screen, I.x_coord, I.y_coord + 16, I.letter.pmap[0], I.color, 1);

	Font_t P = font_init(15, 0);
	sr_font (screen, P.x_coord, P.y_coord + 24, P.letter.pmap[0], P.color, 1);

	Font_t E = font_init(4, 0);
	sr_font (screen, E.x_coord, E.y_coord + 32, E.letter.pmap[0], E.color, 1);

	Font_t S_2 = font_init(18, 1);
	sr_font (screen, S_2.x_coord, S_2.y_coord, S_2.letter.pmap[0], S_2.color, 1);

	Font_t T = font_init(19, 1);
	sr_font (screen, T.x_coord, T.y_coord + 8, T.letter.pmap[0], T.color, 1);

	Font_t A = font_init(0, 1);
	sr_font (screen, A.x_coord, A.y_coord + 16, A.letter.pmap[0], A.color, 1);

	Font_t C = font_init(2, 1);
	sr_font (screen, C.x_coord, C.y_coord + 24, C.letter.pmap[0], C.color, 1);

	Font_t K = font_init(10, 1);
	sr_font (screen, K.x_coord, K.y_coord + 32, K.letter.pmap[0], K.color, 1);




	// mimic blocks falling for splash screen
	Piece_t piece;
	fall_time = 0;

	bool new_piece = true;
	setup_tim3(1000, 10);
	while(1)
	{
		if(MOVE == 'X')
		{
			break;
		}
		if (new_piece)
		{
			piece = piece_init();
			piece.x_coord = 19 - 1;
			piece.y_coord = 47;
			// blit onto screen
			convert_shape_format (positions, piece);
			sr_coord_board (screen, positions, piece.color, 1);

			new_piece = false;
		}
		// fall dowm
		else if (fall_time >= (BASE_TIME - 2800))
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

			  	if (check_loss())
			  	{
			  		init_screen(screen, BLACK);
			  		clean_up();

			  		Font_t S = font_init(18, 0);
			  		sr_font (screen, S.x_coord, S.y_coord, S.letter.pmap[0], S.color, 1);

			  		Font_t W = font_init(22, 0);
			  		sr_font (screen, W.x_coord, W.y_coord + 8, W.letter.pmap[0], W.color, 1);

			  		Font_t I = font_init(8, 0);
			  		sr_font (screen, I.x_coord, I.y_coord + 16, I.letter.pmap[0], I.color, 1);

			  		Font_t P = font_init(15, 0);
			  		sr_font (screen, P.x_coord, P.y_coord + 24, P.letter.pmap[0], P.color, 1);

			  		Font_t E = font_init(4, 0);
			  		sr_font (screen, E.x_coord, E.y_coord + 32, E.letter.pmap[0], E.color, 1);

			  		Font_t S_2 = font_init(18, 1);
			  		sr_font (screen, S_2.x_coord, S_2.y_coord, S_2.letter.pmap[0], S_2.color, 1);

			  		Font_t T = font_init(19, 1);
			  		sr_font (screen, T.x_coord, T.y_coord + 8, T.letter.pmap[0], T.color, 1);

			  		Font_t A = font_init(0, 1);
			  		sr_font (screen, A.x_coord, A.y_coord + 16, A.letter.pmap[0], A.color, 1);

			  		Font_t C = font_init(2, 1);
			  		sr_font (screen, C.x_coord, C.y_coord + 24, C.letter.pmap[0], C.color, 1);

			  		Font_t K = font_init(10, 1);
			  		sr_font (screen, K.x_coord, K.y_coord + 32, K.letter.pmap[0], K.color, 1);

			  		continue; // continue scrolling new blocks

			  	}

			  	// set for new piece fetch
			  	new_piece = true;
			  	lock_pos(locked_positions, piece);
			 }

			 fall_time = 0;
		}
		// update the previous
		memcpy (p_positions, positions, sizeof(positions));
	}
}
