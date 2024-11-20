/*
 * SCH.c
 *
 *  Created on: Nov 5, 2024
 *      Author: hachi
 */
#include "SCH.h"

enum {
	ERROR_SCH_TOO_MANY_TASKS,
	ERROR_SCH_CANNOT_DELETE_TASK
} errorCode;

#define MAX_TASKS 40

//Cau truc cua 1 Task
typedef struct sTask {
	void (*pFunction)(); 	//Con tro ham
	uint32_t delay;			//Delay truoc khi chay
	uint32_t period;		//Chu ky lap (tick)
	uint8_t runMe;			//So lan can chay task nay
	uint32_t taskID;		//ID cua task
	struct sTask * next;	//Con tro next
} sTask;

//Hien thuc bang Linked List
static sTask * head = NULL;	//Danh sach task
static uint32_t count = 0; 	//Dem so tang dan cho task ID

//Hien thuc cap phat tinh
static sTask data[MAX_TASKS]; //Cap phat tinh cho sch
static sTask * freeList = NULL;//Danh sach bo nho trong

//Lay tu freeList 1 vung nho de cap phat
static sTask * sTaskAlloc(){
	if(freeList == NULL) return NULL;
	sTask * res = freeList;
	freeList = freeList->next;
	res->next = NULL;
	return res;
}

//Giai phong 1 Task, dua vao freeList
static void sTaskFree(sTask * target){
	if(target==NULL) return;
	target->pFunction = NULL;
	target->delay = 0;
	target->period = 0;
	target->runMe = 0;
	target->taskID = 0;
	target->next = freeList;
	freeList = target;
	return;
}
//Khoi tao sch
void SCH_Init(){
	int i;
	for(i = 0; i < MAX_TASKS; i++){
		data[i].next = freeList;
		freeList = &data[i];
	}
}
//Ham ho tro cho add task vao Linked List
static void addtolist(sTask * newTask){
	if(head == NULL || head->delay > newTask->delay){
		if(head != NULL) head->delay -= newTask->delay;
		newTask->next = head;
		head = newTask;
	} else {
		sTask * current = head;
		sTask * prev = NULL;
		while (current != NULL && current->delay <= newTask->delay) {
			if(current->delay == newTask->delay) newTask->delay++;
			newTask->delay -= current->delay;
			prev = current;
			current = current->next;
		}

		if (prev == NULL) {
			newTask->next = head;
			head = newTask;
		} else {
			newTask->next = prev->next;
			prev->next = newTask;
		}
		if (newTask->next != NULL) {
			newTask->next->delay -= newTask->delay;
		}
	}
}
//Them mot task vao sch
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
	addtolist(newTask);
	return newTask->taskID;
}

void SCH_Update(){
	if(head == NULL) return;
	if(head->delay > 0){
		head->delay -- ;
	}
	if(head->delay == 0){
		head->runMe += 1;
	}
}

void SCH_Dispatch_Tasks(void) {
	while(head->delay <= 0){
		if(head->period <= 0){
			sTask * endTask = head;
			head = head->next;
			(*endTask->pFunction)();
			sTaskFree(endTask);
		}else {
			sTask * endTask = head;
			endTask->delay = endTask->period;
			head = head->next;
			addtolist(endTask);
		}
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
	if(head == NULL) return 1;
	sTask * current = head;
	sTask * prev = NULL;
	while (current != NULL && current->taskID != taskID) {
		prev = current;
		current = current->next;
	}
	if(current == NULL) return -1;
	if (prev == NULL) {
		head = current->next;
	}else{
		prev->next = current->next;
	}
	if(current->next != NULL){
		current->next->delay += current->delay;
	}
	sTaskFree(current);
	return 0;
}

