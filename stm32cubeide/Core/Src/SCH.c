/*
 * SCH.c
 *
 *  Created on: Nov 5, 2024
 *      Author: hachi
 */
#include "SCH.h"

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

#define MAX_TASKS 10  // Số lượng task tối đa trong scheduler

typedef struct {
    void (*pFunction)();  // Con trỏ hàm đến công việc
    uint32_t delay;       // Thời gian chờ để chạy task lần đầu tiên
    uint32_t period;      // Chu kỳ chạy lại task
    uint8_t runMe;       // Biến đếm số lần cần chạy task
} sTask;

sTask SCH_Tasks[MAX_TASKS];
uint32_t taskCounter = 0;  // Bộ đếm ID task

void SCH_Update(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (SCH_Tasks[i].pFunction) {
            if (SCH_Tasks[i].delay == 0) {
                // Khi delay đã hết, đặt cờ để chạy task
                SCH_Tasks[i].runMe += 1;

                // Đặt lại delay cho task nếu nó có chu kỳ lặp
                if (SCH_Tasks[i].period) {
                    SCH_Tasks[i].delay = SCH_Tasks[i].period;
                }
            } else {
                // Giảm delay của task
                SCH_Tasks[i].delay--;
            }
        }
    }
}

void SCH_Dispatch_Tasks(void) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (SCH_Tasks[i].runMe > 0) {
            (*SCH_Tasks[i].pFunction)();  // Chạy task
            SCH_Tasks[i].runMe -= 1;

            // Nếu task không có chu kỳ (chỉ chạy 1 lần), xóa task khỏi hàng đợi
            if (SCH_Tasks[i].period == 0) {
                SCH_Delete_Task(i);
            }
        }
    }
}

uint32_t SCH_Add_Task(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD) {
    for (int i = 0; i < MAX_TASKS; i++) {
        if (!SCH_Tasks[i].pFunction) {  // Tìm vị trí trống
            SCH_Tasks[i].pFunction = pFunction;
            SCH_Tasks[i].delay = DELAY;
            SCH_Tasks[i].period = PERIOD;
            SCH_Tasks[i].runMe = 0;

            return i;  // Trả về ID của task
        }
    }
    return MAX_TASKS;  // Hàng đợi đầy
}

uint8_t SCH_Delete_Task(uint32_t taskID) {
    if (taskID < MAX_TASKS && SCH_Tasks[taskID].pFunction) {
        SCH_Tasks[taskID].pFunction = NULL;
        SCH_Tasks[taskID].delay = 0;
        SCH_Tasks[taskID].period = 0;
        SCH_Tasks[taskID].runMe = 0;
        return 1;  // Xóa thành công
    }
    return 0;  // Không thành công
}


