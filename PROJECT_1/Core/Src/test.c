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
/*
void test_leds()
{
	//U3: skip Q6, Q7 10000001
	//Q5:TL4_Green Q4:TL4_Yellow,  Q3: TL4_Red Q2:TL3_Green Q1:TL3_Yellow Q0: TL3_Red
	for(int i = 5; i >= 0; i--){
		uint8_t byte_U3 = 1 << i;
		Output_Lights(byte_U3,0,0);
		HAL_Delay(5000); // 5 seconds between lights
	}
	//U2: skip Q6, Q7
	//Q5: PL2_Blue Q4: PL2_green Q3: PL2_Red Q2: TL2_Green Q1:TL2_Yellow Q0: TL2_Red

	for(int i = 5; i >= 0; i--){
		uint8_t byte_U2 = 1 << i;
		Output_Lights(0,byte_U2,0);
		HAL_Delay(5000);
	}
	//U1: skip Q6, Q7
	//Q5: PL1_Blue Q4: PL1_green Q3: PL1_Red Q2: TL1_Green Q1:TL1_Yellow Q0: TL1_Red
	for(int i = 5; i >= 0; i--){
		uint8_t byte_U1 = 1 << i;
		Output_Lights(0,0,byte_U1);
		HAL_Delay(5000);
	}


}
*/
// lights assigned to 1-18 and tested with HAL_Delay individually turning all of them on induvidually then off with 2 seconds per light switch.
//Check with visual confirmation
void test_lights(){
	// turns all lights on 1 by 1 every 2 seconds
	for(int i = 1; i <= 18; i++){
		light_set(i,COLOR_ON);
		HAL_Delay(2000); // 5 seconds
		Output_Lights();
	}// turns off every other light from IDs
	for(int i = 1; i <= 18; i++){
		light_set(i,COLOR_OFF);
		HAL_Delay(2000); // 2 seconds
		Output_Lights();
	}

}
// turns on all lights then waits 5 seconds and turns them off with lights_reset().
void test_lights_reset(){
	for(int i = 1; i <= 18; i++){
		light_set(i,COLOR_ON);
	}
	Output_Lights();
	HAL_Delay(5000); //wait 5 sec
	lights_reset();
	Output_Lights();

}

void test_buttons(){
	uint32_t lit_time = 5000; // 5000 ms
	uint32_t PL1_last_change_time = 0;
	uint32_t PL2_last_change_time = 0;
	lights_reset();
	while(1){
	Debounce_Button_Inputs();
	uint32_t current_time = HAL_GetTick();

	if(Lower_Pedestrian_Button_Pressed()){
		light_set(ID_PL1_BLUE,COLOR_ON);
		PL1_last_change_time = current_time;
	}

	if(Upper_Pedestrian_Button_Pressed()){
		light_set(ID_PL2_BLUE,COLOR_ON);
		PL2_last_change_time = current_time;
	}

	if(current_time - PL1_last_change_time >= lit_time) light_set(ID_PL1_BLUE,COLOR_OFF);
	if(current_time - PL2_last_change_time >= lit_time) light_set(ID_PL2_BLUE,COLOR_OFF);
	Output_Lights();
	}

}

//tests switches by turning on their respective traffic lights green light if switch state is high and turns off if switch state low.
void test_switches(){
	Debounce_Switch_Inputs();
	while(1){
		Debounce_Switch_Inputs();
		if(Car_Present(1)) light_set(ID_TL1_GREEN, COLOR_ON); else light_set(ID_TL1_GREEN, COLOR_OFF);
		if(Car_Present(2)) light_set(ID_TL2_GREEN, COLOR_ON); else light_set(ID_TL2_GREEN, COLOR_OFF);
		if(Car_Present(3)) light_set(ID_TL3_GREEN, COLOR_ON); else light_set(ID_TL3_GREEN, COLOR_OFF);
		if(Car_Present(4)) light_set(ID_TL4_GREEN, COLOR_ON); else light_set(ID_TL4_GREEN, COLOR_OFF);
		Output_Lights();
	}

}

void test_program(){
	//test_lights();
	//test_lights_reset();
	//test_buttons();
	test_switches();

}






