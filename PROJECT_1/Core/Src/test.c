/**
******************************************************************************
@brief file for program tests
@file test.c
@author Linus Wennergren
@date 8-December-2025
******************************************************************************
*/
#include "main.h"
#include "traffic_control_functions.h"
#include <math.h>

void test_leds()
{

	for(int i = 0; i < 24; i++){
		uint32_t data = 1UL << i; // raises 2 to the power of i aka bit index i of the uint32_t
		uint8_t byte_U3 = (uint8_t) (data >> 16 & 0xFF);
		uint8_t byte_U2 = (uint8_t) (data >> 8 & 0xFF);
		uint8_t byte_U1 = (uint8_t) (data >> 0 &0xFF) ;
		Shift_Out_24(byte_U3, byte_U2, byte_U1);
		HAL_Delay(3000);


	}
}

void test_program(){
	test_leds();

}



