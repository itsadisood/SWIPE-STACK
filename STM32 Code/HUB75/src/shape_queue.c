#include "shape_queue.h"

coord_t pos1 [SHAPE_NUM_PIX];
coord_t pos2 [SHAPE_NUM_PIX];
coord_t pos3 [SHAPE_NUM_PIX];

coord_t p_pos1 [SHAPE_NUM_PIX];
coord_t p_pos2 [SHAPE_NUM_PIX];
coord_t p_pos3 [SHAPE_NUM_PIX];

static int 
gen_rand_int (int lower, int upper)
{
  // Seed the random number generator using the current time
  uint64_t curr_seed = rand ();
  srand(curr_seed);
  return ((rand() % (upper - lower + 1)) + lower);
}

Piece_t
piece_init (void)
{
  // get a random number
  int rand_idx = gen_rand_int (0, 6);

  // initialize the piece values
  Piece_t new_piece;
  new_piece.x_coord = 10; // start piece in middle-top of grid
  new_piece.y_coord = 61;
  new_piece.shape = get_shape (1);
  new_piece.color = get_shape_color (rand_idx);
  new_piece.rotation = 0;
  return new_piece;
}

void
test (coord_t * positions, Piece_t piece)
{
  uint8_t pos = 0;
  map_t format = piece.shape.pmap[piece.rotation];
  for (int i = 0; i < SHAPE_H; i++)
  {
    uint64_t doubleword = format.pmap[i];
    for (int j = 0; j < SHAPE_W; j++)
    {
      if ((doubleword & 1))
      {
				// if ((piece.y_coord - i) <= 41)
				// {
				// 	positions[pos].x = piece.x_coord - j;
				// 	positions[pos].y = piece.y_coord - i;
				// }
				// else
				// {
				// 	positions[pos].x = 255;
				// 	positions[pos].y = 255;
				// }
        positions[pos].x = piece.x_coord - j;
        positions[pos].y = piece.y_coord - i;
        pos++;
      }
      doubleword >>= 1; 
    }
  }
}

void
init_shape_queue (Piece_t * shape_queue)
{
  shape_queue[0] = piece_init ();
  shape_queue[1] = piece_init ();
  shape_queue[1].x_coord += 10;
  shape_queue[2] = piece_init ();
  shape_queue[2].x_coord += 20;

  test (p_pos1, shape_queue[0]);
  test (p_pos2, shape_queue[1]);
  test (p_pos3, shape_queue[2]);
}

void 
enqueue_shape (Piece_t * shape_queue)
{
  shape_queue[2] = shape_queue[1];
  shape_queue[1] = shape_queue[0];
  shape_queue[0] = piece_init ();
  shape_queue[1].x_coord += 10;
  shape_queue[2].x_coord += 10;
}

Piece_t
dequeue_shape (Piece_t * shape_queue)
{
  shape_queue[2].x_coord = 19; // start piece in middle-top of grid
  shape_queue[2].y_coord = 47;
  return shape_queue[2];
}

void
disp_shape_queue (Piece_t * shape_queue, pixel_t * screen)
{
  sr_coord (screen, p_pos1, WHITE, 0);
  sr_coord (screen, p_pos2, WHITE, 0);
  sr_coord (screen, p_pos3, WHITE, 0);

  test (pos1, shape_queue[0]);
  test (pos2, shape_queue[1]);
  test (pos3, shape_queue[2]);

  sr_coord (screen, pos1, shape_queue[0].color, 1);
  sr_coord (screen, pos2, shape_queue[1].color, 1);
  sr_coord (screen, pos3, shape_queue[2].color, 1);

  memcpy (p_pos1, pos1, sizeof(pos1));
  memcpy (p_pos2, pos2, sizeof(pos2));
  memcpy (p_pos3, pos3, sizeof(pos3));

}
