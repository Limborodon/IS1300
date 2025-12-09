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

/**
******************************************************************************
@brief Tests all the leds using the shift_out_24() function.
@author Linus Wennergren
@date 9-December-2025
******************************************************************************
*/
void test_leds()
{
	//U3: skip Q6, Q7
	//Q5:TL4_Green Q4:TL4_Yellow,  Q3: TL4_Red Q2:TL3_Green Q1:TL3_Yellow Q0: TL3_Red
	for(int i = 5; i >= 0; i--){
		uint8_t byte_U3 = 1 << i;
		Shift_Out_24(byte_U3,0,0);
		HAL_Delay(5000); // 5 seconds between lights
	}
	//U2: skip Q6, Q7
	//Q5: PL2_Blue Q4: PL2_green Q3: PL2_Red Q2: TL2_Green Q1:TL2_Yellow Q0: TL2_Red

	for(int i = 5; i >= 0; i--){
		uint8_t byte_U2 = 1 << i;
		Shift_Out_24(0,byte_U2,0);
		HAL_Delay(5000);
	}
	//U1: skip Q6, Q7
	//Q5: PL1_Blue Q4: PL1_green Q3: PL1_Red Q2: TL1_Green Q1:TL1_Yellow Q0: TL1_Red
	for(int i = 5; i >= 0; i--){
		uint8_t byte_U1 = 1 << i;
		Shift_Out_24(0,0,byte_U1);
		HAL_Delay(5000);
	}


}

void test_program(){
	test_leds();

}



