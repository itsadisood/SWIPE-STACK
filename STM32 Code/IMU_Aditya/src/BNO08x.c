/*
 * BNO08x.c
 *
 *  Created on: Feb 1, 2024
 *      Author: Aditya
 */


#include "BNO08x.h"
#include <math.h>

void toggle_IMU_RST(uint8_t num)
{
	if(num == 1)
		GPIOB -> BSRR |= GPIO_BSRR_BR_10;
	else if(num == 0)
		GPIOB -> BSRR |= GPIO_BSRR_BS_10;
}


void toggle_IMU_NSS(uint8_t num)
{
	if(num == 1)
		GPIOB -> BSRR |= GPIO_BSRR_BR_12;
	else if(num == 0)
		GPIOB -> BSRR |= GPIO_BSRR_BS_12;
}
