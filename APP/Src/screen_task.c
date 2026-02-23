#include "screen_task.h"

TaskHandle_t screen_task_handle;
lcd_judge_t lcd_judge = lcd_state_1;
uint8_t temp_arr[20];

uint8_t old_password[4] = {'1', '1', '1', '\0'};
uint8_t current_password[4] = {'1', '1', '1', '\0'};

uint8_t f_pwm = 0;
uint8_t f_lcd_updata = 0;

void Password_init(uint8_t *p_B1, uint8_t *p_B2, uint8_t *p_B3)
{
    *p_B1 = 0;
    *p_B2 = 0;
    *p_B3 = 0;
    LCD_Clear(Black);
    LCD_DisplayStringLine(Line2, (uint8_t *)"       PSD");
    sprintf((char *)temp_arr, "    B1: @");
    LCD_DisplayStringLine(Line4, (uint8_t *)temp_arr);

    sprintf((char *)temp_arr, "    B2: @");
    LCD_DisplayStringLine(Line5, (uint8_t *)temp_arr);

    sprintf((char *)temp_arr, "    B3: @");
    LCD_DisplayStringLine(Line6, (uint8_t *)temp_arr);
}

void Set_Pwm(void)
{
   switch(f_pwm)
   {
    case 0:
       __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 500);
       elog_a("PWM", "1Khz 50%%");
       break;
    case 1:
       __HAL_TIM_SET_PRESCALER(&htim2, 40-1);
       __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 100);
       elog_a("PWM", "2Khz 10%%");
       break;
   }
  }

void Screen_Task(void *pvParameters)
{
		uint8_t B1 = 0;
    uint8_t B2 = 0;
    uint8_t B3 = 0;
    Set_Pwm();
    char password[4] = {'0', '0', '0', '\0'};
    Password_init(&B1, &B2, &B3);
    LCD_DisplayStringLine(Line2, (uint8_t *)"       PSD");
    while(1)
    {
        switch(lcd_judge)
        {
            case lcd_state_1:
						{
            if(my_key[0].f_push == 1) 
            {
                B1++;
                elog_a("B1", "%d", B1);
                sprintf((char *)temp_arr, "    B1: %d", B1);
                LCD_DisplayStringLine(Line4, (uint8_t *)temp_arr);
                if(B1 > 9)
                {
                    B1 = 0;
                }
                password[0] = B1 + '0';
								my_key[0].f_push = 0;
            }
            else if( my_key[1].f_push == 1) 
            {
                B2++;
                elog_a("B2", "%d", B2);
                sprintf((char *)temp_arr, "    B2: %d", B2);
                LCD_DisplayStringLine(Line5, (uint8_t *)temp_arr);
                if(B2 > 9)
                {
                    B2 = 0;
                }
                password[1] = B2 + '0';
								my_key[1].f_push = 0;
            }
            else if( my_key[2].f_push == 1) 
            {
                B3++;
                elog_a("B3", "%d", B3);
                sprintf((char *)temp_arr, "    B3: %d", B3);
                LCD_DisplayStringLine(Line6, (uint8_t *)temp_arr);
                if(B3 > 9)
                {
                    B3 = 0;
                }
                password[2] = B3 + '0';
								my_key[2].f_push = 0;
            }
              if(my_key[3].f_push == 1)
              {
                if(strcmp(password, (char *)current_password) == 0)
                {
                    lcd_judge = lcd_state_2;
                    LCD_Clear(Black);
                    f_pwm = 1;
                    Set_Pwm();
                    Led_On(&my_led[1]);
                    elog_a("screen_task:", "password is correct");
                }
                else
                {
                    B1 = B2 = B3 = 0;
                    Password_init(&B1, &B2, &B3);
                    f_pwm = 0;
                    Set_Pwm();
                    Led_On(&my_led[0]);
                    elog_e("screen_task:", "password is incorrect");
                }
                my_key[3].f_push = 0;
              }
                break;
					}
            case lcd_state_2:
						{
              if(my_key[3].f_push == 1)
              {
                lcd_judge = lcd_state_1;
                LCD_Clear(Black);
                Password_init(&B1, &B2, &B3);
                my_key[3].f_push = 0;
                break;
              }
              if(f_pwm == 0)
              {
                if(f_lcd_updata == 0)
                {
                  elog_a("DEBUG", "ERRRRRRRRR");
                  LCD_DisplayStringLine(Line2, (uint8_t *)"       STA");
                  sprintf((char*)temp_arr, "    F:%dHZ", 1000);
                  LCD_DisplayStringLine(Line4, (uint8_t *)temp_arr);
                  sprintf((char*)temp_arr, "    D:%d%%", 50);
                  LCD_DisplayStringLine(Line5, (uint8_t *)temp_arr);
                  f_lcd_updata = 1;
              }
                
              }
              else
              {
                lcd_judge = lcd_state_3;
                f_lcd_updata = 0;
                LCD_Clear(Black);
              }
                break;
						}
						
            case lcd_state_3:
            {
              if(my_key[3].f_push == 1)
              {
                lcd_judge = lcd_state_1;
                LCD_Clear(Black);
                Password_init(&B1, &B2, &B3);
                my_key[3].f_push = 0;
                break;
              }
              if(f_pwm == 1)
              {
                if(f_lcd_updata == 0)
                {
                  LCD_DisplayStringLine(Line2, (uint8_t *)"       STA");
                  sprintf((char*)temp_arr, "    F:%dHZ", 2000);
                  LCD_DisplayStringLine(Line4, (uint8_t *)temp_arr);
                  sprintf((char*)temp_arr, "    D:%d%%", 10);
                  LCD_DisplayStringLine(Line5, (uint8_t *)temp_arr);
                  f_lcd_updata = 1;
                 }
              }
              else
              {
                lcd_judge = lcd_state_2;
                f_lcd_updata = 0;
                LCD_Clear(Black);
              }
              break;
            }
        }
        #if (DEBUG_MODE == 1)
				elog_i("task", "screen_task running");
        #endif
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void Screen_Task_Init(void)
{
    BaseType_t rusult;

    LCD_Init();
    LCD_Clear(Black);
    LCD_SetBackColor(Black);
    LCD_SetTextColor(White);

    rusult = xTaskCreate(Screen_Task, "screen_task", 256, NULL, 2, &screen_task_handle);
    if(rusult != pdPASS)
    {
        elog_e("task", "Creat Error");
    }
}

