#ifndef __GAME_DRIVER__
#define __GAME_DRIVER__

#include "hub75_driver.h"

#define BOARD_TOP 41

volatile bool KEY_LEFT;
volatile bool KEY_ROT;
volatile bool KEY_RIGHT;
volatile bool ROTATE;

// fall time for blocks
volatile uint32_t fall_time;


void game_init (pixel_t * screen);

void clear_row (pixel_t * screen, int y);

void drop_rows (pixel_t * screen, int row);

void setup_adc (void);

void set_random_seed ();

void setup_tim3(uint32_t psc, uint32_t arr); // setup game timer

void init_exti();

void sr_coord_board (pixel_t * screen, coord_t * positions, hub75_color_t color, bool set);

// IRQ Handlers
extern void EXTI0_1_IRQHandler();
extern void EXTI2_3_IRQHandler();
extern void EXTI4_15_IRQHandler();
extern void TIM3_IRQHandler();

#endif
