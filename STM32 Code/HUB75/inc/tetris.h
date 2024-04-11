#ifndef __TETRIS__
#define __TETRIS__

#include "shape_queue.h"
#include <math.h>
#include "bluetooth_driver.h"
#include "oled_driver.h"

#define BASE_TIME 3000

int  tetris        (pixel_t * screen);
void splash_screen (pixel_t * screen);
void lose_screen   (pixel_t * screen);

#endif
