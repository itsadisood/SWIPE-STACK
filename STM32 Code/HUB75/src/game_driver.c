# include "game_driver.h"

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

  // clear screen
  init_screen (screen, BLACK);

  // init the game board
  board_init (screen);

  // enable interrupts 
  init_exti();
}

void 
set_random_seed ()
{
  // turn on the ADC
  setup_adc ();

  // rand_seed = (ADC->DR & 0xf) ** 5 
  srand (pow (ADC1->DR & 0xf, 5));
}

void 
clear_row (pixel_t * screen, int y)
{
  uint8_t offset;
  y += 1;
  for(int x = 6; x < 26; x++)
  {
    // Compute the location
    uint8_t loc = (x & 0xf) * HUB75_C + (y * 2);

    if (x > 0xf)
    {
      offset = 3;
    }
    else
    {
      offset = 0;
    }

    // logic to clear the rows
    screen[loc].color &= ~(WHITE << offset);
    screen[loc + 1].color &= ~(WHITE << offset);
  }
}

void 
drop_rows (pixel_t * screen, int row)
{
  row++;
  pixel_t prev;
  uint8_t offset;  

  for(int x = 6; x < 26; x++)
  {
    for(int y = (row * 2); y < 41; y++)
    {
      // calculate the position
      uint16_t loc = (x & 0xf) * HUB75_C + y;

      if(x > 0xf)
      {
        offset = 3;
      }
      else 
      {
        offset = 0; 
      }

      // Logic to drop the rows
      prev = screen[loc];
      // undo previous state
      screen[loc].color &= ~(WHITE << offset);
      // drop the rows
      screen[loc - 2].color |= ((prev.color >> offset) & 0x7) << offset;
    }
  }
}

void
sr_coord_board (pixel_t * screen, coord_t * positions, hub75_color_t color, bool set)
{
  uint8_t offset;
  for (int i = 0; i < SHAPE_NUM_PIX; i++)
  {
    // Bounds check
    if (positions[i].y > BOARD_TOP) continue;

    uint16_t loc = (positions[i].x & 0xf) * HUB75_C + positions[i].y;
    
    if (positions[i].x > 0xf)
    {
      offset = 3;
    }
    else
    {
      offset = 0;
    }
    screen[loc].color &= ~(WHITE << offset);
    if (set)
    {
      screen[loc].color |= color << offset;
    }
  }
}

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

/*
  Upcounting Timer 3 to track elapsed game time
*/
void
setup_tim3 (uint32_t psc, uint32_t arr)
{
  // Turn on the clock for timer 3
  RCC -> APB1ENR |= RCC_APB1ENR_TIM3EN;
  // set the clk freq
  TIM3->PSC = psc-1;
  TIM3->ARR = arr-1;
  // Set for Upcounting
  TIM3->CR1 &= ~TIM_CR1_DIR;
  TIM3 -> DIER |= TIM_DIER_UIE;
  // Turn on timer
  TIM3->CR1 |= TIM_CR1_CEN;
  NVIC -> ISER[0] |= 0x00010000; // enable tim3 interrupt in NVIC
}

void
TIM3_IRQHandler()
{
  TIM3 -> SR &= ~(TIM_SR_UIF);
  fall_time += 1;

}

void EXTI0_1_IRQHandler()
{
  // clear pending flag for pc1
  EXTI -> PR = 0x2;
  KEY_ROT = true ;
}

void EXTI2_3_IRQHandler()
{
  // clear pending flag for pc2
  EXTI -> PR = 0x4; // clear pending flag for pc2
  KEY_LEFT = true;

}

void EXTI4_15_IRQHandler()
{
  // clear pending flag for pa8
  EXTI -> PR = 0x100; // clear pending flag for pa8
  KEY_RIGHT = true;
}

void init_exti()
{
  RCC -> AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;
  GPIOA -> MODER &= ~GPIO_MODER_MODER8;
  GPIOC -> MODER &= ~GPIO_MODER_MODER2;
  GPIOC -> MODER &= ~GPIO_MODER_MODER1;
  GPIOA -> PUPDR &= ~GPIO_PUPDR_PUPDR8;
  GPIOC -> PUPDR &= ~GPIO_PUPDR_PUPDR2;
  GPIOC -> PUPDR &= ~GPIO_PUPDR_PUPDR1;
  GPIOA -> PUPDR |= GPIO_PUPDR_PUPDR8_1;
  GPIOC -> PUPDR |= GPIO_PUPDR_PUPDR2_1;
  GPIOC -> PUPDR |= GPIO_PUPDR_PUPDR1_1;

  // 1-2
  RCC -> APB2ENR |= 0x1; // enable bit 0 (SYSCFGCOMPEN)
  SYSCFG -> EXTICR[0] &= 0xfffff00f; // clear port 2 and 1
  SYSCFG -> EXTICR[0] |= 0x00000200; // enable PC2 for EXTI2
  SYSCFG -> EXTICR[0] |= 0x00000020; // enable PC1 for EXTI2
  SYSCFG -> EXTICR[3] &= 0xfffffff0; // clear port 8
  SYSCFG -> EXTICR[3] |= 0x00000000; // enable PA8 for EXTI8

  // 3
  // enable rising edge interrupt on PC2, PA8, PC1
  EXTI -> RTSR |= EXTI_RTSR_TR2;
  EXTI -> RTSR |= EXTI_RTSR_TR8;
  EXTI -> RTSR |= EXTI_RTSR_TR1;

  // 4
  // unmask PC2, PA8 so interrupt can be seen by NVIC
  EXTI -> IMR |= EXTI_IMR_MR2;
  EXTI -> IMR |= EXTI_IMR_MR8;
  EXTI -> IMR |= EXTI_IMR_MR1;

  // 5
  // enable interrupts for EXTI pins 0-1, 2-3, and 4-15 e = 1110
  NVIC -> ISER[0] |= 0xe0;
}