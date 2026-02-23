#include "pwm_task.h"

TaskHandle_t Pwm_Task_Handle = NULL;
//uint8_t f_pwm = 0;

void Pwm_Task(void *pvParameters)
{
//    while(1)
//    {
//        if(f_pwm == 0)
//        {
//            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500);
//            elog_i("pwm_task", "pwm = 1khz");
//        }
//        else
//        {
//            __HAL_TIM_SET_PRESCALER(&htim2, 40-1);
//            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 100);
//            elog_i("pwm_task", "pwm = 2khz");
//            vTaskDelay(pdMS_TO_TICKS(5000));
//            f_pwm = 0;
//        }
//        vTaskDelay(pdMS_TO_TICKS(100));
//    }
}

void Pwm_Task_Init(void)
{
//    BaseType_t temp;
//    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
//    temp = xTaskCreate(Pwm_Task, "Pwm_Task", 256, NULL, 2, &Pwm_Task_Handle);
//    if(temp != pdPASS)
//    {
//        elog_e("pwm_task", "Pwm_Task create fail");
//    }
}
