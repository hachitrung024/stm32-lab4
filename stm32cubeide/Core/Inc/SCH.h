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
unsigned char SCH_Add_Task (void (* pFn)(),unsigned int DELAY,
		unsigned int PERIOD);
void SCH_Dispatch_Task(void);
unsigned char SCH_Delete_Task(unsigned char TASK_INDEX);
#endif /* INC_SCH_H_ */
