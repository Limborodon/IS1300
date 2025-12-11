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

//U1
#define TL1_RED        (1 << 0)
#define TL1_YELLOW     (1 << 1)
#define TL1_GREEN      (1 << 2)
#define PL1_RED        (1 << 3)
#define PL1_GREEN      (1 << 4)
#define PL1_BLUE       (1 << 5)

// U2
#define TL2_RED        (1 << 0)
#define TL2_YELLOW     (1 << 1)
#define TL2_GREEN      (1 << 2)
#define PL2_RED        (1 << 3)
#define PL2_GREEN      (1 << 4)
#define PL2_BLUE       (1 << 5)

// U3
#define TL3_RED        (1 << 0)
#define TL3_YELLOW     (1 << 1)
#define TL3_GREEN      (1 << 2)

#define TL4_RED        (1 << 3)
#define TL4_YELLOW     (1 << 4)
#define TL4_GREEN      (1 << 5)

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

uint32_t toggleFreq      = 300;
uint32_t pedestrianDelay = 4000;
uint32_t walkingDelay    = 5000;
uint32_t orangeDelay     = 2000;

static uint8_t Upper_Pedestrian_Button_Pressed(void) {
    return (PL2_Debounced_State == GPIO_PIN_RESET);
}

static uint8_t Lower_Pedestrian_Button_Pressed(void) {
    return (PL1_Debounced_State == GPIO_PIN_RESET);
}

//helper
static void Output_Lights(uint8_t u3, uint8_t u2, uint8_t u1) {
    Shift_Out_24(u3, u2, u1);
}



void task1(void)
{
	while (1)
	    {
	        uint32_t now = HAL_GetTick();

	        // Default
	        uint8_t byte_U1 = 0; //TL1 & PL1
	        uint8_t byte_U2 = 0; //TL2 & PL2
	        uint8_t byte_U3 = 0; //TL3 & TL4
	        Debounce_Button_Inputs();

	        switch (current_state)
	        {
	        case STATE_IDLE_T1:
	            byte_U1 = 0;
	            byte_U2 = PL2_RED | TL2_GREEN;
	            byte_U3 = TL4_GREEN;

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

	            byte_U1 = 0;
	            byte_U2 = TL2_GREEN | PL2_RED | (indicatorState ? PL2_BLUE : 0);
	            byte_U3 = TL4_GREEN;

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
	            byte_U1 = 0;
	            byte_U2 = TL2_YELLOW | PL2_RED | (indicatorState ? PL2_BLUE : 0);
	            byte_U3 = TL4_YELLOW;

	            if (now - stateStartTime >= orangeDelay) {
	                stateStartTime = now;
	                current_state = STATE_PED_GREEN_T1;
	            }
	            break;

	        case STATE_PED_GREEN_T1:
	            byte_U1 = 0;
	            byte_U2 = TL2_RED | PL2_GREEN;
	            byte_U3 = TL4_RED;

	            if (now - stateStartTime >= walkingDelay) {
	                current_state = STATE_PED_GREEN_END_T1;
	            }
	            break;

	        case STATE_PED_GREEN_END_T1:
	            byte_U1 = 0;
	            byte_U2 = TL2_RED | PL2_RED;
	            byte_U3 = TL4_RED;

	            stateStartTime = now;
	            current_state = STATE_RETURN_ORANGE_T1;
	            break;

	        case STATE_RETURN_ORANGE_T1:
	            byte_U1 = 0;
	            byte_U2 = TL2_YELLOW | PL2_RED;
	            byte_U3 = TL4_YELLOW;

	            if (now - stateStartTime >= orangeDelay) {
	                current_state = STATE_IDLE_T1;
	            }
	            break;
	        }

	        // Output
	        Output_Lights(byte_U3, byte_U2, byte_U1);
	    }
}
void traffic_control(){
	task1();
}

