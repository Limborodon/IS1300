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
    STATE_WAITING_T1,
    STATE_WALKING_T1
} TrafficState_t_T1;

TrafficState_t_T1 current_state = STATE_IDLE_T1;

// State machine for Task 2
typedef enum {
    STATE_VERTICAL_GREEN_T2,
    STATE_VERTICAL_ORANGE_T2,
    STATE_HORIZONTAL_GREEN_T2,
    STATE_HORIZONTAL_ORANGE_T2,
	STATE_VERTICAL_ORANGE_PREPARE_T2,
	STATE_HORIZONTAL_ORANGE_FINISH_T2

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

void task1_logic(uint32_t now) {
    switch (current_state) {
        case STATE_IDLE_T1:
            light_set(ID_PL2_RED, COLOR_ON);
            if (Upper_Pedestrian_Button_Pressed()) {
                stateStartTime = now;
                lastBlinkTime = now;
                indicatorState = 0;
                current_state = STATE_WAITING_T1;
            }
            break;

        case STATE_WAITING_T1:
            if (now - lastBlinkTime >= toggleFreq) {
                indicatorState ^= 1;
                lastBlinkTime = now;
            }
            light_set(ID_PL2_RED, COLOR_ON);
            if(indicatorState) light_set(ID_PL2_BLUE, COLOR_ON);

            // Saftey Check: Vertical cars must be red before walking
            bool cars_are_stopped = (current_state_T2 == STATE_HORIZONTAL_GREEN_T2 ||
                                     current_state_T2 == STATE_HORIZONTAL_ORANGE_T2);

            if ((now - stateStartTime >= pedestrianDelay) && cars_are_stopped) {
                stateStartTime = now;
                current_state = STATE_WALKING_T1;
            }
            break;

        case STATE_WALKING_T1:
            light_set(ID_PL2_GREEN, COLOR_ON);
            if (now - stateStartTime >= walkingDelay) {
                current_state = STATE_IDLE_T1;
            }
            break;
    }
}

void task2_logic(uint32_t now) {
    bool vertical_active = Vertical_Car_Sensor_Active();
    bool horizontal_active = Horizontal_Car_Sensor_Active();
    bool ped_waiting = (current_state == STATE_WAITING_T1);

    switch (current_state_T2) {

        case STATE_VERTICAL_GREEN_T2:
            light_set(ID_TL2_GREEN, COLOR_ON); light_set(ID_TL4_GREEN, COLOR_ON);
            light_set(ID_TL1_RED, COLOR_ON);   light_set(ID_TL3_RED, COLOR_ON);

            // If a Horizontal car or Pedestrian is waiting
            if (horizontal_active || ped_waiting) {
                if (redWaitStartTime_T2 == 0) redWaitStartTime_T2 = now;

                // R 2.7: Immediate if vertical empty. R 2.6: Force if redDelayMax reached.
                if (!vertical_active || (now - redWaitStartTime_T2 >= redDelayMax)) {
                    stateStartTime_T2 = now;
                    redWaitStartTime_T2 = 0;
                    current_state_T2 = STATE_VERTICAL_ORANGE_T2;
                }
            } else {
                redWaitStartTime_T2 = 0;
                // R 2.4: Empty cycle after greenDelay
                if (!vertical_active && (now - stateStartTime_T2 >= greenDelay)) {
                    stateStartTime_T2 = now;
                    current_state_T2 = STATE_VERTICAL_ORANGE_T2;
                }
            }
            break;

        case STATE_VERTICAL_ORANGE_T2:
            light_set(ID_TL2_YELLOW, COLOR_ON); light_set(ID_TL4_YELLOW, COLOR_ON);
            light_set(ID_TL1_RED, COLOR_ON);    light_set(ID_TL3_RED, COLOR_ON);
            if (now - stateStartTime_T2 >= orangeDelay) {
                stateStartTime_T2 = now;
                current_state_T2 = STATE_HORIZONTAL_ORANGE_T2;
            }
            break;

        case STATE_HORIZONTAL_ORANGE_T2:
            light_set(ID_TL2_RED, COLOR_ON);    light_set(ID_TL4_RED, COLOR_ON);
            light_set(ID_TL1_YELLOW, COLOR_ON); light_set(ID_TL3_YELLOW, COLOR_ON);
            if (now - stateStartTime_T2 >= orangeDelay) {
                stateStartTime_T2 = now;
                current_state_T2 = STATE_HORIZONTAL_GREEN_T2;
            }
            break;

        case STATE_HORIZONTAL_GREEN_T2:
            light_set(ID_TL1_GREEN, COLOR_ON); light_set(ID_TL3_GREEN, COLOR_ON);
            light_set(ID_TL2_RED, COLOR_ON);   light_set(ID_TL4_RED, COLOR_ON);

            // If a Vertical car is waiting at the red light
            if (vertical_active) {
                if (redWaitStartTime_T2 == 0) redWaitStartTime_T2 = now;

                // R2.7: Immediate change if Horizontal is empty
                // R2.6: Force change if Vertical has waited longer than redDelayMax
                if (!horizontal_active || (now - redWaitStartTime_T2 >= redDelayMax)) {
                    stateStartTime_T2 = now;
                    redWaitStartTime_T2 = 0;
                    current_state_T2 = STATE_HORIZONTAL_ORANGE_FINISH_T2;
                }
            } else {
                redWaitStartTime_T2 = 0;
                // R2.4: Return to Vertical if Horizontal is empty and greenDelay passed
                // Also return if a pedestrian is waiting (ped_waiting should force return to vertical)
                if ((!horizontal_active && (now - stateStartTime_T2 >= greenDelay)) || ped_waiting) {
                    stateStartTime_T2 = now;
                    current_state_T2 = STATE_HORIZONTAL_ORANGE_FINISH_T2;
                }
            }
            break;

        case STATE_HORIZONTAL_ORANGE_FINISH_T2:
            light_set(ID_TL1_YELLOW, COLOR_ON); light_set(ID_TL3_YELLOW, COLOR_ON);
            light_set(ID_TL2_RED, COLOR_ON);    light_set(ID_TL4_RED, COLOR_ON);
            if (now - stateStartTime_T2 >= orangeDelay) {
                stateStartTime_T2 = now;
                current_state_T2 = STATE_VERTICAL_ORANGE_PREPARE_T2;
            }
            break;

        case STATE_VERTICAL_ORANGE_PREPARE_T2:
            light_set(ID_TL1_RED, COLOR_ON);    light_set(ID_TL3_RED, COLOR_ON);
            light_set(ID_TL2_YELLOW, COLOR_ON); light_set(ID_TL4_YELLOW, COLOR_ON);
            if (now - stateStartTime_T2 >= orangeDelay) {
                stateStartTime_T2 = now;
                current_state_T2 = STATE_VERTICAL_GREEN_T2;
            }
            break;
    }
}
void traffic_control(){
	// Initializations
	current_state = STATE_IDLE_T1;
	current_state_T2 = STATE_VERTICAL_GREEN_T2;
	uint32_t now = HAL_GetTick();
	stateStartTime = now;
	stateStartTime_T2 = now;

	while (1) {
		now = HAL_GetTick();
		// Reset light and poll inputs
		lights_reset();
	    Debounce_Button_Inputs();
	    Debounce_Switch_Inputs();

	    // Run Task 2 Logic (Car light logic)
	    task2_logic(now);

	    // Run Task 1 Logic (PED light logic)
	    task1_logic(now);

	    Output_Lights();
	}
}

