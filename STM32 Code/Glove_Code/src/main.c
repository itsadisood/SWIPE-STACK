/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

/** Pin definitions:

  PB3 -> UART5_TX -> B_RX
 B_TX -> UART5_RX -> PB4

    PB5 -> UART1_TX -> IMU_RX (not used)
 IMU_TX -> UART1_RX -> PB6

**/

#include "stm32f0xx.h"
#include <string.h>

// asm function to waste
// CPU clocks
void
nano_wait(unsigned int n)
{
  asm("        mov r0,%0\n"
      "repeat: sub r0,#83\n"
      "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

// *****************************************
// BLUETOOTH CODE **************************
// *****************************************

// if configured as slave and IMME1, make yourself discoverable
void sendATStart()
{
	char *startTx = "AT+START";
	char startRx[8] = {};
	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = startTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_RXNE)){}
		startRx[i] = USART5 -> RDR;
	}
}

// if configured as master, start discovery of slave peripherals
void sendATDisc()
{
	char *discTx = "AT+DISC?";
	char discRx[8] = {};
	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = discTx[i];
	}

	for(uint32_t i = 0; i < 100; i++)
	{
		while(!(USART5->ISR & USART_ISR_RXNE)){}
		discRx[i] = USART5 -> RDR;
	}
}

void sendATConn()
{
	char * conTx = "AT+CONE4E11295FAB6";
	char   conRx[8] = {};

	for(uint32_t i = 0; conTx[i] != '\0'; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = conTx[i];
	}

}

/** Configure device in master or slave mode
 	 AT+ROLE? -> ask current role
 	 AT+ROLE1 -> set to master
 	 AT+ROLE0 -> set to slave
**/
void setATRole()
{
	char roleTx[8] = "AT+ROLE1"; // SET TO 1
	char roleRx[8] = {};

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = roleTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_RXNE)){}
		roleRx[i] = USART5 -> RDR;
	}
}

/** Configure how the bluetooth module startups
 	 AT+IMME1 -> if slave, wait for AT+START to advertise
			  -> if master, wait for AT+DISC to discover
 	 AT+IMME0 -> start working from get go.
**/
void setATImme()
{

	char immeTx[8] = "AT+IMME1"; // SET TO 1
	char immeRx[8] = {};

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = immeTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_RXNE)){}
		immeRx[i] = USART5 -> RDR;
	}
}

// change name of bluetooth module (preserved across power outs)
void setBxName()
{
	  // set name of bluetooth device
	  char* nameTx = "AT+NAMEGloveR";
	  char nameRx[14]  = {};

	  // send "AT" test command
	  for(uint32_t i = 0; nameTx[i] != '\0'; i++)
	  {
		  while(!(USART5->ISR & USART_ISR_TXE)){}
		  USART5 -> TDR = nameTx[i];
	  }

	  for(int i = 0; i < 14; i++)
	  {
		  while (!(USART5->ISR & USART_ISR_RXNE)) {}
		  nameRx[i] = USART5->RDR;
	  }
}

// Factory reset module (CAREFUL)
void sendATRenew()
{
	  // set name of bluetooth device
	  char* nameTx = "AT+RENEW";
	  char nameRx[8]  = {};

	  // send "AT" test command
	  for(uint32_t i = 0; i < 8; i++)
	  {
		  while(!(USART5->ISR & USART_ISR_TXE)){}
		  USART5 -> TDR = nameTx[i];
	  }

	  for(int i = 0; i < 8; i++)
	  {
		  while (!(USART5->ISR & USART_ISR_RXNE)) {}
		  nameRx[i] = USART5->RDR;
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
		  while(!(USART5->ISR & USART_ISR_TXE)){}
		  USART5 -> TDR = nameTx[i];
	  }
}

// Directly connect to a specified device through MAC Address
void sendATCon()
{
	char * conTx = "AT+CONE4E11295FAB6";
	char   conRx[8] = {};

	for(uint32_t i = 0; conTx[i] != '\0'; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = conTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_RXNE)){}
		conRx[i] = USART5 -> RDR;
	}
}

void setup_BX_GPIO()
{
	RCC -> AHBENR |= RCC_AHBENR_GPIOBEN;                    // Enable GPIO clock
	GPIOB -> MODER |= GPIO_MODER_MODER3_1; 					// PB3 is TX to Bluetooth (Alt Mode)
	GPIOB -> AFR[0] |= 0x4 << 12;							// PB3 selected for USART5_TX
	GPIOB -> MODER |= GPIO_MODER_MODER4_1; 					// PB4 is RX from Bluetooth (Alt Mode)
	GPIOB -> AFR[0] |= 0x4 << 16; 							// PB4 selected for USART5_RX
}

void setup_BX_UART()
{
	RCC -> APB1ENR |= RCC_APB1ENR_USART5EN;                // clock on for USART5
	USART5 -> CR1 &= ~USART_CR1_UE;                        // turn off for config
	USART5 -> CR1 &= ~USART_CR1_M;                         // 8 bit word length
	USART5 -> CR2 &= ~USART_CR2_STOP;                      // 1 stop bits
	USART5 -> CR1 &= ~USART_CR1_PCE;                       // parity disable
	USART5 -> CR1 &= ~USART_CR1_OVER8;                     // 16x oversampling
	USART5 -> BRR = 48000000 / 9600;					   // 9600 baud rate
	USART5 -> CR1 |= USART_CR1_TE;				           // transmitter enable
	USART5 -> CR1 |= USART_CR1_RE;				           // receiver enable
	USART5 -> CR1 |= USART_CR1_UE;                         // enable usart

	while(!(((USART5 -> ISR) & USART_ISR_TEACK) && ((USART5 -> ISR) & USART_ISR_REACK))); // wait for TEACK and REACK to be set
}

void BluetoothSetup()
{
//	sendATRenew();
//	setBxName();
//	setAdvInterval();
//	setATRole();
//	setATImme();
//	sendATDisc();
//	sendATCon();
//	sendATReset();
//	sendATStart();
//	sendATDisc();
	sendATConn(); // CALL TO PAIR

}

// *****************************************
// IMU CODE ********************************
// *****************************************

// sizing constants
#define FIFOSIZE 					19								// # of bytes to receive from IMU into temp buffer before processing
#define PREDICTIONS_PER_SECOND 	5 								// # of times the entire toolchain runs to display a output
#define PACKET_SIZE				100 / PREDICTIONS_PER_SECOND 	// # of samples that must be filled in before calculating an output

// gesture senstivity thresholds
#define DEADZONE_PITCH				4								// up and down twist dead zone
#define PITCH_UP_LIMIT				25								// rotate
#define PITCH_DN_LIMIT				26								// place down

#define DEADZONE_YAW 				4								// left and right twist dead zone
#define YAW_LFT_LIMIT 				21								// swipe left
#define YAW_RGT_LIMIT				16								// swipe right

uint8_t data_fifo[FIFOSIZE];

uint8_t startup = 'Y';
int init_yaw = 0;
int curr_sample_num = 0;
char last_gesture = 'N';

struct Packet
{
	float pitch [PACKET_SIZE];
	float yaw [PACKET_SIZE];
};

struct Packet curr_packet;


float convertToFloat(uint8_t lsb, uint8_t msb)
{
	int16_t result = ((int16_t) msb << 8) | lsb;
	int8_t decimal_part = result % 100;
	int16_t integer_part = result / 100;
	float final_number = integer_part + (float) decimal_part / 100.0;
	return final_number;
}

void detect_gesture(struct Packet curr_packet)
{
    int down_pitch_count = 0;
    int up_pitch_count = 0;
    int left_yaw_count = 0;
    int right_yaw_count = 0;

    for (int i = 0; i < PACKET_SIZE; i++)
    {
        if (curr_packet.pitch[i] < -(DEADZONE_PITCH + PITCH_UP_LIMIT))
        {
            up_pitch_count++;
        }
        if(curr_packet.pitch[i] > (DEADZONE_PITCH + PITCH_DN_LIMIT))
        {
        	down_pitch_count++;
        }
        if(curr_packet.yaw[i] > (DEADZONE_YAW + YAW_RGT_LIMIT))
		{
			right_yaw_count++;
		}
		if(curr_packet.yaw[i] < -(DEADZONE_YAW + YAW_LFT_LIMIT))
		{
			left_yaw_count++;
		}
    }

    if (down_pitch_count >= PACKET_SIZE / 2)
    {
        sendBxChar('D');
        last_gesture = 'D';
    }
    else if (up_pitch_count >= PACKET_SIZE / 2)
	{
		sendBxChar('U');
		last_gesture = 'U';
	}
    else if (left_yaw_count >= PACKET_SIZE / 2)
	{
		sendBxChar('L');
        last_gesture = 'N';
	}
	else if (right_yaw_count >= PACKET_SIZE / 2)
	{
		sendBxChar('R');
		last_gesture = 'N';
	}
    else
	{
    	last_gesture = 'N';
	}
}

void DMA1_CH2_3_DMA2_CH1_2_IRQHandler()
{
	if(data_fifo[0] == 0xAA && data_fifo[1] == 0XAA)
	{
		if(curr_sample_num == PACKET_SIZE)
		{
			curr_sample_num = 0;
			detect_gesture(curr_packet);
		}
		else
		{
			DMA1 -> IFCR |= DMA_IFCR_CTCIF3;
			curr_packet.yaw[curr_sample_num] = convertToFloat(data_fifo[3], data_fifo[4]) - init_yaw;
			curr_packet.pitch[curr_sample_num] = convertToFloat(data_fifo[5], data_fifo[6]);
			if(startup == 'Y')
			{
				init_yaw = curr_packet.yaw[curr_sample_num];
				startup = 'N';
			}
			curr_sample_num += 1;
		}
	}
	else
	{
		DMA1_Channel3->CCR &= ~DMA_CCR_EN;
		setup_IMU_DMA();
	}
}

void EXTI0_1_IRQHandler()
{
	EXTI->PR |= EXTI_PR_PR0;
	sendBxChar('X');
}

void setup_IMU_GPIO(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;					// portB clock
	GPIOB->MODER &= ~(GPIO_MODER_MODER7);				// clearing PB7 to input
	GPIOB->MODER |= GPIO_MODER_MODER7_1; 				// setting PB7 to AFR mode (UART)
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFRL7); 	   			// selecting AF0 for PB7

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;  			// enable syscfg clock
	SYSCFG->EXTICR[0] &= SYSCFG_EXTICR1_EXTI0_PA; 		// enable pa0 on EXTI0
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;					// portA clock
	GPIOA->MODER &= ~(GPIO_MODER_MODER0);				// clearing PA0 to
	EXTI->RTSR |= EXTI_RTSR_TR0;						// interrupt on Rising edge PA0
	EXTI->IMR |= EXTI_IMR_MR0;							// enable PA0 interrupt
	NVIC->ISER[0] |= (1 << EXTI0_1_IRQn);
}

void setup_IMU_UART(void)
{
	RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;             // clock on for usart1
	USART1 -> CR1 &= ~USART_CR1_UE;                     // turn off for config
	USART1 -> CR1 &= ~USART_CR1_M;                      // 8 bit word length
	USART1 -> CR2 &= ~USART_CR2_STOP;                   // 1 stop bits
	USART1 -> CR1 &= ~USART_CR1_PCE;                    // parity disable
	USART1 -> CR1 &= ~USART_CR1_OVER8;                  // 16x oversampling
	USART1 -> BRR = 48000000 / 115200;					// 115200 baud rate
	USART1 -> CR1 |= USART_CR1_RE;				        // receiver enable
	USART1 -> CR3 |= USART_CR3_DMAR;					// come dma steal my data
	USART1 -> CR1 |= USART_CR1_UE;                      // enable usart
}

void setup_IMU_DMA()
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	DMA1_Channel3->CCR &= ~DMA_CCR_EN;      			// turn off enable bit
	DMA1_Channel3->CMAR = (uint32_t) data_fifo;    	 	// cmar address (read to here)
	DMA1_Channel3->CPAR = (uint32_t) &(USART1->RDR); 	// cpar address (read from here)
	DMA1_Channel3->CNDTR = FIFOSIZE;        			// cndtr set to 8
	DMA1_Channel3->CCR &= ~DMA_CCR_DIR;     			// read from perip
	DMA1_Channel3->CCR &= ~DMA_CCR_MSIZE;   			// 16 bit m size
	DMA1_Channel3->CCR &= ~DMA_CCR_PSIZE;   			// 16 bit p size
	DMA1_Channel3->CCR |= DMA_CCR_MINC;     			// increment CMAR
	DMA1_Channel3->CCR |= DMA_CCR_CIRC;					// circular mode
	DMA1_Channel3->CCR |= DMA_CCR_PL_0 | DMA_CCR_PL_1;  // highest priority
	DMA1_Channel3->CCR |= DMA_CCR_TCIE; 				// int on transfer complete
	DMA1_Channel3->CCR |= DMA_CCR_EN;					// enable for dma
	NVIC -> ISER[0] |= (1 << DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
}

void sendBxChar(char txdata)
{
	if (txdata != last_gesture)
	{
	while(!(USART5->ISR & USART_ISR_TXE)) {}
  	USART5->TDR = txdata;
	}
}

//void debugSend(char txdata) {
//  while(!(USART5->ISR & USART_ISR_TXE)){}
//      USART5->TDR = txdata;
//}
//
//void debugSendString(char* data) {
//  int i = 0;
//  while(data[i] != '\0') {
//    debugSend(data[i]);
//    i++;
//  }
//}


// ************************************
// CODE ENTRY *************************
// ************************************

int main(void)
{
	// bluetooth
	setup_BX_GPIO();
	setup_BX_UART();
	BluetoothSetup();
	// imu
	setup_IMU_GPIO();
	setup_IMU_DMA();
	setup_IMU_UART();

	while(1)
	{
		asm("wfi");
	}
}
