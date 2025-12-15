/**
******************************************************************************
@brief file for the implementation tasks
@file traffic_control.c
@author Linus Wennergren
@date 8-December-2025
******************************************************************************
*/
#include<main.h>

#include "traffic_control_functions.h"


typedef enum {
    STATE_IDLE_T1,
    STATE_BLINK_WAIT_T1,
    STATE_CAR_ORANGE_T1,
    STATE_PED_GREEN_T1,
    STATE_PED_GREEN_END_T1,
    STATE_RETURN_ORANGE_T1
} TrafficState_t_T1;

TrafficState_t_T1 current_state = STATE_IDLE_T1;

uint32_t stateStartTime = 0;
uint32_t lastBlinkTime  = 0;
uint8_t indicatorState  = 0;


// Timing variables
uint32_t toggleFreq      = 300;
uint32_t pedestrianDelay = 4000;
uint32_t walkingDelay    = 5000;
uint32_t orangeDelay     = 2000;

void task1(void)
{
	while (1)
	    {
	        uint32_t now = HAL_GetTick();

	        // Default
	        lights_reset();
	        Debounce_Button_Inputs();

	        switch (current_state)
	        {
	        case STATE_IDLE_T1:
	            light_set(ID_PL2_RED, COLOR_ON);
	            light_set(ID_TL2_GREEN, COLOR_ON);
	            light_set(ID_TL4_GREEN, COLOR_ON);

	            if (Upper_Pedestrian_Button_Pressed()) {
	                stateStartTime = now;
	                lastBlinkTime = now;
	                indicatorState = 0;
	                current_state = STATE_BLINK_WAIT_T1;
	            }
	            break;

	        case STATE_BLINK_WAIT_T1:

	            // Blink
	            if (now - lastBlinkTime >= toggleFreq) {
	                indicatorState ^= 1;
	                lastBlinkTime = now;
	            }

	            light_set(ID_TL2_GREEN, COLOR_ON);
	            light_set(ID_PL2_RED, COLOR_ON);
	            light_set(ID_TL4_GREEN, COLOR_ON);

	            if(indicatorState) light_set(ID_PL2_BLUE, COLOR_ON);

	            // After pedestrianDelay STATE car orange
	            if (now - stateStartTime >= pedestrianDelay) {
	                stateStartTime = now;
	                current_state = STATE_CAR_ORANGE_T1;
	            }
	            break;

	        case STATE_CAR_ORANGE_T1:
	        	//Should blink until ped light green, thus also when orange from green
	        	if (now - lastBlinkTime >= toggleFreq) {
	        		indicatorState ^= 1;
	        		lastBlinkTime = now;
	        	}
	            light_set(ID_TL2_YELLOW, COLOR_ON);
	            light_set(ID_PL2_RED, COLOR_ON);
	            light_set(ID_TL4_YELLOW, COLOR_ON);

	            if (indicatorState) {
	            	light_set(ID_PL2_BLUE, COLOR_ON);
	            }

	            if (now - stateStartTime >= orangeDelay) {
	                stateStartTime = now;
	                current_state = STATE_PED_GREEN_T1;
	            }
	            break;

	        case STATE_PED_GREEN_T1:
	            light_set(ID_TL2_RED, COLOR_ON);
	            light_set(ID_PL2_GREEN, COLOR_ON);
	            light_set(ID_TL4_RED, COLOR_ON);

	            if (now - stateStartTime >= walkingDelay) {
	                current_state = STATE_PED_GREEN_END_T1;
	            }
	            break;

	        case STATE_PED_GREEN_END_T1:
	            light_set(ID_TL2_RED, COLOR_ON);
	            light_set(ID_PL2_RED, COLOR_ON);
	            light_set(ID_TL4_RED, COLOR_ON);

	            stateStartTime = now;
	            current_state = STATE_RETURN_ORANGE_T1;
	            break;

	        case STATE_RETURN_ORANGE_T1:
	            light_set(ID_TL2_YELLOW, COLOR_ON);
	            light_set(ID_PL2_RED, COLOR_ON);
	            light_set(ID_TL4_YELLOW, COLOR_ON);

	            if (now - stateStartTime >= orangeDelay) {
	                current_state = STATE_IDLE_T1;
	            }
	            break;
	        }

	        Output_Lights();
	    }
}

void task2(){


}
void traffic_control(){
	task1();
}

