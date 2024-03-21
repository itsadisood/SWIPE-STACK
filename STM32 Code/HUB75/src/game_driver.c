# include "game_driver.h"

volatile uint64_t global_count = 0;

static void 
board_init (pixel_t * screen)
{
  sr_font (screen, 4, 0, get_board (), WHITE, 1);
}

void
game_init (pixel_t * screen)
{
  // set up the peripherals
  init_io ();
  setup_dma (screen);
  setup_tim2 (10, 2, 1);

  // enable dma transfers
  ed_dma (1);

  // create the initial game screen
  init_screen (screen, BLACK);
  board_init (screen);
}

// extern void TIM14_IRQHandler ()
// {
// 	TIM14 -> SR &= ~(TIM_SR_UIF);
//   global_count++;	
// }

// // Coinfigure the timer generate a random numnber
// void 
// setup_tim14 (void)
// {
// 	// Turn on the clock for timer 14
// 	RCC -> APB1ENR |= RCC_APB1ENR_TIM14EN;
// 	// set the clk freq to 100 Hz
// 	TIM14->PSC = (10-1);
// 	TIM14->ARR = (48-1);
// 	// Set for Upcounting
// 	TIM14->CR1 &= ~TIM_CR1_DIR;
// 	// Enable interrupt
// 	TIM14->DIER |= TIM_DIER_UIE;
// 	// Turn on timer
// 	TIM14->CR1 |= 0x1;
//   // Set up the vector table
//   NVIC -> ISER[0] = 1 << TIM14_IRQn;
// }

/**
  * @brief Coinfigure the ADC to
  * @param None
  * @retval None
**/
void 
setup_adc(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
  // set PC3 to anlog mode
	GPIOC->MODER |= 0x3 << 6;

	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC -> CR2 |= RCC_CR2_HSI14ON;
  // Wait for 14MHz internal clk
	while (!(RCC -> CR2 & RCC_CR2_HSI14RDY));
  // enable the ADC
	ADC1 -> CR |= ADC_CR_ADEN;
  // Wait for ADC ready
	while (!(ADC1 -> ISR & ADC_ISR_ADRDY));
	ADC1 -> CHSELR = ADC_CHSELR_CHSEL13;
	while (!(ADC1 -> ISR & ADC_ISR_ADRDY));
  // start the ADC
  ADC1 -> CR |= ADC_CR_ADSTART;
  // Wait for end of conversion
  while(!(ADC1->ISR & ADC_ISR_EOC));
}

