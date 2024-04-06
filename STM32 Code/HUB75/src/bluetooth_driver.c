/**
 * @brief Includes coinfiguration of peripherals and drivers
 *        that are mainly written for the bluebooth.
 * @author Vishnu Lagudu (vlagudu@purdue.edu)
 *         Alex Chitsazzadeh (achitsaz@purdue.edu)
 *         Aditya Sood (sood12@purdue.edu)
**/

char MOVE;

#include "bluetooth_driver.h"

/* Coinfigure the peripherals required for UART transmission */

/* Bluetooth Drivers */

// Factory reset module (CAREFUL)
void sendATRenew()
{
	  // set name of bluetooth device
	  char* nameTx = "AT+RENEW";
	  char nameRx[8]  = {};

	  // send "AT" test command
	  for(uint32_t i = 0; i < 8; i++)
	  {
		  while(!(USART4->ISR & USART_ISR_TXE)){}
		  USART4 -> TDR = nameTx[i];
	  }

	  for(int i = 0; i < 8; i++)
	  {
		  while (!(USART4->ISR & USART_ISR_RXNE)) {}
		  nameRx[i] = USART4->RDR;
	  }
}

/** Basic AT check
  	get AT+OK if device ready to go
**/
void sendATCheck()
{
	  uint8_t testTX[2] = "AT";
	  uint8_t testRX[2];

	  // send "AT" test command
	  for(uint32_t i = 0; i < 2; i++){
		  while(!(USART4->ISR & USART_ISR_TXE)){}
		  USART4 -> TDR = testTX[i];
	  }

	  // expect "OK" from Bx
	  for(int i = 0; i < 2; i++)
	  {
		  while (!(USART4->ISR & USART_ISR_RXNE)) {}
		  testRX[i] = USART4->RDR;
	  }
}

// retrieve MAC address of bluetooth module
void getATAddr()
{
	  uint8_t testAddrTx[8] = {'A','T','+','A','D','D','R','?'};
	  uint8_t testAddrRx[20] = {};

	  // send "AT+Addr?" command
	  for(uint32_t i = 0; i < 8; i++){
		  while(!(USART4->ISR & USART_ISR_TXE)){}
		  USART4 -> TDR = testAddrTx[i];
	  }

//	   expect "OK+Mac Addr" from Bx
	  for(int i = 0; i < 20; i++)
	  {
		  while (!(USART4->ISR & USART_ISR_RXNE)) {}
		  testAddrRx[i] = USART4->RDR;
	  }
}

// change name of bluetooth module (preserved across power outs)
void setBxName()
{
	  // set name of bluetooth device
	  char* nameTx = "AT+NAMEHub";
	  char nameRx[10]  = {};

	  // send "AT" test command
	  for(uint32_t i = 0; nameTx[i] != '\0'; i++)
	  {
		  while(!(USART4->ISR & USART_ISR_TXE)){}
		  USART4 -> TDR = nameTx[i];
	  }

	   for(int i = 0; i < 10; i++)
	   {
		   while (!(USART4->ISR & USART_ISR_RXNE)) {}
		   nameRx[i] = USART4->RDR;
	   }
}

/** Configure device in master or slave mode
 	 AT+ROLE? -> ask current role
 	 AT+ROLE1 -> set to master
 	 AT+ROLE0 -> set to slave
**/
void setATRole()
{
	char roleTx[8] = "AT+ROLE0";
	char roleRx[8] = {};

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART4->ISR & USART_ISR_TXE)){}
		USART4 -> TDR = roleTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART4->ISR & USART_ISR_RXNE)){}
		roleRx[i] = USART4 -> RDR;
	}
}

// Soft reset module (usually after changing config settings)
void sendATReset()
{
	  // set name of bluetooth device
	  char* nameTx = "AT+RESET";
	  char nameRx[8]  = {};

	  // send "AT" test command
	  for(uint32_t i = 0; i < 8; i++)
	  {
		  while(!(USART4->ISR & USART_ISR_TXE)){}
		  USART4 -> TDR = nameTx[i];
	  }

	  for(int i = 0; i < 8; i++)
	  {
		  while (!(USART4->ISR & USART_ISR_RXNE)) {}
		  nameRx[i] = USART4->RDR;
	  }
}

void ss_bluetooth()
{
  init_io_bt();
  setup_uart_bt();
//  sendATRenew();
//	sendATCheck();
	getATAddr();
//	setBxName();
//	setATRole();
//	sendATReset();
}

/* Bluetooth Peripherals */

void
init_io_bt ()
{
  RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;                
	GPIOA -> MODER |= GPIO_MODER_MODER0_1; 	
  GPIOA -> MODER |= GPIO_MODER_MODER1_1;
  GPIOA -> AFR[0] = 0; 				
	GPIOA -> AFR[0] |= (0x4);								 	
	GPIOA -> AFR[0] |= (0x4 << 4); 	  
}

void 
setup_uart_bt ()
{
  RCC -> APB1ENR |= RCC_APB1ENR_USART4EN;   
  // Set the control register reset state             
	USART4 -> CR1 = 0;                                            
	USART4 -> BRR = 48000000 / 9600;					  
	USART4 -> CR1 |= USART_CR1_TE;				           
	USART4 -> CR1 |= USART_CR1_RE;				           
	USART4 -> CR1 |= USART_CR1_UE;   
	USART4 -> CR1 |= USART_CR1_RXNEIE;
	USART4 -> CR3 |= USART_CR3_DMAR;               
  // wait for TX and RX to be ready
	while(!((USART4 -> ISR) & (USART_ISR_TEACK | USART_ISR_REACK)));
}

// need to fix this
void
init_dma_bt ()
{
  DMA1_Channel6 -> CCR &= ~DMA_CCR_EN;
	DMA1_Channel6 -> CMAR = &MOVE;
	DMA1 -> RMPCR |= DMA_RMPCR2_CH2_USART4_RX;
	DMA1_Channel6 -> CPAR = (uint32_t) &(USART4->RDR);
	DMA1_Channel6 -> CNDTR = sizeof (char);
	DMA1_Channel6 -> CCR |= DMA_CCR_PL;
}

void
enable_dma_bt ()
{
	DMA1_Channel6 -> CCR |= DMA_CCR_EN;
}
