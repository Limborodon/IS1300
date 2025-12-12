/**
******************************************************************************
@brief traffic_control_functions, file for project_functions.c file.
@file traffic_control_functions.c
@author Linus Wennergren
@date 8-December-2025
******************************************************************************
*/
#include "main.h"
#include "traffic_control_functions.h"
/**
******************************************************************************
@brief Shift_Out_24, Shifts out 24 bits to the shift registers for pedestrian and traffic lights.
@author Linus Wennergren
@date 8-December-2025
******************************************************************************
*/


#define DEBOUNCE_DELAY (5) // ms
// For PED button
uint8_t PL1_Debounced_State = BUTTON_RELEASED;
uint8_t PL2_Debounced_State = BUTTON_RELEASED;

uint32_t PL1_Last_Change_Time = 0;
uint32_t PL2_Last_Change_Time = 0;

//function for outputing lights uses the shift register to shift in bits with SHCP and then shows with the Latch(STCP) lights up the LEDs.
void Output_Lights(uint8_t byte_U3, uint8_t byte_U2, uint8_t byte_U1) {
    // Combine the three bytes
    uint32_t data = ((uint32_t)byte_U3 << 16) | ((uint32_t)byte_U2 << 8) | byte_U1;

    // Latch LOW
    HAL_GPIO_WritePin(STCP_GPIO_Port, STCP_Pin, GPIO_PIN_RESET);

    // Transmit 24 bits
    for (int i = 0; i < 24; i++) {
        if ((data >> (23 - i)) & 0x01) {
            HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(DS_GPIO_Port, DS_Pin, GPIO_PIN_RESET);
        }

        // Clock Pulse
        HAL_GPIO_WritePin(SHCP_GPIO_Port, SHCP_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(SHCP_GPIO_Port, SHCP_Pin, GPIO_PIN_RESET);
    }

    // Latch HIGH
    HAL_GPIO_WritePin(STCP_GPIO_Port, STCP_Pin, GPIO_PIN_SET);
}

// Debounces Button inputs with a 5ms debounce delay. Not 100% necessary because we only care about the first button press.
// The solution is good for rejecting electrical noise in a larger syste and does not impact it much.
void Debounce_Button_Inputs() {
    uint32_t current_time = HAL_GetTick();
    uint8_t PL1_reading = HAL_GPIO_ReadPin(PL1_Switch_GPIO_Port, PL1_Switch_Pin);
    uint8_t PL2_reading = HAL_GPIO_ReadPin(PL2_Switch_GPIO_Port, PL2_Switch_Pin);

    if (PL1_reading != PL1_Debounced_State) {
        if (current_time - PL1_Last_Change_Time >= DEBOUNCE_DELAY) {
            PL1_Debounced_State = PL1_reading;
        }
    } else {
        PL1_Last_Change_Time = current_time;
    }
    if (PL2_reading != PL2_Debounced_State) {
            if (current_time - PL2_Last_Change_Time >= DEBOUNCE_DELAY) {
                PL2_Debounced_State = PL2_reading;
            }
        } else {
            PL2_Last_Change_Time = current_time;
        }
}

uint8_t Upper_Pedestrian_Button_Pressed(void) {
    return (PL2_Debounced_State == BUTTON_PRESSED);
}

uint8_t Lower_Pedestrian_Button_Pressed(void) {
    return (PL1_Debounced_State == BUTTON_PRESSED);
}









