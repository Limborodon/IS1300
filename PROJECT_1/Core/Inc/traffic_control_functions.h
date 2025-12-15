/**
******************************************************************************
@brief traffic_control_functions, header file for traffic_control_functions.c file.
@file traffic_control_functions.h
@author Linus Wennergren
@date 8-December-2025
******************************************************************************
*/
#ifndef INC_TRAFFIC_CONTROL_FUNCTIONS_H_
#define INC_TRAFFIC_CONTROL_FUNCTIONS_H_

#include<stdbool.h>

#define BUTTON_PRESSED   GPIO_PIN_RESET // Button pressed
#define BUTTON_RELEASED  GPIO_PIN_SET   // Button released

// LED IDs
//U1
#define ID_TL1_RED        1
#define ID_TL1_YELLOW     2
#define ID_TL1_GREEN      3
#define ID_PL1_RED        4
#define ID_PL1_GREEN      5
#define ID_PL1_BLUE       6
//U2
#define ID_TL2_RED        7
#define ID_TL2_YELLOW     8
#define ID_TL2_GREEN      9
#define ID_PL2_RED        10
#define ID_PL2_GREEN      11
#define ID_PL2_BLUE       12

//U3
#define ID_TL3_RED        13
#define ID_TL3_YELLOW     14
#define ID_TL3_GREEN      15

#define ID_TL4_RED        16
#define ID_TL4_YELLOW     17
#define ID_TL4_GREEN      18

// Color Codes for turning off or on
#define COLOR_ON  1
#define COLOR_OFF 0

extern uint8_t PL1_Debounced_State;
extern uint8_t PL2_Debounced_State;



void Output_Lights();
void Debounce_Button_Inputs();
void light_set(uint8_t led_id, uint8_t color_code);
void lights_reset();

bool Upper_Pedestrian_Button_Pressed();
bool Lower_Pedestrian_Button_Pressed();

#endif /* INC_TRAFFIC_CONTROL_FUNCTIONS_H_ */


