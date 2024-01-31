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
int firstSPI = 0;
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
void printInt(int val, char* pin) {
  char* output = malloc(sizeof(char)*30);
  sprintf(output, "%s value is %d\r\n", pin, val);
  debugSendString(output);
  free(output);
}

void spiSend(char* txdata) {
  int i = 0;
  //while not null terminator
  while (txdata[i] != '\0'){
    //if transmit buffer is empty, set next value and increment iterator
    if(SPI1->SR & SPI_SR_TXE) {
      SPI1->DR = txdata[i];
      i++;
    }
    //if receive buffer is not empty, send recieved data over UART
    if(SPI1->SR & SPI_SR_RXNE){
      debugSend((char)(SPI1->DR));
    }
  }
}

//void TXEIE_IRQHANDLER(void){
//  //toggle the chipselect back up
//  GPIOA->BSRR |= GPIO_BSRR_BS_3;
//}

void EXTI4_15_IRQHandler(void){
  SPI1->CR1 |= SPI_CR1_SPE;
  if(!firstSPI){
    int data = 0;
    int sends = 0;
    int reads = 0;
    GPIOA->BSRR |= GPIO_BSRR_BR_3;
    //read header
    int header[4];
    //transmit and receive 24 bytes
    while(sends < 24 || reads < 24){
      if((SPI1->SR & SPI_SR_TXE) && !(SPI1->SR & SPI_SR_RXNE)){
        *(uint8_t*)&SPI1->DR = sends;
        sends++;
      }
      if(SPI1->SR & SPI_SR_RXNE){
        if(reads<4){
          header[reads] = *(uint8_t*)&SPI1->DR;
          printInt(header[reads], "Header Info");
        }
        else{
          printInt(*(uint8_t*)&SPI1->DR, "Cargo Info");
        }
        reads++;
      }
    }
    firstSPI = 1;
    GPIOA->BSRR |= GPIO_BSRR_BS_3;

  }

  SPI1->CR1 &= ~SPI_CR1_SPE;
  EXTI->PR |= EXTI_PR_PR10;
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
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    //clear SPE bit before configuring
    SPI1->CR1 &= ~SPI_CR1_SPE;

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
    //set nss and sync high
    GPIOA->BSRR |= GPIO_BSRR_BS_3;
    GPIOA->BSRR |= GPIO_BSRR_BS_2;



    //SPI is the 0 (default) alternate function for GPIOA, so setting the AFR is not needed

    //configure appropriate clock mode for the BNo085
    SPI1->CR1 |= SPI_CR1_CPOL;
    SPI1->CR1 |= SPI_CR1_CPHA;
    //configure as master/main
    SPI1->CR1 |= SPI_CR1_MSTR;
    //set baud rate low
    SPI1->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;
    //SPI1->CR1 |= SPI_CR1_SSM;
    //SPI1->CR1 |= SPI_CR1_SSI;
    SPI1->CR2 |= SPI_CR2_SSOE;

    //Set data size to 16 bits
    //SPI1->CR2 &= ~SPI_CR2_DS_3;
    SPI1->CR2 |= SPI_CR2_NSSP;

    //set RXNE for 8 bit reads
    SPI1->CR2 |= SPI_CR2_FRXTH;


    GPIOA->BSRR |= GPIO_BSRR_BS_12;

    //configure GPIO interrupt through EXTI
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PA;
    EXTI->IMR |= EXTI_IMR_MR10;
    EXTI->RTSR |= EXTI_RTSR_TR10;
    NVIC->ISER[0] |= 1 << 7;


    //enable spi receive register not empty interrupt
    //SPI1->CR2 |= SPI_CR2_TXEIE;



    //toggle the reset off (high)

}
int main(void)
{
  char* myString = "Hello World!---------------------------------\r\n";
  int iterator = 0;
  int loopCount = 0;

  setup_serial();
  setup_SPI1();
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
  //reset
  GPIOA->BSRR |= GPIO_BSRR_BR_2;

  wait(100);


  //char spiString[3] = {(char)0, (char)0, '\0'};

  //debugSendString(spiString);
  ///spiSend(spiString);


}
