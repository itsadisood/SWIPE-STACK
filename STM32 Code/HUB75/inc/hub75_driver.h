/*
 * hub75_driver.h
 *
 * Vishnu Lagudu
 * vlagudu@purdue.edu
 *
 * hub75_driver constants and functions
 */

#ifndef __HUB75_DRIVER__
#define __HUB75_DRIVER__

#include "typeface.h"

#define HUB75_R 16
#define HUB75_C 64
#define HUB75_H HUB75_R << 1
#define MEMSIZE HUB75_R * HUB75_C


typedef struct
{
  int color  : 6;
  int space1 : 2;
  int latch  : 1;
  int blank  : 1;
  int space2 : 1;
  int raddr  : 5;
} pixel_t;

typedef struct
{
  uint8_t x;
  uint8_t y;
} coord_t;

void nano_wait (unsigned int);

void init_io (void);

void setup_dma (void * addr);

void ed_dma (bool DMA_FLG);

void setup_tim2 (uint32_t psc, uint32_t arr, uint32_t ccr);

void init_screen (pixel_t * screen, hub75_color_t color);

void sr_font (pixel_t * screen, uint8_t row, uint8_t col, const map_t typeface, hub75_color_t color, bool set);

void sr_coord_hub75 (pixel_t * screen, coord_t * positions, hub75_color_t color, bool set);

#endif
