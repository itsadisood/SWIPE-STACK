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

typedef struct headerInfo{
  uint16_t cargoSize;
  uint8_t channel;
  uint8_t seqNum;
}headerInfo;

unsigned char reverse(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void wait(int size ){
  for(int i = 0; i < size * 10000; i++){
  }
}

void nanoWait(int size){
  for(int i = 0; i < 225 * size; i++){}
}

int getFormatSize(char* string, ...){
  va_list argptr;
  va_start(argptr,string);
  ssize_t bufsz = snprintf(NULL, 0, string, argptr);
  va_end(argptr);
  return bufsz;
};

void debugSend(char txdata) {
  while(!(USART5->ISR & USART_ISR_TXE)){}
      USART5->TDR = txdata;
}

void debugSendString(char* data) {
  int i = 0;
  while(data[i] != '\0') {
    debugSend(data[i]);
    i++;
  }
}
void printPin(int pinVal, char* pin) {
  char* output = malloc(sizeof(char)*30);
  int pinBool = 1 && pinVal;
  sprintf(output, "%s value is %d\r\n", pin, pinBool);
  debugSendString(output);
  free(output);
}

void printIntArr(uint8_t* arr, int size) {
  debugSendString("[");
  for(int i = 0; i < size; i++){
    char* intStr = malloc(sizeof(char) * (getFormatSize("%u", arr[i]) + 3));
    sprintf(intStr, " %u,", arr[i]);
    debugSendString(intStr);
    free(intStr);
    if((i+1)%11 == 0){
      debugSendString("\r\n");
    }
  }
  debugSendString("]");
}
void printInt(int val, char* pin) {
  char* output = malloc(sizeof(char)*30);
  sprintf(output, "%s value is %d\r\n", pin, val);
  debugSendString(output);
  free(output);
}
void uprintf(char* string, ...){
  va_list argptr;
  va_start(argptr,string);
  va_list argptr2;
  va_copy(argptr2, argptr);

  printInt(va_arg(argptr2, int), "Value 1");
  printInt(va_arg(argptr2, int), "Value 2");

  //get size of string
  ssize_t bufsz = getFormatSize(string, argptr);
  char* output = malloc(sizeof(char) * (bufsz + 1));
  snprintf(output, bufsz + 1, string, argptr2);
  va_end(argptr);
  va_end(argptr2);

  debugSendString(output);
  free(output);
}

//void TXEIE_IRQHANDLER(void){
//  //toggle the chip select back up
//  GPIOA->BSRR |= GPIO_BSRR_BS_3;
//}

void EXTI4_15_IRQHandler(void){
  int data = 0;
  int sends = 0;
  int reads = 0;
  uint8_t temp = 0;
  headerInfo header = {.cargoSize = 0, .channel = 0, .seqNum = 0};

  GPIOA->BSRR |= GPIO_BSRR_BR_3;
  //read header
  int cargo[4];
  //transmit and receive 24 bytes
  while(sends < 4 || reads < 4){
    if((SPI1->SR & SPI_SR_TXE) && !(SPI1->SR & SPI_SR_RXNE)){
      *(uint8_t*)&SPI1->DR = (uint8_t)0;
      sends++;
    }
    while(reads < sends){
      if(SPI1->SR & SPI_SR_RXNE){
        cargo[reads] = *(uint8_t*)&SPI1->DR;
        reads++;
       }
    }
  }


  if(cargo[3] == 0){
    header.cargoSize = cargo[0] + (cargo[1]<<8);
    printInt(header.cargoSize, "cargo size");
  }
  else {
    header.cargoSize = cargo[0];
    printInt(cargo[0], "cargo size");
  }

  printInt(cargo[0], "Header Info 1");
  printInt(cargo[1], "Header Info 2");
  printInt(cargo[2], "Header Info 3");
  printInt(cargo[3], "seq num");

  uint8_t* fullCargo = malloc(sizeof(uint8_t) * header.cargoSize - 4);

  while(sends < header.cargoSize || reads < header.cargoSize){
    if((SPI1->SR & SPI_SR_TXE) && !(SPI1->SR & SPI_SR_RXNE)){
      *(uint8_t*)&SPI1->DR = (uint8_t)sends;
      sends++;
    }
    while(reads < sends){
      if(SPI1->SR & SPI_SR_RXNE){
        fullCargo[reads - 4] = *(uint8_t*)&SPI1->DR;
        reads++;
       }
    }
  }
  GPIOA->BSRR |= GPIO_BSRR_BS_3;

  printIntArr(fullCargo, header.cargoSize);
  //parse the cargo
  int i = 1;
  while(i < header.cargoSize) {
    uint8_t tag = fullCargo[i];
    uint8_t length = fullCargo[i + 1];

    printInt(tag, "tag");
    printInt(length, "length");

    if(length == 0){
      debugSendString("Borked");
      break;
    }

    uint8_t* value = malloc(sizeof(uint8_t) * length);
    for(int j = 0; j < length; j++) {
      value[j] = fullCargo[i+j+2];
      printInt(value[j], "Val");
    }

    switch (tag) {
      case 0:
        debugSendString("Reserved?");
        break;
      case 1:
        debugSendString("GUID: ");
        for(int j = 0; j < length; j++) {
          debugSend((char)value[j]);
         }
        break;
      case 2:
        debugSendString("Reserved?");
      break;
        // code block
    }
    free(value);
    i = i + 2 + length;
  }


//
//  //now that we know how big the cargo is, make an array to hold the cargo
//  uint8_t* cargoArray = malloc(sizeof(uint8_t) * header.cargoSize);

//  while(sends < header.cargoSize || reads < header.cargoSize){
//    if((SPI1->SR & SPI_SR_TXE) && !(SPI1->SR & SPI_SR_RXNE)){
//      *(uint8_t*)&SPI1->DR = 0;
//      sends++;
//    }
//    if(SPI1->SR & SPI_SR_RXNE){
//      cargoArray[reads] = *(uint8_t*)&SPI1->DR;
//      uprintf("Reading cargo %u of %u\r\n", reads, header.cargoSize);
//      printInt(cargoArray[reads], "Cargo Info");
//      reads++;
//     }
//  }
  //print out TLVs
//  for(int i = 0; i < header.cargoSize - 4; ){
//    printInt(cargoArray[i],"Tag");
//    printInt(cargoArray[i+1], "Length");
//    for(int j = 0; j < cargoArray[i + 1]; j++){
//      printInt(cargoArray[i+j], "Value");
//    }
//    i = i + cargoArray[i+1];
//  }


  wait(100000);
  //free(cargoArray);
  EXTI->PR |= EXTI_PR_PR10;
  return;
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

void setup_SPI1(void) {
  //enable clock for SPI and GPIOA
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    //clear SPE bit before configuring
    SPI1->CR1 &= ~SPI_CR1_SPE;

    //configure as master/main
    SPI1->CR1 |= SPI_CR1_MSTR;

    //configure appropriate clock mode for the BNo085
    SPI1->CR1 |= SPI_CR1_CPOL;
    SPI1->CR1 |= SPI_CR1_CPHA;

    SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2; //set baud rate low
    SPI1->CR2 |= SPI_CR2_SSOE;
    //SPI1->CR2 &= ~SPI_CR2_DS_3; //Set data size to 16 bits

    SPI1->CR2 |= SPI_CR2_FRXTH; //set RXNE level for 8 bit reads


    SPI1->CR1 |= SPI_CR1_SPE;

//    NVIC->ISER[0] |= 1 << 7;

}
void setup_GPIO(void) {
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;

  //configure GPIOA pins for alternative function for SPI
  GPIOA->MODER |= GPIO_MODER_MODER4_1;
  GPIOA->MODER |= GPIO_MODER_MODER5_1;
  GPIOA->MODER |= GPIO_MODER_MODER6_1;
  GPIOA->MODER |= GPIO_MODER_MODER7_1;


  //Set up the reset, wake, manual nss and sync pins
  GPIOA->MODER |= GPIO_MODER_MODER2_0;
  GPIOA->MODER |= GPIO_MODER_MODER3_0;
  GPIOA->MODER |= GPIO_MODER_MODER11_0;
  GPIOA->MODER |= GPIO_MODER_MODER12_0;

  //Reset the RST pin value
  GPIOA->BSRR |= GPIO_BSRR_BR_12;

  //(set wake high)
  GPIOA->BSRR |= GPIO_BSRR_BS_11;
  //set nss, sync and reset high
  GPIOA->BSRR |= GPIO_BSRR_BS_3;
  GPIOA->BSRR |= GPIO_BSRR_BS_2;
  GPIOA->BSRR |= GPIO_BSRR_BS_12;

  //SPI is the 0 (default) alternate function for GPIOA, so setting the AFR is not needed

}
void setup_interrupt(void) {
  //configure GPIO interrupt through EXTI
  SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PA;
  EXTI->FTSR |= EXTI_FTSR_TR10;
  EXTI->IMR |= EXTI_IMR_MR10;
  NVIC->ISER[0] |= 1 << 7;
}
int main(void)
{
  char* myString = "Hello World!---------------------------------\r\n";
  int iterator = 0;
  int loopCount = 0;

  setup_serial();
  setup_SPI1();
  setup_GPIO();
  setup_interrupt();
  printPin(SPI1->CR2 & SPI_CR2_DS_0, "DR 0");
  printPin(SPI1->CR2 & SPI_CR2_DS_1, "DR 1");
  printPin(SPI1->CR2 & SPI_CR2_DS_2, "DR 2");
  printPin(SPI1->CR2 & SPI_CR2_DS_3, "DR 3");



  while(myString[iterator] != '\0')
  {
//    if(USART5->ISR & USART_ISR_TXE) {
      //USART5->TDR = myString[iterator];
      debugSend(myString[iterator]);
      iterator++;
    }
//  }

  wait(100);


  while(/*(GPIOA->IDR & GPIO_IDR_10)*/1 == 1){
//    debugSendString("Awaiting HOST_INTN\r\n");
//    printPin((GPIOA->IDR&GPIO_IDR_10), "PIN Interrupt");
//    printPin((GPIOA->ODR&GPIO_ODR_11), "PIN WAKE");
//    printPin((GPIOA->ODR&GPIO_ODR_12), "PIN RESET");
//    printPin(SPI1->SR & SPI_SR_TXE, "TX EMPTY");
//    printPin(SPI1->SR & SPI_SR_RXNE, "RX NOT EMPTY");
//    printInt(loopCount, "Iteration");
//    printInt(SPI1->CR1 & SPI_CR1_SPE, "SPE");

    wait(100);
    loopCount = loopCount + 1;

  }

}
