#ifndef __SHAPE_QUEUE__
#define __SHAPE_QUEUE__

#include "game_driver.h"

#define QUEUE_SIZE 3
#define SHAPE_H 8
#define SHAPE_W 8

typedef struct
{
  uint8_t x_coord;
  uint8_t y_coord;
  shape_t shape;
  hub75_color_t color;
  int8_t rotation;
} Piece_t;

void init_shape_queue (Piece_t * shape_queue);

void convert_shape_format (coord_t * positions, Piece_t piece);

void enqueue_shape (Piece_t * shape_queue);

Piece_t dequeue_shape (Piece_t * shape_queue);

void disp_shape_queue (Piece_t * shape_queue, pixel_t * screen);

#endif