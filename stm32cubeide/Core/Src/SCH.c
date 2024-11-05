/*
 * SCH.c
 *
 *  Created on: Nov 5, 2024
 *      Author: hachi
 */
#include "SCH.h"
typedef struct {
	void (* pTask) (void);
	uint32_t Delay;
	uint32_t Period;
	uint8_t RunMe;
	uint32_t TaskID;
} sTask;
enum {
	ERROR_SCH_TOO_MANY_TASKS,
	ERROR_SCH_WAITING_FOR_SLAVE_TO_ACK,
	ERROR_SCH_WAITING_FOR_START_COMMAND_FROM_MASTER,
	ERROR_SCH_ONE_OR_MORE_SLAVES_DID_NOT_START,
	ERROR_SCH_LOST_SLAVE,
	ERROR_SCH_CAN_BUS_ERROR,
	ERROR_I2C_WRITE_BYTE_AT24C64,
	ERROR_SCH_CANNOT_DELETE_TASK
} errorCode;
#define SCH_MAX_TASKS	40
#define NO_TASK_ID		0
sTask SCH_tasks_G[SCH_MAX_TASKS];
static uint8_t Error_code_G = 0;

sTask test;

void SCH_Init(void) {
	SCH_Delete_Task(0);
	Error_code_G = 0;
}

void SCH_Update(void){
	test.Delay--;
	if(test.Delay==0){
		test.Delay=test.Period;
		test.RunMe = 1;
	}
}
unsigned char SCH_Add_Task (void (* pFn)(),unsigned int DELAY,
		unsigned int PERIOD){
	test.pTask = pFn;
	test.Delay = DELAY;
	test.Period = PERIOD;
	test.RunMe = 0;
	return 0;
}
void SCH_Dispatch_Task(void){
	if(test.RunMe == 1){
		(*test.pTask)();
		test.RunMe=0;
	}
}

unsigned char SCH_Delete_Task(unsigned char Index){
	test.pTask = 0x0000;
	test.Delay = 0;
	test.Period = 0;
	test.RunMe = 0;
	return 0;
}


