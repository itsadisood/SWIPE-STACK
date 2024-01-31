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



void wait(void){
  for(int i = 0; i < 10000000; i++){}
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
  sprintf(output, "GPIO Pin %s value is %d\r\n", pin, pinBool);
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

void EXTI4_15_IRQHandler(void){
  int zero = 0;
  //GPIOA->BSRR |= GPIO_BSRR_BR_11;
  //SPI1->CR1 &= !SPI_CR1_SSI;
    //if transmit buffer is empty, set next value
    if(SPI1->SR & SPI_SR_TXE) {
      SPI1->DR = (char)zero;
    }
    while(!(SPI1->SR & SPI_SR_TXE)){};
    //GPIOA->BSRR |= GPIO_BSRR_BS_11;
    //SPI1->CR1 |= SPI_CR1_SSI;
    //if receive buffer is not empty, send recieved data over UART
    if(SPI1->SR & SPI_SR_RXNE){
      //debugSend((char)(SPI1->DR));
    }
   //reset interrupt flag
   EXTI->PR |= EXTI_PR_PR10;
}
void SPI1_IRQHANDLER(void){
  debugSendString("SPI Data Received");

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
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI1->CR1 |= !SPI_CR1_SPE;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

    GPIOA->MODER |= GPIO_MODER_MODER4_1;
    GPIOA->MODER |= GPIO_MODER_MODER5_1;
    GPIOA->MODER |= GPIO_MODER_MODER6_1;
    GPIOA->MODER |= GPIO_MODER_MODER7_1;


    //Set up the reset and wake pins
    //GPIOA->MODER |= GPIO_MODER_MODER4_0;
    GPIOA->MODER |= GPIO_MODER_MODER11_0;
    GPIOA->MODER |= GPIO_MODER_MODER12_0;

    GPIOA->BSRR |= GPIO_BSRR_BR_12;

    //(set wake high)
    GPIOA->BSRR |= GPIO_BSRR_BS_11;
    //set nss high
    //GPIOA->BSRR |= GPIO_BSRR_BS_4;



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
    //
    SPI1->CR2 &= !SPI_CR2_DS_3;
    SPI1->CR2 |= SPI_CR2_NSSP;

    //configure GPIO interrupt through EXTI
    SYSCFG->EXTICR[2] |= SYSCFG_EXTICR3_EXTI10_PA;
    EXTI->IMR |= EXTI_IMR_MR10;
    EXTI->FTSR |= EXTI_FTSR_TR10;
    NVIC->ISER[0] |= 1 << 7;


    //enable spi receive register not empty interrupt
    //SPI1->CR2 |= SPI_CR2_RXNEIE;

    SPI1->CR1 |= SPI_CR1_SPE;

    //toggle the reset off (high)
    GPIOA->BSRR |= GPIO_BSRR_BS_12;
}
int main(void)
{
  char* myString = "Hello World!---------------------------------\r\n";
  int iterator = 0;

  setup_serial();
  setup_SPI1();
  printPin(SPI1->CR2 & SPI_CR2_DS_0, "DR 0");
  printPin(SPI1->CR2 & SPI_CR2_DS_1, "DR 1");
  printPin(SPI1->CR2 & SPI_CR2_DS_2, "DR 2");
  printPin(SPI1->CR2 & SPI_CR2_DS_3, "DR 3");

  while(iterator < 12)
  {
//    if(USART5->ISR & USART_ISR_TXE) {
      //USART5->TDR = myString[iterator];
      debugSend(myString[iterator]);
      iterator++;
    }
//  }
  //toggle wake on (low)
  //GPIOA->BSRR |= GPIO_BSRR_BR_11;
  while(/*(GPIOA->IDR & GPIO_IDR_10)*/1 == 1){
    debugSendString("Awaiting HOST_INTN\r\n");
    printPin((GPIOA->IDR&GPIO_IDR_10), "PIN Interrupt");
    printPin((GPIOA->ODR&GPIO_ODR_11), "PIN WAKE");
    printPin((GPIOA->ODR&GPIO_ODR_12), "PIN RESET");
    wait();

  }
  GPIOA->BSRR |= GPIO_BSRR_BR_11;
  wait();
  GPIOA->BSRR |= GPIO_BSRR_BS_11;


  //char spiString[3] = {(char)0, (char)0, '\0'};

  //debugSendString(spiString);
  ///spiSend(spiString);


}
