/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
#include "stdio.h"
#include "stdlib.h"
#include "stdarg.h"

typedef struct pixel{
  unsigned int red : 2;
  unsigned int green : 2;
  unsigned int blue : 2;
  unsigned int deadSpace : 5;
  unsigned int row : 5;
}pixel;


void wait(int size ){
  for(int i = 0; i < size * 10000; i++){
  }
}

void nanoWait(int size){
  for(int i = 0; i < 225 * size; i++){}
}


void setupDMA(void* addr) {
  RCC->AHBENR |= RCC_AHBENR_DMAEN;

  //Set data count for 64 columns by 16 rows each
  DMA1_Channel1->CNDTR = 64 * 16;

  DMA1_Channel1->CMAR = (uint32_t) addr;

  DMA1_Channel1->CPAR = (uint32_t) (&(GPIOB->ODR));

  //set memory access size to 32 bits
  DMA1_Channel1->CCR |= DMA_CCR_MSIZE_1;

  //set peripheral access size to 16 bits
  DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;

  //set minc to 1 and pinc to 0
  DMA1_Channel1->CCR &= ~DMA_CCR_PINC;
  DMA1_Channel1->CCR |= DMA_CCR_MINC;

  //set to circular
  DMA1_Channel1->CCR |= DMA_CCR_CIRC;
  //set to mem to peripheral direction
  DMA1_Channel1->CCR |= DMA_CCR_DIR;

  //TIM2 is the default mapping for DMA1 Channel1
  //DMA1->RMPCR |= DMA_RMPCR1_CH3_TIM2;


  //enable the channel
  DMA1_Channel1->CCR |= DMA_CCR_EN;



}

void setupTIM2(int freq) {
  int COUNT = 100;
  int rows = 32;
  int prescaler = (48000000 / ( freq * COUNT * rows)) - 1 ;
  int reload = (COUNT) - 1;

  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  GPIOB->MODER |= GPIO_MODER_MODER0_0;
  GPIOB->MODER |= GPIO_MODER_MODER1_0;
  GPIOB->MODER |= GPIO_MODER_MODER2_0;
  GPIOB->MODER |= GPIO_MODER_MODER3_0;
  GPIOB->MODER |= GPIO_MODER_MODER4_0;
  GPIOB->MODER |= GPIO_MODER_MODER5_0;
  GPIOB->MODER |= GPIO_MODER_MODER11_0;
  GPIOB->MODER |= GPIO_MODER_MODER12_0;
  GPIOB->MODER |= GPIO_MODER_MODER13_0;
  GPIOB->MODER |= GPIO_MODER_MODER14_0;
  GPIOB->MODER |= GPIO_MODER_MODER15_0;


  //set gpio A1 to output the Timer 2 Channel 2
  GPIOB->MODER |= GPIO_MODER_MODER10_1;


  //set AFR for pin 1 to the value 2
  GPIOB->AFR[1] |= 0x2 << 8;

  //set prescaler
  //TIM3->PSC |= TIM_PSC_PSC;

  //set prescaler
  TIM2->PSC = prescaler;
  //set ARR
  TIM2->ARR = reload;
  //set dir
  TIM2->CR1 &= ~TIM_CR1_DIR;

  //set OCcM to 110
  TIM2->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;

  //set CCR to
  TIM2->CCR3 = (COUNT) / 2;

  //Enable DMA
  TIM2->DIER |= TIM_DIER_CC3DE;

  //enable channel
  TIM2->CCER |= TIM_CCER_CC3E;


}
void setupTIM17(int freq) {
  int COUNT = 100;
  int SCALE = 64;
  int rows = 32;
  int prescaler = (48000000 / ( freq * COUNT * rows)) - 1 ;
  int reload = (COUNT * SCALE) - 1;

  RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  //set gpio b9 to output alternate function
  GPIOB->MODER |= GPIO_MODER_MODER9_1;

  //set AFR for pin 1 to the value 2
  GPIOB->AFR[1] |= 0x2 << 4;

  //set prescaler
  TIM17->PSC = prescaler;
  TIM17->ARR = reload;
  //set dir
  TIM17->CR1 &= ~TIM_CR1_DIR;

  //set OCcM to 110
  TIM17->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;

  //set CCR to
  TIM17->CCR1 = (COUNT - (SCALE));

  TIM17->BDTR |= TIM_BDTR_MOE;

  //enable channel
  TIM17->CCER |= TIM_CCER_CC1E;


}

void setup_serial(void)
{
    RCC->AHBENR |= 0x00180000;
    GPIOC->MODER  |= 0x02000000;
    GPIOC->AFR[1] |= 0x00020000;
    GPIOD->MODER  |= 0x00000020;
    GPIOD->AFR[0] |= 0x00000200;
    RCC->APB1ENR |= 0x00100000;
    USART5->CR1 &= ~0x00000001;
    USART5->CR1 |= 0x00008000;
    USART5->BRR = 0x340;
    USART5->CR1 |= 0x0000000c;
    USART5->CR1 |= 0x00000001;
}


int main(void)
{
  pixel* screen = malloc(sizeof(pixel) * 16 * 64);
  int freq = 1;

  setupTIM2(freq);
  setupTIM17(freq);
  TIM2->CR1 |= TIM_CR1_CEN;
  TIM17->CR1 |= TIM_CR1_CEN;




//  uint addr1 = &(screen[0][0]);
//  uint addr2 = &(screen[1][0]);
//  uint addr3 = &(screen[0][1]);
//
//  int structSize  = (uint)&(screen[1][0]) - (uint)&(screen[0][0]);
//  int structSize2 = (uint)&(screen[0][1]) - (uint)&(screen[0][0]);
//
//  int addrSize = addr2 - addr1;
//  int addrSize2 = addr3 - addr1;
//
//  size_t size = sizeof(pixel);
//  size_t intSize = sizeof(int);


  int red, green, blue;

  int column = 16;
  int row = 64;

int index = 0;
for(int i = 0; i < column; i++) {
  for(int j = 0; j < row; j++){
    index = (i*row) + j;
    if(i % 3 == 0){
      screen[index].red = 3;
      screen[index].green = 3;
      screen[index].blue = 3;
    }
    else if (i % 2 == 0) {
      screen[index].red = 3;
      screen[index].green = 3;
      screen[index].blue = 3;
    }
    else{
      screen[index].red = 3;
      screen[index].green = 3;
      screen[index].blue = 3;
    }

    screen[index].row = i;
    red = screen[index].red;
    green = screen[index].green;
    blue = screen[index].blue;



  }
}
int arr[64*32];

for(int i = 0; i < 2048; i++){
  arr[i] = i;
}

setupDMA(screen);


volatile int l = 1;
int j = 0;
while(l){
  for(int i = 0; i< 200; i++){
    j = i * 10 + j;
  }
}
j = 20;

}
