/**
******************************************************************************
@brief file for the implementation tasks
@file traffic_control.c
@author Linus Wennergren, George Yandem
@date 19-December-2025
******************************************************************************
*/
#include<main.h>

#include "traffic_control_functions.h"
/**
 * @brief States for the Pedestrian Traffic Light.
 */
typedef enum {
    STATE_IDLE_T1, /**< PED light Red, waiting for button press. */
    STATE_WAITING_T1, /**< Button pressed, Blue light blinking, waiting for car red light. */
    STATE_WALKING_T1 /**< PED light is Green, it is safe to cross. */
} TrafficState_t_T1;

/** @brief Current state of the PED Traffic Light */
TrafficState_t_T1 current_state = STATE_IDLE_T1;

/**
 * @brief States for the Car Traffic Lights.
 */
typedef enum {
    STATE_VERTICAL_GREEN_T2, /**< Vert. lane is Green, Hori. lane is Red. */
    STATE_VERTICAL_ORANGE_T2, /**< Vert. switching from Green to Red. */
    STATE_HORIZONTAL_GREEN_T2, /**< Hori. lane is Green, Vert. is Red. */
    STATE_HORIZONTAL_ORANGE_T2, /**< Hori. switching Red to Green . */
	STATE_VERTICAL_ORANGE_PREPARE_T2, /**< Vert. switching Red to Green. */
	STATE_HORIZONTAL_ORANGE_FINISH_T2 /**< Hori. switching Green to Red. */

} TrafficState_t_T2;

/** @brief Current state of the vehicle intersection */
TrafficState_t_T2 current_state_T2 = STATE_VERTICAL_GREEN_T2;


/** @name Timing and States
 * Variables used for time-tracking and hardware state.
 * @{ */
uint32_t stateStartTime = 0;
uint32_t lastBlinkTime  = 0;
uint8_t indicatorState  = 0;

uint32_t stateStartTime_T2 = 0;
uint32_t redWaitStartTime_T2 = 0;
/** @} */

/** @name Configuration Constants for tasks
 * Timing delays defined in milliseconds.
 * @{ */
uint32_t toggleFreq      = 300;
uint32_t pedestrianDelay = 4000;
uint32_t walkingDelay    = 5000;
uint32_t orangeDelay     = 2000;
uint32_t greenDelay      = 5000;
uint32_t redDelayMax     = 6000;
/** @} */

/**
 * @brief  Logic for Pedestrian Crossing.
 * @param  now Current system time in milliseconds.
 */
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

/**
 * @brief  Logic for Vehicle Intersection .
 * @param  now Current system time in milliseconds.
 */
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


                if (!vertical_active || (now - redWaitStartTime_T2 >= redDelayMax)) {
                    stateStartTime_T2 = now;
                    redWaitStartTime_T2 = 0;
                    current_state_T2 = STATE_VERTICAL_ORANGE_T2;
                }
            } else {
                redWaitStartTime_T2 = 0;

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

                if (!horizontal_active || (now - redWaitStartTime_T2 >= redDelayMax)) {
                    stateStartTime_T2 = now;
                    redWaitStartTime_T2 = 0;
                    current_state_T2 = STATE_HORIZONTAL_ORANGE_FINISH_T2;
                }
            } else {
                redWaitStartTime_T2 = 0;
                //Return to Vertical if Horizontal is empty and greenDelay passed
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

/**
 * @brief  Main entry point for traffic control.
 * @details Initializes the state machines and runs the Statemachine logic and Outputs lights
 */
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

	    // Run Task 2 Logic
	    task2_logic(now);

	    // Run Task 1 Logic
	    task1_logic(now);

	    Output_Lights();
	}
}

