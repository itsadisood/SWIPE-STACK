#ifndef __GAME_DRIVER__
#define __GAME_DRIVER__

#include "hub75_driver.h"

volatile bool goLeft;
volatile bool goRight;
volatile uint32_t fall_time;


void game_init (pixel_t * screen);

// void setup_tim14 (void);

void setup_adc (void);
void setup_tim3(uint32_t psc, uint32_t arr); // setup game timer
void init_exti();
extern void EXTI0_1_IRQHandler();
extern void EXTI2_3_IRQHandler();
extern void EXTI4_15_IRQHandler();
extern void TIM3_IRQHandler();


#endif
