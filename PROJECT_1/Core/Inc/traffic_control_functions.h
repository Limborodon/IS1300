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

/** @name Button and Sensor Logic Levels
 * @{ */
#define BUTTON_PRESSED   GPIO_PIN_RESET /**< When button is pushed. */
#define BUTTON_RELEASED  GPIO_PIN_SET /**< When button is released. */
#define NO_CAR_PRESET    GPIO_PIN_RESET /**< When no car is detected. */
#define CAR_PRESET       GPIO_PIN_SET /**< When car is detected. */
/** @} */


/** @name LED Identifiers
 * Used as the led_id parameter
 * @{ */
/* U1*/
#define ID_TL1_RED        1
#define ID_TL1_YELLOW     2
#define ID_TL1_GREEN      3
#define ID_PL1_RED        4
#define ID_PL1_GREEN      5
#define ID_PL1_BLUE       6
/* U2*/
#define ID_TL2_RED        7
#define ID_TL2_YELLOW     8
#define ID_TL2_GREEN      9
#define ID_PL2_RED        10
#define ID_PL2_GREEN      11
#define ID_PL2_BLUE       12

/* U3 */
#define ID_TL3_RED        13
#define ID_TL3_YELLOW     14
#define ID_TL3_GREEN      15

#define ID_TL4_RED        16
#define ID_TL4_YELLOW     17
#define ID_TL4_GREEN      18

/** @name Color Codes
 * @{ */
#define COLOR_ON  1
#define COLOR_OFF 0
/** @} */

/** @name Pedestrian button states
 * @{ */
extern GPIO_PinState PL1_Debounced_State;
extern GPIO_PinState PL2_Debounced_State;
/** @} */

/** @brief Updates the shift registers with current global light states. */
void Output_Lights();

/** @brief Polls and debounces all pedestrian button inputs. */
void Debounce_Button_Inputs();

/** @brief Polls and debounces all car sensor switch inputs. */
void Debounce_Switch_Inputs();

/**
 * @brief  Sets a specific LED state
 * @param  led_id The ID of the LED (1-18)
 * @param  color_code COLOR_ON or COLOR_OFF
 */
void light_set(uint8_t led_id, uint8_t color_code);

/** @brief Resets all virtual light states to off. */
void lights_reset();

/**
 * @brief  Checks if a car is detected
 * @param  car_number (1-4).
 * @return true if car is present.
 */
bool Car_Present(uint8_t car_number);

/** @brief Returns true if any vertical car sensors are active. */
bool Vertical_Car_Sensor_Active();

/** @brief Returns true if any horizontal car sensors are active. */
bool Horizontal_Car_Sensor_Active();

/** @brief Checks the debounced state of the upper pedestrian button. */
bool Upper_Pedestrian_Button_Pressed();

/** @brief Checks the debounced state of the lower pedestrian button. */
bool Lower_Pedestrian_Button_Pressed();



#endif /* INC_TRAFFIC_CONTROL_FUNCTIONS_H_ */


