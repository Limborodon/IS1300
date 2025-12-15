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


#define DEBOUNCE_DELAY (5) // ms

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





// Global PED button state variables
uint8_t PL1_Debounced_State = BUTTON_RELEASED;
uint8_t PL2_Debounced_State = BUTTON_RELEASED;

uint32_t PL1_Last_Change_Time = 0;
uint32_t PL2_Last_Change_Time = 0;

// Global state variables for the lights/shift register
// Volatile in case we use want to use RTOS
volatile uint8_t current_U1_state;
volatile uint8_t current_U2_state;
volatile uint8_t current_U3_state;

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

void Output_Lights(){
	Shift_Out_24(current_U3_state,current_U2_state,current_U1_state);
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

bool Upper_Pedestrian_Button_Pressed(void) {
    return (PL2_Debounced_State == BUTTON_PRESSED);
}

bool Lower_Pedestrian_Button_Pressed(void) {
    return (PL1_Debounced_State == BUTTON_PRESSED);
}

void light_set(uint8_t led_id, uint8_t color_code) {
    uint8_t bitmask = 0;
    volatile uint8_t *state_ptr = NULL; // Pointer to U1, U2, or U3 state used for updating without loss of information. volatile for safety.

    // Determine the target led and bitmask
    switch (led_id) {
        // U1
        case ID_TL1_RED:    bitmask = TL1_RED;    state_ptr = &current_U1_state; break;
        case ID_TL1_YELLOW: bitmask = TL1_YELLOW; state_ptr = &current_U1_state; break;
        case ID_TL1_GREEN:  bitmask = TL1_GREEN;  state_ptr = &current_U1_state; break;
        case ID_PL1_RED:    bitmask = PL1_RED;    state_ptr = &current_U1_state; break;
        case ID_PL1_GREEN:  bitmask = PL1_GREEN;  state_ptr = &current_U1_state; break;
        case ID_PL1_BLUE:   bitmask = PL1_BLUE;   state_ptr = &current_U1_state; break;

        // U2
        case ID_TL2_RED:    bitmask = TL2_RED;    state_ptr = &current_U2_state; break;
        case ID_TL2_YELLOW: bitmask = TL2_YELLOW; state_ptr = &current_U2_state; break;
        case ID_TL2_GREEN:  bitmask = TL2_GREEN;  state_ptr = &current_U2_state; break;
        case ID_PL2_RED:    bitmask = PL2_RED;    state_ptr = &current_U2_state; break;
        case ID_PL2_GREEN:  bitmask = PL2_GREEN;  state_ptr = &current_U2_state; break;
        case ID_PL2_BLUE:   bitmask = PL2_BLUE;   state_ptr = &current_U2_state; break;

        // U3
        case ID_TL3_RED:    bitmask = TL3_RED;    state_ptr = &current_U3_state; break;
        case ID_TL3_YELLOW: bitmask = TL3_YELLOW; state_ptr = &current_U3_state; break;
        case ID_TL3_GREEN:  bitmask = TL3_GREEN;  state_ptr = &current_U3_state; break;
        case ID_TL4_RED:    bitmask = TL4_RED;    state_ptr = &current_U3_state; break;
        case ID_TL4_YELLOW: bitmask = TL4_YELLOW; state_ptr = &current_U3_state; break;
        case ID_TL4_GREEN:  bitmask = TL4_GREEN;  state_ptr = &current_U3_state; break;

        default:
            return; // Invalid
    }

    if (state_ptr == NULL) return; // Should not happen, but just in case it does.

    // Set or clear light depending on color_code value.
    if (color_code == COLOR_ON) {
        *state_ptr |= bitmask;  // Turn on the LED
    } else {
        *state_ptr &= ~bitmask; // Turn off the LED
    }
}

void lights_reset(){
	current_U1_state = 0;
	current_U2_state = 0;
	current_U3_state = 0;
}




