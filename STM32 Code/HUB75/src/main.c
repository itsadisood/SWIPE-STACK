/*
 * main.c
 *
 * Vishnu Lagudu
 * vlagudu@purdue.edu
 *
 * Drive the LED matrix
*/

#include "tetris.h"

  uint8_t h [] = {0, 0x7f, 0x08, 0x08, 0x08, 0x7f, 0, 0};
  uint8_t a [] = {0, 0x7c, 0x12, 0x11, 0x12, 0x7c, 0, 0};
  uint8_t d [] = {0, 0x7f, 0x41, 0x41, 0x41, 0x3e, 0, 0};
  uint8_t b [] = {0, 0x7f, 0x49, 0x49, 0x49, 0x36, 0, 0};
  uint8_t t [] = {0, 0x60, 0x40, 0x7f, 0x40, 0x60, 0, 0};
  uint8_t f [] = {0, 0x7f, 0x48, 0x48, 0x48, 0x40, 0, 0};
  uint8_t e [] = {0, 0x7f, 0x49, 0x49, 0x49, 0x41, 0, 0};

pixel_t screen [MEMSIZE];

int
main ()
{
  // game_init (screen);
  setup_adc ();
  // set the initial random seed
  srand (pow(ADC1->DR, 3));
  // setup_tim14 ();
  Piece_t piece = piece_init ();
  // nano_wait (10000000/2);
  Piece_t piece_2 = piece_init ();
  // nano_wait (10000000/0.5);
  Piece_t piece_3 = piece_init ();
  // nano_wait (10000000/4);
  Piece_t piece_4 = piece_init ();
  // nano_wait (10000000/3);
  Piece_t piece_5 = piece_init ();

  init_io ();
  setup_dma (screen);
  setup_tim2 (10, 2, 1);

  // enable dma transfers
  ed_dma (1);

  // create the initial game screen
  init_screen (screen, BLACK);
  sr_font (screen, 0, 0, piece.shape.pmap[piece.rotation], piece.color, 1);
  sr_font (screen, 8, 0, piece_2.shape.pmap[piece_2.rotation], piece_2.color, 1);
  sr_font (screen, 16, 0, piece_3.shape.pmap[piece_3.rotation], piece_3.color, 1);
  sr_font (screen, 24, 0, piece_4.shape.pmap[piece_4.rotation], piece_4.color, 1);
  sr_font (screen, 0, 8, piece_5.shape.pmap[piece_5.rotation], piece_5.color, 1);
}
