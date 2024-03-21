/*
 * font.h
 *
 * Vishnu Lagudu
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

extern volatile uint64_t global_count;

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
  uint8_t width; // width
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

// Pixel maps
// static const uint8_t A [] =
// {
//     0b0010000,
//     0b0101000,
//     0b1000100,
//     0b1111100,
//     0b1000100,
//     0b1000100,
//     0b1000100
// };

// static const uint8_t B [] =
// {
//     0b1111000,
//     0b1000100,
//     0b1000100,
//     0b1111000,
//     0b1000100,
//     0b1000100,
//     0b1111000
// };

// static const uint8_t C [] =
// {
//     0b0111100,
//     0b1000000,
//     0b1000000,
//     0b1000000,
//     0b1000000,
//     0b1000000,
//     0b0111100
// };
// static const uint64_t board [] = 
// // {

// // }

// typedef struct _Font
// {
// 	uint8_t        w;    // width
// 	uint8_t        h;    // height
// 	const uint8_t* pmap; // pixel map
// } Font;

// static const Font fonts[] =
// {
// 	{5, 7, A},
// 	{5, 7, B},
// 	{5, 7, C}
// };
