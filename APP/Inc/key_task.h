#ifndef __KEY_TASK_H__
#define __KEY_TASK_H__

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

void Key_Task_Init(void);
void Key_Task(void *pvParameters);


#endif

