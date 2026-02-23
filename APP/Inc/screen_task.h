#ifndef __SCREEN_TASK_H__
#define __SCREEN_TASK_H__

#include "main.h"
#include "stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "lcd.h"
#include "pwm_task.h"

typedef enum lcd_judge
{
    lcd_state_1 = 0,
    lcd_state_2 = 1,
    lcd_state_3 = 2
}lcd_judge_t;

void Screen_Task(void *pvParameters);
void Screen_Task_Init(void);
void Set_Pwm(void);

extern uint8_t f_pwm;
extern uint8_t old_password[4];
extern uint8_t current_password[4];

#endif
