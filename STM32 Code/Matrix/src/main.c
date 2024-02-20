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
  unsigned int spacer : 1;
  unsigned int red : 2;
  unsigned int green : 2;
  unsigned int blue : 2;
  unsigned int latch : 1;
  unsigned int deadSpace : 3;
  unsigned int row : 5;
}pixel;

void drawShape(pixel* screen, pixel* shape, int x, int y, int locx, int locy){
  int screenIndex = 0;
  int shapeIndex = 0;


  for(int i = 0; i < x; i++) {
    for(int j = 0; j < y; j++) {
      shapeIndex = (i * x) + y;
      screenIndex = ((i * x) * 64) + (j + y);
      screen[screenIndex].red = shape[shapeIndex].red;
      screen[screenIndex].green = shape[shapeIndex].green;
      screen[screenIndex].blue = shape[shapeIndex].blue;
    }
  }
}
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

void setupTIM2(uint32_t psc, uint32_t arr, uint32_t ccr) {
  //prescaler to 1,000 so we get 48,000 hz out
  uint32_t prescaler =  psc;//(0x03E8/scaler) - 1;
  //reload to 100 so we get 480 hz out
  uint32_t reload = arr;//(100) - 1;

  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  GPIOB->MODER |= GPIO_MODER_MODER0_0;
  GPIOB->MODER |= GPIO_MODER_MODER1_0;
  GPIOB->MODER |= GPIO_MODER_MODER2_0;
  GPIOB->MODER |= GPIO_MODER_MODER3_0;
  GPIOB->MODER |= GPIO_MODER_MODER4_0;
  GPIOB->MODER |= GPIO_MODER_MODER5_0;
  GPIOB->MODER |= GPIO_MODER_MODER6_0;
  GPIOB->MODER |= GPIO_MODER_MODER7_0;
  GPIOB->MODER |= GPIO_MODER_MODER8_0;
  GPIOB->MODER |= GPIO_MODER_MODER11_0;
  GPIOB->MODER |= GPIO_MODER_MODER12_0;
  GPIOB->MODER |= GPIO_MODER_MODER13_0;
  GPIOB->MODER |= GPIO_MODER_MODER14_0;
  GPIOB->MODER |= GPIO_MODER_MODER15_0;


  //set gpio A1 to output the Timer 2 Channel 2
  GPIOB->MODER |= GPIO_MODER_MODER10_1;


  //set AFR for pin 1 to the value 2
  GPIOB->AFR[1] |= 0x2 << (4 * 2);

  //set prescaler
  //TIM3->PSC |= TIM_PSC_PSC;

  //set prescaler
  TIM2->PSC = prescaler;
  //set ARR
  TIM2->ARR = reload;
  //set dir
  TIM2->CR1 &= ~TIM_CR1_DIR;

  //set OCcM to 111 PWM mode 2
  TIM2->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2| TIM_CCMR2_OC3M_0;

  //set CCR to
  TIM2->CCR3 = ccr;//(reload + 1) / 2;

  //Enable DMA
  TIM2->DIER |= TIM_DIER_CC3DE;

  TIM2->CR2 |= TIM_CR2_MMS_0; /* (1)*/
  TIM2->SMCR |= /*TIM_SMCR_TS_1 | TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 |*/ TIM_SMCR_MSM; /* (2) */

  //enable channel
  TIM2->CCER |= TIM_CCER_CC3E;


}

void setupTIM3(uint32_t psc, uint32_t arr, uint32_t ccr) {
  //prescaler to 1,000 so we get 48,000 hz out
  int prescaler =  psc; //(0x03E8/scaler) - 1;
  //reload to 100 so we get 480 hz out
  int reload =  arr;//(64 * 100) - 1;

  RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  //set gpio b9 to output alternate function
  GPIOB->MODER &= ~GPIO_MODER_MODER0_0;
  GPIOB->MODER |= GPIO_MODER_MODER0_1;

  //set AFR for pin 1 to the value 2
  GPIOB->AFR[0] |= 0x1  << (4 * 0);

  //set prescaler
  TIM3->PSC = prescaler;
  TIM3->ARR = reload;
  TIM3->CCR3 = ccr;

  //set dir
  TIM3->CR1 &= ~TIM_CR1_DIR;

  //set OCcM to 110
  TIM3->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2| TIM_CCMR2_OC3M_0;

  //set slave mode
  TIM3->SMCR |= TIM_SMCR_TS_0  | TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1;

  //enable channel
  TIM3->CCER |= TIM_CCER_CC3E;
}


void setupTIM17(int scaler) {
  //prescaler to 1,000 so we get 48,000 hz out
  int prescaler =  (0x03E8/scaler) - 1;
  //reload to 100 so we get 480 hz out
  int reload =  (64 * 100) - 1;

  RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;
  RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

  //set gpio b9 to output alternate function
  GPIOB->MODER |= GPIO_MODER_MODER9_1;
  //GPIOB->MODER |= GPIO_MODER_MODER7_1;

  //set AFR for pin 1 to the value 2
  GPIOB->AFR[1] |= 0x2 << 4;
  //GPIOB->AFR[0] |= 0x2 << (4 * 7);

  //set prescaler
  TIM17->PSC = prescaler;
  TIM17->ARR = reload;
  //set dir
  TIM17->CR1 &= ~TIM_CR1_DIR;

  //set OCcM to 110
  TIM17->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2| TIM_CCMR1_OC1M_0;

  //set CCR to
  TIM17->CCR1 = ((reload+1) - 64);

  TIM17->BDTR |= TIM_BDTR_MOE;

  //enable channel
  TIM17->CCER |= TIM_CCER_CC1E;
  TIM17->CCER |= TIM_CCER_CC1NE;


}


int main(void)
{
  pixel* screen = malloc(sizeof(pixel) * 16 * 64);
  pixel* square = malloc(sizeof(pixel) * 8 * 8);

  for(int i = 0; i < 8; i++){
    for(int j = 0; j < 8; j++){
      square[(i*8)+j].red = 1;
      square[(i*8)+j].blue = 1;
      square[(i*8)+j].green = 1;
    }
  }

  int freq = 10;
  int scaler = 1;
  //0x03E8/scaler
  //setup(psc, arr, ccr);
  setupTIM2(117-1, (2 * scaler) - 1, (2 * scaler) / 2);
  setupTIM3(1000-1, (128 * scaler) - 1, (scaler*127));


  setupTIM17(freq);



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

  int row = 16;
  int column = 64;

int index = 0;
//setup rows

for(int i = 0; i < row; i++) {
  for(int j = 0; j < column; j++){

    index = (i*column) + j;
     if(j < 10) {
       screen[index].red = 3;
       screen[index].green = 1;
       screen[index].blue = 3;
     }
     else{
      if(i % 3 == 0){
        screen[index].red = 1;
        screen[index].green = 0;
        screen[index].blue = 2;
      }
      else if (i % 2 == 0) {
        screen[index].red = 1;
        screen[index].green = 2;
        screen[index].blue = 0;
      }
      else{
        screen[index].red = 0;
        screen[index].green = 1;
        screen[index].blue = 2;
      }
    }

    if(j == 63) {
      screen[index].latch = 1;
    }
    else{
      screen[index].latch = 0;
    }

    screen[index].row = i;
    red = screen[index].red;
    green = screen[index].green;
    blue = screen[index].blue;
  }
}


TIM2->EGR |= TIM_EGR_UG;
TIM3->EGR |= TIM_EGR_UG;

TIM2->CR1 |= TIM_CR1_CEN;
//TIM3->CR1 |= TIM_CR1_CEN;
//TIM17->CR1 |= TIM_CR1_CEN;

int arr[64*32];

for(int i = 0; i < 2048; i++){
  arr[i] = i;
}

setupDMA(screen);

while(1) {
  for(int j = 0; j < 4096; j++){
    nanoWait(10);
    for(int i = 0; i < 1024; i++) {
//      if(j % 3 == 0){
//        screen[i].red = 0;
//        screen[i].green = 0;
//        screen[i].blue = 3;
//      }
//      else if (j % 2 == 0) {
//        screen[i].red = 0;
//        screen[i].green = 3;
//        screen[i].blue = 0;
//      }
//      else{
//        screen[i].red = 3;
//        screen[i].green = 0;
//        screen[i].blue = 0;
//      }
      if( i == j/4){
              screen[i].red = 3;
              screen[i].green = 0;
              screen[i].blue = 0;
      }
      else{
        screen[i].red = 3;
        screen[i].green = 3;
        screen[i].blue = 3;
      }
    }
  }
}

//drawShape(screen, square, 8, 8, 0,0);

volatile int l = 1;
int j = 0;
while(l){
  for(int i = 0; i< 200; i++){
    j = i * 10 + j;
  }
}
j = 20;

}
