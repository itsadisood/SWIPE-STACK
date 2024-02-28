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
#include "math.h"

typedef struct __attribute__((__packed__))pixel{
  unsigned int spacer : 1;
  unsigned int red : 2;
  unsigned int blue : 2;
  unsigned int green : 2;
  unsigned int latch : 1;
  unsigned int deadSpace : 3;
  unsigned int row : 5;
}pixel;

typedef struct __attribute__((__packed__))pixel4{
  unsigned int red : 2;
  unsigned int blue : 2;
  unsigned int green : 2;
}pixel4;

typedef struct __attribute__((__packed__))sprite{
  pixel4* image;
  uint8_t xSize;
  uint8_t ySize;
  uint8_t curPosx;
  uint8_t curPosy;
  uint8_t nextPosx;
  uint8_t nextPosy;
}sprite;


pixel screen[16 * 64 * 3];
pixel screen2[16 * 64 * 3];
pixel4 screen4[32 * 64];
int bufSel = 0;

const uint16_t sineLookupTable[] = {
2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2,
2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1};

void fastPrintScreen(sprite* image, pixel*screen) {
  //i is row, j is column
  int index = 0;
  int screenIndex = 0;
  int halfSize = (image->xSize * image->ySize) / 2;
  pixel4* picture = image->image;

  //loop for each color plane
  for(int s = 0; s < 1; s++){
    //loop for each column in the image
    for(int i = 0; i < image->xSize; i++){
      //loop for each row in the image
      for(int j = 0; j < image->ySize; j++){
        //find index into image array
        index = (i * image->ySize) + j;
        screenIndex = ((i + image->curPosx) * 64) + (j + image->curPosy);
        //              layer offset                          index into 64x32                      index of halfway + current index
//        screen[screenIndex].red   = (0b01 * (picture[index].red   >= (s+1))) + (0b10 * (picture[index + (halfSize)].red   >= (s+1)));
//        screen[screenIndex].blue  = (0b01 * (picture[index].blue  >= (s+1))) + (0b10 * (picture[index + (halfSize)].blue  >= (s+1)));
//        screen[screenIndex].green = (0b01 * (picture[index].green >= (s+1))) + (0b10 * (picture[index + (halfSize)].green >= (s+1)));
          if(screenIndex < 1024) {
            screen[screenIndex].red   |= (0b01 * (picture[index].red   >= (s+1)));
            screen[screenIndex].blue  |= (0b01 * (picture[index].blue  >= (s+1)));
            screen[screenIndex].green |= (0b01 * (picture[index].green >= (s+1)));
          }
          else{
            screen[screenIndex - 1024].red   |= (0b10 * (picture[index].red   >= (s+1)));
            screen[screenIndex - 1024].blue  |= (0b10 * (picture[index].blue  >= (s+1)));
            screen[screenIndex - 1024].green |= (0b10 * (picture[index].green >= (s+1)));
          }

       }
    }
  }
}


//takes 64x32 pixel4 array and translates it onto a 64x32x3 pixel array
void printScreen(pixel4* colorScreen, pixel*screen, pixel* screen2) {
  //i is row, j is column
  int index = 0;
  pixel* curBuff = screen;

//  if(bufSel){
//    curBuff = screen;
//    bufSel = 0;
//  }
//  else{
//    curBuff = screen2;
//    bufSel = 1;
//  }
  //DMA1_Channel1->CCR &= ~DMA_CCR_EN;
  //loop for each color plane
  for(int s = 0; s < 1; s++){
    //loop for each column in the screen struct
    for(int i = 0; i < 16; i++){
      //loop for each row in the screen struct
      for(int j = 0; j < 64; j++){
        //find index into screen array
        index = (i * 64) + j;
        //              layer offset                          index into 64x32                      index of halfway + current index
        curBuff[index + (s * 1024)].red   = (0b01 * (colorScreen[index].red   >= (s+1))) + (0b10 * (colorScreen[index + (1024)].red   >= (s+1)));
        curBuff[index + (s * 1024)].blue  = (0b01 * (colorScreen[index].blue  >= (s+1))) + (0b10 * (colorScreen[index + (1024)].blue  >= (s+1)));
        curBuff[index + (s * 1024)].green = (0b01 * (colorScreen[index].green >= (s+1))) + (0b10 * (colorScreen[index + (1024)].green >= (s+1)));
       }
    }
  }


  //DMA1_Channel1->CMAR = (uint32_t) curBuff;
  //DMA1_Channel1->CCR |= DMA_CCR_EN;

  return;
}
//takes a 64x32 pixel4 array and draws a 2d pixel4 shape of dimensions sizex*sizey on the screen at (locx,locy)
void drawShape(pixel4* screen, pixel4* shape, int sizex, int sizey, int locx, int locy){
  int screenIndex = 0;
  int shapeIndex = 0;
  for(int i = 0; i < sizex; i++) {
    for(int j = 0; j < sizey; j++) {

      shapeIndex = (i * sizey) + j;

      screenIndex = ((i+locx) * 64) + (j+locy);

      if(0<screenIndex<2048){
        screen[screenIndex].red = shape[shapeIndex].red;
        screen[screenIndex].green = shape[shapeIndex].green;
        screen[screenIndex].blue = shape[shapeIndex].blue;
      }
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
  DMA1_Channel1->CNDTR = 64 * 16 * 1;
  DMA1_Channel1->CMAR = (uint32_t) addr;
  DMA1_Channel1->CPAR = (uint32_t) (&(GPIOB->ODR));

  //set memory access size to 32 bits
  //DMA1_Channel1->CCR |= DMA_CCR_MSIZE_1;
  DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;

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
void setupDMA2(void* addr, void* addr2) {
  RCC->AHBENR |= RCC_AHBENR_DMAEN;

  //Set data count for 64 columns by 16 rows each
  DMA1_Channel2->CNDTR = 64 * 16 * 3;
  DMA1_Channel2->CMAR = (uint32_t) addr;
  DMA1_Channel2->CPAR = (uint32_t) addr2;

  //mem2mem mode
  DMA1_Channel2->CCR |= DMA_CCR_MEM2MEM;

  //high priority
  DMA1_Channel2->CCR |= DMA_CCR_PL_0 | DMA_CCR_PL_1;

  //set memory access size to 32 bits
  DMA1_Channel2->CCR |= DMA_CCR_MSIZE_1;

  //set peripheral access size to 32 bits
  DMA1_Channel2->CCR |= DMA_CCR_PSIZE_1;

  //set minc to 1 and pinc to 0
  DMA1_Channel2->CCR |= DMA_CCR_PINC;
  DMA1_Channel2->CCR |= DMA_CCR_MINC;

  //set to circular
  DMA1_Channel2->CCR |= DMA_CCR_CIRC;
  //set to mem to peripheral direction
  DMA1_Channel2->CCR |= DMA_CCR_DIR;

  //TIM2 is the default mapping for DMA1 Channel1
  //DMA1->RMPCR |= DMA_RMPCR1_CH3_TIM2;

  //enable the channel
  DMA1_Channel2->CCR |= DMA_CCR_EN;
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



int main(void)
{
//  pixel* screen = malloc(sizeof(pixel) * 16 * 64 * 3);
//  pixel4* screen4 = malloc(sizeof(pixel4) * 32 * 64);



  //int freq = 117;
  int scaler = 1;
  //0x03E8/scaler
  setupTIM2(10-1, (2 * scaler) - 1, (2 * scaler) / 2);


  int row = 16;
  int column = 64;

int index = 0;
//initialize screen array. s is the colorplane index, i is the column index and j is the row index.
//Set
for(int s = 0; s < 3; s++){
  for(int i = 0; i < row; i++) {
    for(int j = 0; j < column; j++){
      index = (i * column) + j + (s * 1024);
      //set all colors to 0
      screen[index].red = 0;
      screen[index].green = 0;
      screen[index].blue = 0;
      //set latch values
      if(j == 63) {
        screen[index].latch = 1;
      }
      else{
        screen[index].latch = 0;
      }
      //setup row indexes
      screen[index].row = (i-1) % 15;
    }
  }
}
//
//for(int s = 0; s < 3; s++){
//  for(int i = 0; i < row; i++) {
//    for(int j = 0; j < column; j++){
//      index = (i * column) + j + (s * 1024);
//      //set all colors to 0
//      screen2[index].red = 0;
//      screen2[index].green = 0;
//      screen2[index].blue = 0;
//      //set latch values
//      if(j == 63) {
//        screen2[index].latch = 1;
//      }
//      else{
//        screen2[index].latch = 0;
//      }
//      //setup row indexes
//      screen2[index].row = (i-1) % 63;
//    }
//  }
//}
TIM2->EGR |= TIM_EGR_UG;
TIM2->CR1 |= TIM_CR1_CEN;
size_t packedSize = sizeof(pixel4);

setupDMA(screen);
for(int i = 0; i < 32; i++) {
  for(int j = 0; j < 64; j++){
    index = (i*64) + j;
    screen4[index].red = 0;
    screen4[index].green = 0;
    screen4[index].blue = 0;
  }
}
int size = 5;
pixel4* cube = malloc(sizeof(pixel4) * pow(size,2));
for(int i = 0; i < pow(size,2); i++){
  cube[i].red = 3;
  cube[i].blue = 3;
  cube[i].green = 3;

}

sprite cube2;
cube2.image = cube;
cube2.xSize = size;
cube2.ySize = size;

int index4 = 0;
int posx = 10;
int posy = -1;
int velox = 1;
int veloy = 1;
int newposx = 0;
int newposy = 0;
int rgb = 0;
int yVelDir = -1;

while(1){
  cube2.curPosx=posx;
  cube2.curPosx=posy;
//  for(int rgb = 0; rgb < 64; rgb++){
//    for(int i = 0; i < 32; i++) {
//      for(int j = 0; j < 64; j++) {
//        index4 = (64 * i) + j;
//        screen4[index4].red =   sineLookupTable[rgb];
//        screen4[index4].blue =  sineLookupTable[(rgb + 21) % 63];
//        screen4[index4].green = sineLookupTable[(rgb + 43) % 63];



//  if(velox < 1 && yVelDir == -1){
//    yVelDir = 1;
//  }
//  else if(velox > 3 && yVelDir == 1){
//    yVelDir = -1;
//  }
//  velox = velox + yVelDir;
        for(int i = 0; i < pow(size,2); i++){
          cube[i].red = sineLookupTable[rgb];
          cube[i].blue = sineLookupTable[(rgb + 21) % 63];
          cube[i].green = sineLookupTable[(rgb + 43) % 63];
        }
        rgb = (rgb + 1)%64;
//      }
//    }
//  }
   //nanoWait(100);
   newposx = velox + posx;
   newposy = veloy + posy;

  if((newposx+size-1) > 32 || newposx < 0){
    velox = -velox;
    newposx = velox + posx;
  }

  if((newposy+size-1) > 64 || newposy< 0){
    veloy = -veloy;
    newposy = veloy + posy;
  }

  for(int i = 0; i < 32; i++) {
    for(int j = 0; j < 64; j++){
      index = (i*64) + j;
      screen4[index].red = 0;
      screen4[index].green = 0;
      screen4[index].blue = 0;
    }
  }

  cube2.nextPosx=newposx;
  cube2.nextPosy=newposy;
  posx = newposx;
  posy = newposy;
  drawShape(screen4, cube, size,size, posx,posy);

//  for(int i = 0; i < 2048; i++){
//    screen4[i].red = 3;
//    screen4[i].green = 3;
//    screen4[i].blue = 3;
//  }



  printScreen(screen4, screen, screen2);
  //fastPrintScreen(&cube2, screen);
}




}
