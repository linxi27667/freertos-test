#ifndef __SERIAL_TASK_H__
#define __SERIAL_TASK_H__

#include "main.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "elog.h"



void Serial_Task(void *paramater);
void Serial_Task_Init(void);


#endif
