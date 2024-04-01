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

// *****************************************
// BLUETOOTH CODE **************************
// *****************************************

// main game receiver characteristics for right glove (MASTER)
const char *RX_MACADDR_R = "E4E11295D8C6"; // known MAC address for Main RX BT on STM32F0 PCB
const char *RX_NAME_R   = "TetrisHub";
const char *RX_ADVINT_R  = "100ms";

// right glove transmitter characteristics (SLAVE)
const char *TX_MACADDR_R = "6098665B565";
const char *TX_NAME_R = "GloveR";
const char *TX_ADVINT_R  = "100ms";

// left glove transmitter characteristics (SLAVE)
const char *TX_MACADDR_L = "E4E11295D794";
const char *TX_NAME_L = "GloveL";
const char *TX_ADVINT_L  = "100ms";


void getATAddr()
{
	  uint8_t testAddrTx[8] = {'A','T','+','A','D','D','R','?'};
	  uint8_t testAddrRx[29] = {};

	  // send "AT+Addr?" command
	  for(uint32_t i = 0; i < 8; i++){
		  while(!(USART5->ISR & USART_ISR_TXE)){}
		  USART5 -> TDR = testAddrTx[i];
	  }

	  // expect "OK+Mac Addr" from Bx
	  for(int i = 0; i < 29; i++)
	  {
		  while (!(USART5->ISR & USART_ISR_RXNE)) {}
		  testAddrRx[i] = USART5->RDR;
	  }
}

void sendATStart()
{
	char *startTx = "AT+START";
	char startRx[8] = {}; //"OK+START"
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

void sendATDisc()
{
	char *discTx = "AT+DISC?";
	char discRx[8] = {}; //"OK+DISCS"
	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = discTx[i];
	}

	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_RXNE)){}
		discRx[i] = USART5 -> RDR;
	}
}

void setATRole()
{
	// AT+ROLE? -> ask current role
	// AT+ROLE1 -> set to master
	// AT+ROLE0 -> set to slave
	char roleTx[8] = "AT+ROLE0";
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

void setAdvInterval()
{
	char* advTx = "AT+ADVI9";
	char advRx[8] = {};

	// send message to obtain advertising interval
	for(uint32_t i = 0; i < 8; i++)
	{
		while(!(USART5->ISR & USART_ISR_TXE)){}
		USART5 -> TDR = advTx[i];
	}

	// expect "OK+GET:[P]" from Bx
	for(int i = 0; i < 8; i++)
	{
		while (!(USART5->ISR & USART_ISR_RXNE)) {}
		advRx[i] = USART5->RDR;
	}
}

void sendATCheck()
{
	  uint8_t testTX[2] = "AT";
	  uint8_t testRX[2];

	  // send "AT" test command
	  for(uint32_t i = 0; i < 2; i++){
		  while(!(USART5->ISR & USART_ISR_TXE)){}
		  USART5 -> TDR = testTX[i];
	  }

	  // expect "OK" from Bx
	  for(int i = 0; i < 2; i++)
	  {
		  while (!(USART5->ISR & USART_ISR_RXNE)) {}
		  testRX[i] = USART5->RDR;
	  }
}

void setATImme()
{
	// AT+IMME1 -> if slave, wait for AT+START to advertise
	//			-> if master, wait for AT+DISC to discover
	// AT+IMME0 -> start working from get go.

	char immeTx[8] = "AT+IMME0";
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

	  for(int i = 0; i < 8; i++)
	  {
		  while (!(USART5->ISR & USART_ISR_RXNE)) {}
		  nameRx[i] = USART5->RDR;
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
//	sendATCheck();
//	getATAddr();
//	setBxName();
//	setATRole();
//	setATImme();
//	sendATReset();
//	sendATStart();
//	sendATDisc();
}


// *****************************************
// IMU CODE ********************************
// *****************************************

// sizing constants
#define FIFOSIZE 					19								// # of bytes to receive from IMU into temp buffer before processing
#define PREDICTIONS_PER_SECOND 	3 								// # of times the entire toolchain runs to display a output
#define PACKET_SIZE				100 / PREDICTIONS_PER_SECOND 	// # of samples that must be filled in before calculating an output

// gesture senstivity thresholds
#define DEADZONE_ROLL 				5								// deadzone consideration for resting hand movement
#define ROLL_THRESHOLD 			32								//

uint8_t data_fifo[FIFOSIZE];

struct Packet
{
	float roll  [PACKET_SIZE];

};
struct Packet curr_packet;

int curr_sample_num = 0;

float convertToFloat(uint8_t lsb, uint8_t msb)
{
	int16_t result = ((int16_t) msb << 8) | lsb;
	int8_t decimal_part = result % 100;
	int16_t integer_part = result / 100;
	float final_number = integer_part + (float) decimal_part / 100.0;
	return final_number;
}

//void detect_roll(float sampled_roll_values[])
//{
//    int left_roll_count = 0;
//    int right_roll_count = 0;
//
//    for (int i = 0; i < PACKET_SIZE; i++)
//    {
//        // Check if roll is outside deadzones and thresholds
//        if (sampled_roll_values[i] < -(DEADZONE_ROLL + ROLL_THRESHOLD))
//        {
//            left_roll_count++; // Increment roll count for significant roll samples
//        }
//        else if(sampled_roll_values[i] > DEADZONE_ROLL + ROLL_THRESHOLD)
//        {
//        	right_roll_count++;
//        }
//    }
//    // Check if the number of significant roll samples exceeds a threshold (e.g., 5 out of 10)
//    if (left_roll_count >= PACKET_SIZE / 2)
//    {
//        sendBxString("1LR"); // Send message indicating roll detection
//    }
//    else if (right_roll_count >= PACKET_SIZE / 2)
//	{
//		sendBxString("1RR"); // Send message indicating roll detection
//	}
//}

void detect_swipe_horizontal(float sampled_x_acl_values[])
{
    int left_swipe_count = 0;
    int right_swipe_count = 0;

    for (int i = 0; i < PACKET_SIZE; i++)
    {
        // Check if roll is outside deadzones and thresholds
        if (sampled_x_acl_values[i] < -X_ACL_THRESHOLD)
        {
            left_swipe_count++; // Increment roll count for significant roll samples
        }
        else if(sampled_x_acl_values[i] > X_ACL_THRESHOLD)
        {
        	right_swipe_count++;
        }
    }
    // Check if the number of significant roll samples exceeds a threshold (e.g., 5 out of 10)
    if (left_swipe_count >= PACKET_SIZE / 2)
    {
    	sendBxString("1LS"); // Send message indicating roll detection
    }
    else if (right_swipe_count >= PACKET_SIZE / 2)
	{
    	sendBxString("1RS"); // Send message indicating roll detection
	}
}

void DMA1_CH2_3_DMA2_CH1_2_IRQHandler()
{
	//only have 10 ms for this processing
	if(data_fifo[0] == 0xAA && data_fifo[1] == 0XAA) // enforce header check
	{
		if(curr_sample_num == PACKET_SIZE)
		{
			// do prediction call
			curr_sample_num = 0;
			sendBxString("PRD");
//			detect_roll(curr_packet.roll);
			detect_swipe_horizontal(curr_packet.x_acl);
		}
		else
		{
			// fill packet buffer
			DMA1 -> IFCR |= DMA_IFCR_CTCIF3; // clear flag
			curr_packet.roll[curr_sample_num] = convertToFloat(data_fifo[7], data_fifo[8]);
			curr_packet.x_acl[curr_sample_num] = convertToFloat(data_fifo[9], data_fifo[10]);
			curr_packet.z_acl[curr_sample_num] = convertToFloat(data_fifo[13], data_fifo[14]);
			curr_sample_num += 1;
		}
	}
	else // got mis-aligned, cleanup and start again.
	{
		sendBxString("!MSALG!");
		DMA1_Channel3->CCR &= ~DMA_CCR_EN;
		setup_IMU_DMA();
	}
}

void setup_IMU_GPIO(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER &= ~(GPIO_MODER_MODER7);
	GPIOB->MODER |= GPIO_MODER_MODER7_1; 		// AFR mode (UART)
	GPIOB->AFR[0] &= ~(GPIO_AFRL_AFRL7); 	   	// AF0
}

void setup_IMU_UART(void)
{
	RCC -> APB2ENR |= RCC_APB2ENR_USART1EN;                // clock on for usart1
	USART1 -> CR1 &= ~USART_CR1_UE;                        // turn off for config
	USART1 -> CR1 &= ~USART_CR1_M;                         // 8 bit word length
	USART1 -> CR2 &= ~USART_CR2_STOP;                      // 1 stop bits
	USART1 -> CR1 &= ~USART_CR1_PCE;                       // parity disable
	USART1 -> CR1 &= ~USART_CR1_OVER8;                     // 16x oversampling
	USART1 -> BRR = 48000000 / 115200;					   // 115200 baud rate
	USART1 -> CR1 |= USART_CR1_RE;				           // receiver enable
	USART1 -> CR3 |= USART_CR3_DMAR;					   // come dma steal my data
	USART1 -> CR1 |= USART_CR1_UE;                         // enable usart
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
	NVIC -> ISER[0] |= (1<<DMA1_Ch2_3_DMA2_Ch1_2_IRQn);
}

void sendBxChar(char txdata)
{
  while(!(USART5->ISR & USART_ISR_TXE)) {}
      USART5->TDR = txdata;
}

void sendBxString(char* data)
{
  int i = 0;
  while(data[i] != '\0')
  {
    sendBxChar(data[i]);
    i++;
  }
}

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
