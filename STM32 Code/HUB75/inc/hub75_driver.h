/*
 * hub75_driver.h
 *
 * Vishnu Lagudu
 * vlagudu@purdue.edu
 *
 * hub75_driver constants and functions
 */

#include "font.h"
#include "stm32f0xx.h"

#define HUB75_H 64 
#define HUB75_W 32

// functions

// define the pixel
typedef struct _pixel
{
	uint8_t color : 6;
} pixel;

// define the structure
typedef struct _map
{
	pixel * pmap;
	uint8_t height;
	uint8_t width;
} map;

// waste CPU clocks 
void nano_wait (unsigned int);

// initialize GPIO ports A and B for
// communicating with HUB75 matrix
void init_io (void);

// bit-bang clock out of GPIOA
void clock (void);

// bit-bang latch out of GPIOA
void latch (void);

void writebyte (uint8_t byte, uint8_t color);

// clear HUB75 display
void fill_disp (uint8_t color);

void showchar (uint8_t row, uint8_t color, uint8_t* char_map);

// bit bang font through GPIOA
// void draw_font (int, int, int, Font);
