/*
 * SCH.h
 *
 *  Created on: Nov 5, 2024
 *      Author: hachi
 */

#ifndef INC_SCH_H_
#define INC_SCH_H_
#include "main.h"
void SCH_Init(void);
void SCH_Update(void);
void SCH_Dispatch_Tasks(void);
uint32_t SCH_Add_Task(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD);
uint8_t SCH_Delete_Task(uint32_t taskID);
#endif /* INC_SCH_H_ */
