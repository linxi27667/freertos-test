# Stm32g431_Freertos_Lab

STM32G431 + **FreeRTOS**（CMSIS-RTOS）实验工程：按键 / PWM / 屏幕 / 串口任务，重点练习 **UART 空闲中断 + DMA + 信号量** 收包方案。

CubeMX 工程：`freertos-test.ioc`；目标芯片 STM32G431。

## 系统架构图

```mermaid
flowchart TB
  subgraph Sched["FreeRTOS 调度"]
    MAIN[main / MX_Init]
    APPFR[app_freertos.c\n创建各业务任务]
    MAIN --> APPFR
  end

  subgraph Tasks["APP 任务"]
    KEY[key_task]
    PWM[pwm_task]
    SCR[screen_task]
    SER[serial_task\n等信号量处理串口帧]
  end

  subgraph BSP["BSP"]
    BK[bsp_key]
    BL[bsp_led]
    BU[bsp_uart]
    LCD[lcd]
  end

  subgraph HW["外设"]
    UART[USART + DMA + IDLE]
    GPIO[GPIO 按键/LED]
    TIM[定时器 PWM]
    DISP[显示屏]
  end

  APPFR --> KEY & PWM & SCR & SER
  KEY --> BK --> GPIO
  PWM --> TIM
  SCR --> LCD --> DISP
  SER --> BU --> UART
  RTT[RTT_easylog] -.-> APPFR
```

## 串口收包框图

```mermaid
flowchart LR
  RX[UART DMA 接收] --> IDLE[空闲中断]
  IDLE --> SEM[释放二值信号量]
  SEM --> TASK[serial_task 取信号量]
  TASK --> PROC[按 uart1_rx_len 处理有效字节]
  PROC --> REARM[再次启动 ReceiveToIdle_DMA]
```

## 目录说明

| 路径 | 说明 |
|------|------|
| `Core/` | Cube 生成：main、USART、DMA、TIM、FreeRTOS 入口 |
| `APP/` | 业务任务：`key_task` / `pwm_task` / `screen_task` / `serial_task` |
| `BSP/` | 板级：`bsp_key` / `bsp_led` / `bsp_uart` / `lcd` |
| `RTT_easylog/` | RTT 日志 |
| `MDK-ARM/` / `EWARM/` / `cmake/` | 多工具链工程 |
| `Mytext.md` | 串口 DMA 方案学习笔记 |

## 远程

```text
origin → https://github.com/linxi27667/Stm32g431_Freertos_Lab.git
```

本地常见路径：`E:\MCU\lanqiaobei\freertos-test`（若有副本目录，一并改 remote）
