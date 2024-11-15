/*
 * SCH.c
 *
 *  Created on: Nov 5, 2024
 *      Author: hachi
 */
#include "SCH.h"
extern UART_HandleTypeDef huart2;
uint32_t current_time = 0;
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

#define MAX_TASKS 40

typedef struct sTask {
	void (*pFunction)(); 	//Con tro ham
	uint32_t delay;			//Delay truoc khi chay
	uint32_t period;		//Chu ky lap (tick)
	uint8_t runMe;			//So lan can chay task nay
	uint32_t taskID;		//ID cua task
	struct sTask * next;
} sTask;

static sTask * head = NULL;	//Danh sach task
static uint32_t count = 0; 	//Dem so tang dan cho task ID
//Cap phat tinh
static sTask * freeList = NULL;
static sTask mem[MAX_TASKS];
static sTask * sTaskAlloc(){
	if(freeList == NULL) return NULL;
	sTask * res = freeList;
	freeList = freeList->next;
	res->next = NULL;
	return res;
}
static void sTaskFree(sTask * target){
	if(target == NULL) return;
	target->pFunction = NULL;
	target->delay = 0;
	target->period = 0;
	target->runMe = 0;
	target->taskID = 0;
	target->next = freeList;
	freeList = target;
}
void SCH_Init(){
	for(int i = 0; i< MAX_TASKS; i++){
		mem[i].next = freeList;
		freeList = &mem[i];
	}
}

uint32_t SCH_Add_Task(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD){
//	sTask * newTask = (sTask*)malloc(sizeof(sTask));
	sTask * newTask = sTaskAlloc();
	if (newTask == NULL) {
		return count;
	}
	newTask->pFunction = pFunction;
	newTask->delay = DELAY;
	newTask->period = PERIOD;
	newTask->runMe = 0;
	newTask->taskID = count;
	newTask->next = NULL;
	count++;
	//Neu them moi hoac them vao dau
	if(head == NULL || head->delay > newTask->delay){
		if(head != NULL) head->delay -= newTask->delay;
		newTask->next = head;
		head = newTask;
	} else {
		sTask * current = head;
		newTask->delay -= current->delay;
		//Tim kiem vi tri phu hop
		while(current->next != NULL && current->next->delay <= newTask->delay){
			newTask->delay -= current->next->delay;
			current = current->next;
		}
		//Chen vao vi tri cua task do
		newTask->next = current->next;
		current->next = newTask;
		if(newTask->next != NULL) newTask->next->delay -= DELAY;

	}
	return newTask->taskID;
}
static void clearHead(){
	if(head->period <= 0){
		sTask * endTask = head;
		head = head->next;
		(*endTask->pFunction)();
		sTaskFree(endTask);
		return;
	}
	sTask * endTask = head;
	head = head -> next;
	endTask->delay = endTask->period;

	sTask * current = head;
	sTask * prev = NULL;
	while (current != NULL && current->delay <= endTask->delay) {
		endTask->delay -= current->delay;
		prev = current;
		current = current->next;
	}

	if (prev == NULL) {
		endTask->next = head;
		head = endTask;
	} else {
		endTask->next = prev->next;
		prev->next = endTask;
	}
	if (endTask->next != NULL) {
		endTask->next->delay -= endTask->delay;
	}
}
void SCH_Update(){
	if(head == NULL) return;
	if(head->delay > 0){
		head->delay -- ;
	}
	//Danh dau va xu li ca cac task co delay = 0
	sTask * current = head;
	while (current != NULL && current->delay==0){
		head->runMe += 1;
		current = current->next;
	}
}
void SCH_Dispatch_Tasks(void) {
	while (head != NULL && head->delay==0){
	    clearHead();
	}
    sTask *current = head;
    while (current != NULL) {
        if (current->runMe > 0) {
            (*current->pFunction)();
            current->runMe -= 1;
        }
        current = current->next;
    }
}
uint8_t SCH_Delete_Task(uint32_t taskID){
	sTask * current = head;
	sTask * prev = NULL;
	while (current != NULL && current->taskID != taskID) {
		prev = current;
		current = current->next;
	}
	if(current == NULL) return -1;
	if(current->next != NULL){
		current->next->delay += current->delay;
	}
	if (prev == NULL) {
		head = current->next;
	} else {
		prev->next = current->next;
	}
	sTaskFree(current);
	return 0;
}


