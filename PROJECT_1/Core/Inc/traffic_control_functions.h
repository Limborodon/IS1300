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


#define BUTTON_PRESSED   GPIO_PIN_RESET // Button pressed
#define BUTTON_RELEASED  GPIO_PIN_SET   // Button released

extern uint8_t PL1_Debounced_State;
extern uint8_t PL2_Debounced_State;


void Output_Lights(uint8_t byte_U3, uint8_t byte_U2, uint8_t byte_U1);
void Debounce_Button_Inputs();
uint8_t Upper_Pedestrian_Button_Pressed();
uint8_t Lower_Pedestrian_Button_Pressed();

#endif /* INC_TRAFFIC_CONTROL_FUNCTIONS_H_ */


