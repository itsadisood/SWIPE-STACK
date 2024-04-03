#ifndef __TETRIS__
#define __TETRIS__

#include "shape_queue.h"
#include <math.h>
// #include "stab.h"

#define MOVE 2
#define ROW_MASK 0x3fffffffffc

void tetris (pixel_t * screen);

#endif
