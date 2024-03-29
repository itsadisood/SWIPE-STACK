/*
 * @file font.h
 *
 * @author Vishnu Lagudu
 * vlagudu@purdue.edu
 *
 * Pixel maps for game fonts
 */

#ifndef __TYPEFACE__
#define __TYPEFACE__

#include "stm32f0xx.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define NUM_SHAPES 7
#define SHAPE_NUM_PIX 16
#define NUM_ROWS_BOARD 24
#define NUM_COLS_BOARD 44

// colors
typedef enum 
{
  BLACK  = 0,
  RED    = 1,
  GREEN  = 2,
  YELLOW = 3,
  BLUE   = 4,
  PINK   = 5,
  TEAL   = 6,
  WHITE  = 7,
} hub75_color_t;

typedef struct 
{
  uint8_t width; // the no bits in number
	uint8_t height; // height
	const uint64_t * pmap; // pixel map
} map_t;

typedef struct
{
  const map_t * pmap;
  uint8_t max_rotation;
} shape_t;

map_t get_board (void);

shape_t get_shape (uint8_t idx);

hub75_color_t get_shape_color (uint8_t idx);

#endif
