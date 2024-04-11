/*
 * @file font.h
 *
 * @author Alex Chitsazzadeh
 * achitsaz@purdue.edu
 *
 * Pixel maps for fonts
 */

#ifndef __FONT__
#define __FONT__

#include "typeface.h"

#define NUM_LETTERS 26


typedef struct
{
  const map_t * pmap;
} letter_t;

typedef struct
{
  uint8_t x_coord;
  uint8_t y_coord;
  letter_t letter;
  hub75_color_t color;
} Font_t;

letter_t get_letter (uint8_t idx);

hub75_color_t get_letter_color(uint8_t idx);

Font_t font_init (uint8_t index, int row);



#endif
