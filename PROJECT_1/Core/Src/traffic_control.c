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

// State machine for Task 1
typedef enum {
    STATE_IDLE_T1,
    STATE_BLINK_WAIT_T1,
    STATE_CAR_ORANGE_T1,
    STATE_PED_GREEN_T1,
    STATE_PED_GREEN_END_T1,
    STATE_RETURN_ORANGE_T1
} TrafficState_t_T1;

TrafficState_t_T1 current_state = STATE_IDLE_T1;

// State machine for Task 2
typedef enum {
    STATE_VERTICAL_GREEN_T2,
    STATE_VERTICAL_ORANGE_T2,
    STATE_HORIZONTAL_RED_YELLOW_T2,
    STATE_HORIZONTAL_GREEN_T2,
    STATE_HORIZONTAL_ORANGE_T2,
    STATE_VERTICAL_RED_YELLOW_T2
} TrafficState_t_T2;

TrafficState_t_T2 current_state_T2 = STATE_VERTICAL_GREEN_T2;

uint32_t stateStartTime = 0;
uint32_t lastBlinkTime  = 0;
uint8_t indicatorState  = 0;

uint32_t stateStartTime_T2 = 0;
uint32_t redWaitStartTime_T2 = 0;


// Timing variables
uint32_t toggleFreq      = 300;
uint32_t pedestrianDelay = 4000;
uint32_t walkingDelay    = 5000;
uint32_t orangeDelay     = 2000;
uint32_t greenDelay      = 5000;
uint32_t redDelayMax     = 6000;

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

// Implementation of Task 2
void task2(){
    // Initialization
    current_state_T2 = STATE_VERTICAL_GREEN_T2;
    stateStartTime_T2 = HAL_GetTick();

    while (1)
    {
        uint32_t now = HAL_GetTick();

        lights_reset();

        Debounce_Button_Inputs();
        Debounce_Switch_Inputs();

        // Read lane status
        bool vertical_active = Vertical_Car_Sensor_Active();
        bool horizontal_active = Horizontal_Car_Sensor_Active();

        //Implementation of state machine for task 2
        switch (current_state_T2)
        {
            // =========================================================
            case STATE_VERTICAL_GREEN_T2:
                light_set(ID_TL1_GREEN, COLOR_ON); light_set(ID_TL3_GREEN, COLOR_ON);
                light_set(ID_TL2_RED, COLOR_ON); light_set(ID_TL4_RED, COLOR_ON);


                if (horizontal_active && !vertical_active) {
                    stateStartTime_T2 = now;
                    redWaitStartTime_T2 = 0;
                    current_state_T2 = STATE_VERTICAL_ORANGE_T2;
                    break;
                }

                // Only if a car is waiting at the red light
                if (horizontal_active) {
                    if (redWaitStartTime_T2 == 0) {
                        redWaitStartTime_T2 = now; // Start tracking wait time
                    }

                    if (now - redWaitStartTime_T2 >= redDelayMax) {
                        stateStartTime_T2 = now;
                        redWaitStartTime_T2 = 0;
                        current_state_T2 = STATE_VERTICAL_ORANGE_T2;
                    } else if (!vertical_active && (now - stateStartTime_T2 >= greenDelay)) {
                        stateStartTime_T2 = now;
                        redWaitStartTime_T2 = 0;
                        current_state_T2 = STATE_VERTICAL_ORANGE_T2;
                    }
                } else {
                    // If no car waiting at red reset the red wait timer.
                    redWaitStartTime_T2 = 0;
                }

                if (!vertical_active && !horizontal_active && (now - stateStartTime_T2 >= greenDelay)) {
                    stateStartTime_T2 = now;
                    current_state_T2 = STATE_VERTICAL_ORANGE_T2;
                }

                break;

            case STATE_VERTICAL_ORANGE_T2:
                light_set(ID_TL1_YELLOW, COLOR_ON); light_set(ID_TL3_YELLOW, COLOR_ON);
                light_set(ID_TL2_RED, COLOR_ON); light_set(ID_TL4_RED, COLOR_ON);

                if (now - stateStartTime_T2 >= orangeDelay) {
                    stateStartTime_T2 = now;
                    current_state_T2 = STATE_HORIZONTAL_RED_YELLOW_T2;
                }
                break;

            // =========================================================
            case STATE_HORIZONTAL_RED_YELLOW_T2:
                light_set(ID_TL1_RED, COLOR_ON); light_set(ID_TL3_RED, COLOR_ON);
                light_set(ID_TL2_RED, COLOR_ON); light_set(ID_TL4_RED, COLOR_ON);
                light_set(ID_TL2_YELLOW, COLOR_ON); light_set(ID_TL4_YELLOW, COLOR_ON);

                if (now - stateStartTime_T2 >= orangeDelay) {
                    stateStartTime_T2 = now;
                    current_state_T2 = STATE_HORIZONTAL_GREEN_T2;
                }
                break;
            case STATE_HORIZONTAL_GREEN_T2:
                light_set(ID_TL1_RED, COLOR_ON); light_set(ID_TL3_RED, COLOR_ON);
                light_set(ID_TL2_GREEN, COLOR_ON); light_set(ID_TL4_GREEN, COLOR_ON);

                if (vertical_active && !horizontal_active) {
                    stateStartTime_T2 = now;
                    redWaitStartTime_T2 = 0;
                    current_state_T2 = STATE_HORIZONTAL_ORANGE_T2;
                    break;
                }

                if (vertical_active) {
                    if (redWaitStartTime_T2 == 0) {
                        redWaitStartTime_T2 = now;
                    }

                    if (now - redWaitStartTime_T2 >= redDelayMax) {
                        stateStartTime_T2 = now;
                        redWaitStartTime_T2 = 0;
                        current_state_T2 = STATE_HORIZONTAL_ORANGE_T2;

                    } else if (!horizontal_active && (now - stateStartTime_T2 >= greenDelay)) {
                        stateStartTime_T2 = now;
                        redWaitStartTime_T2 = 0;
                        current_state_T2 = STATE_HORIZONTAL_ORANGE_T2;
                    }
                } else {
                    redWaitStartTime_T2 = 0;
                }

                if (!vertical_active && !horizontal_active && (now - stateStartTime_T2 >= greenDelay)) {
                    stateStartTime_T2 = now;
                    current_state_T2 = STATE_HORIZONTAL_ORANGE_T2;
                }

                break;

            case STATE_HORIZONTAL_ORANGE_T2:
                light_set(ID_TL1_RED, COLOR_ON); light_set(ID_TL3_RED, COLOR_ON);
                light_set(ID_TL2_YELLOW, COLOR_ON); light_set(ID_TL4_YELLOW, COLOR_ON);

                if (now - stateStartTime_T2 >= orangeDelay) {
                    stateStartTime_T2 = now;
                    current_state_T2 = STATE_VERTICAL_RED_YELLOW_T2;
                }
                break;

            // =========================================================
            case STATE_VERTICAL_RED_YELLOW_T2:
                light_set(ID_TL1_RED, COLOR_ON); light_set(ID_TL3_RED, COLOR_ON);
                light_set(ID_TL2_RED, COLOR_ON); light_set(ID_TL4_RED, COLOR_ON);
                light_set(ID_TL1_YELLOW, COLOR_ON); light_set(ID_TL3_YELLOW, COLOR_ON);

                if (now - stateStartTime_T2 >= orangeDelay) {
                    stateStartTime_T2 = now;
                    current_state_T2 = STATE_VERTICAL_GREEN_T2;
                }
                break;
        }

        Output_Lights();
    }
}
void traffic_control(){
	//task1();
	task2();
}

