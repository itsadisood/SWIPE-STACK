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

// waste CPU clocks 
void nano_wait (unsigned int);

// initialize GPIO ports A and B for
// communicating with HUB75 matrix
void init_io (void);

// bit-bang clock out of GPIOA
void clock (void);

// bit-bang latch out of GPIOA
void latch (void);

// clear HUB75 display
void fill_disp (uint8_t);

// bit bang font through GPIOA
void draw_font (int, int, int, Font);
