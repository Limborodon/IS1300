/**
******************************************************************************
@brief traffic_control_functions, file for project_functions.c file.
@file traffic_control_functions.c
@author Linus Wennergren
@date 8-December-2025
******************************************************************************
*/
#include "main.h"
/**
******************************************************************************
@brief Shift_Out_24, Shifts out 24 bits to the shift registers for pedestrian and traffic lights.
@author Linus Wennergren
@date 8-December-2025
******************************************************************************
*/
void Shift_Out_24(uint8_t byte_U3, uint8_t byte_U2, uint8_t byte_U1) {
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
