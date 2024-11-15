/*
 * button.h
 *
 *  Created on: Nov 1, 2024
 *      Author: hachi
 */

#ifndef INC_BUTTON_H_
#define INC_BUTTON_H_

#include "main.h"

#define MAX_BUTTONS 		3
#define BUTTON_RELEASED		GPIO_PIN_SET
#define BUTTON_PRESSED		GPIO_PIN_RESET
#define BUTTON_HOLD_DUR		100
void buttonRun();
uint8_t isButtonPressed(uint8_t index);
uint8_t isButtonHolding(uint8_t index);

#endif /* INC_BUTTON_H_ */
