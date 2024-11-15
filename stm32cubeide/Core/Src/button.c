/*
 * button.c
 *
 *  Created on: Nov 1, 2024
 *      Author: hachi
 */

#include "button.h"
static GPIO_TypeDef * button_ports[MAX_BUTTONS] ={BT0_GPIO_Port,BT1_GPIO_Port,BT2_GPIO_Port};
static uint16_t button_pins[MAX_BUTTONS] = {BT0_Pin,BT1_Pin,BT2_Pin};

static GPIO_PinState button_buffers0[MAX_BUTTONS];
static GPIO_PinState button_buffers1[MAX_BUTTONS];
static GPIO_PinState button_buffers2[MAX_BUTTONS];
static GPIO_PinState button_buffers3[MAX_BUTTONS];

static uint8_t button_press_flags[MAX_BUTTONS];
static uint8_t button_hold_flags[MAX_BUTTONS];

static uint16_t button_hold_counters[MAX_BUTTONS];
void buttonRun(){
	for(uint8_t i=0; i < MAX_BUTTONS; i++){
		//BEGIN for
		button_buffers0[i] = button_buffers1[i];
		button_buffers1[i] = button_buffers2[i];
		button_buffers2[i] = HAL_GPIO_ReadPin( button_ports[i], button_pins[i]);
		if((button_buffers0[i] == button_buffers1[i]) && (button_buffers1[i] == button_buffers2[i])){
			//BEGIN stable condition
			if(button_buffers2[i] != button_buffers3[i]){
				//BEGIN trigger edge
				button_buffers3[i] = button_buffers2[i];
				if(button_buffers2[i]==BUTTON_PRESSED){
					//Edge down
					button_press_flags[i] = 1;
					button_hold_counters[i] = BUTTON_HOLD_DUR;
				}
				//END trigger edge
			}else if(button_buffers3[i] == BUTTON_PRESSED){
				//Stable pressed condition
				button_hold_counters[i]--;
				if(button_hold_counters[i]==0) button_hold_flags[i] = 1;
			}
			//END stable condition
		} else{
			button_hold_flags[i] = 0;
		}
		//END for
	}
}
uint8_t isButtonPressed(uint8_t index){
	if(index >= MAX_BUTTONS) return 0;
	if(button_press_flags[index] == 1){
		button_press_flags[index] = 0;
		return 1;
	}
	return 0;
}
uint8_t isButtonHolding(uint8_t index){
	if(index >= MAX_BUTTONS) return 0;
	return button_hold_flags[index];
}
