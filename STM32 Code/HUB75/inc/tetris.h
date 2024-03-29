#ifndef __TETRIS__
#define __TETRIS__

#include "shape_queue.h"
#include <math.h>
// #include "stab.h"

#define MOVE 2
#define ROW_MASK 0x3fffffffffc

Piece_t piece_init (void);

void tetris (pixel_t * screen);

void convert_shape_format (coord_t * positions, Piece_t piece);

bool is_valid_space (uint64_t * locked_positions, Piece_t piece);

void lock_pos (uint64_t * locked_position, Piece_t piece);

uint32_t check_clear ();

bool check_row (uint8_t s, uint64_t mask);

#endif
