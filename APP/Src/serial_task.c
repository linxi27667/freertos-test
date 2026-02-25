#include "serial_task.h"

TaskHandle_t serial_task_handle;

void Serial_Task(void *paramater)
{
//    uart_rx_msg_t rx_msg;
//    while(1)
//    {
//        #if (DEBUG_MODE == 1)
//        elog_i("serial_task:","waiting for queue");
//        #endif
//        if(xQueueReceive(g_uart1_rx_queue_handle, &rx_msg, portMAX_DELAY) == pdPASS)
//        {
//            uint8_t i;
//            uint8_t user_old_password[4];
//            uint8_t user_current_password[4];
//            for(i = 0; i < rx_msg.len; i++)
//            {
//                #if (DEBUG_MODE == 1)
//                elog_a("serial_task:","rx_msg.data[%d] = %c", i, rx_msg.data[i]);
//                #endif
//            }
//            if(sscanf((char *)rx_msg.data, "%[^-]-%3s", (char *)user_old_password, (char *)user_current_password) == 2)
//            {
//                #if (DEBUG_MODE == 1)
//                elog_i("serial_task:", "user_old_password = %s, user_current_password = %s", user_old_password, user_current_password);
//                #endif
//                if(strcmp((char*)user_old_password, (char*)old_password) == 0)
//                {
//                    strcpy((char*)current_password, (char*)user_current_password);
//                    strcpy((char*)old_password, (char*)current_password);
//                    elog_a("serial_task:", "old_password is correct");
//                    elog_a("serial_task:", "current_password is %s", current_password);
//                }
//                else
//                {
//                    elog_e("serial_task:", "old_password is incorrect");
//                }
//            }
//        }
//				vTaskDelay(pdMS_TO_TICKS(100));
//    }
}


void Serial_Task_Init(void)
{
    Uart_Init();

//    BaseType_t rusult = xTaskCreate(Serial_Task, "Serial_Task", 512, NULL, 1, &serial_task_handle);
//    if(rusult != pdPASS)
//    {
//        elog_e("Serial_Task_Init", "Serial_Task_Create_Failed");
//    }
}
