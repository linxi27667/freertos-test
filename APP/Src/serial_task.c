#include "serial_task.h"

TaskHandle_t serial_task_handle;

void Serial_Task(void *paramater)
{
    while(1)
    {
        elog_i("serial_task:","waiting for semaphore");
        if(xSemaphoreTake(xUart1RxSemaphore, portMAX_DELAY) == pdPASS)
        {
						uint8_t i;
            for(i = 0; i < uart1_rx_len; i++)
            {
                elog_a("serial_task:","uart1_rx_buf[%d] = %c", i, uart1_rx_buf[i]);
            }
        }
        vTaskDelay(100);
    }
}


void Serial_Task_Init(void)
{
    BaseType_t temp = xTaskCreate(Serial_Task, "Serial_Task", 512, NULL, 1, &serial_task_handle);
    if(temp != pdPASS)
    {
        elog_e("Serial_Task_Init", "Serial_Task_Create_Failed");
    }
}
