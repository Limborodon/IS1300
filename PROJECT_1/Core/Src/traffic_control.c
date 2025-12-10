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

TrafficState_t_T1 currentState = STATE_IDLE_T1;

void task1(){
	Shift_Out_24(0,0,0);
	while(1){
		switch (currentState){
		        case STATE_IDLE_T1:

		        case STATE_BLINK_WAIT_T1:

		        case STATE_CAR_ORANGE_T1:

		            break;

		        case STATE_PED_GREEN_T1:

		            break;

		        case STATE_PED_GREEN_END_T1:

		            break;

		        case STATE_RETURN_ORANGE_T1:

		            break;
		    }

	}

}
void traffic_control(){
	task1();
}

