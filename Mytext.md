# 串口数据处理学习笔记

基于项目 `freertos-test` 的串口接收方案总结

---

## 一、三种方案对比

| 方案 | 适用场景 | 优点 | 缺点 |
|------|---------|------|------|
| 信号量+DMA | 简单场景 | 内存占用小 | 数据可能被覆盖 |
| 队列+DMA | 多任务通信 | 数据独立安全 | 内存占用较大 |
| 裸机定时器 | 无RTOS项目 | 简单直接 | 效率较低 |

---

## 二、信号量+DMA+空闲中断方案

### 工作流程
```
DMA接收 → 空闲中断触发 → 释放信号量 → 任务处理数据
```

### 代码实现

#### usart.h
```c
#define UART1_RX_BUF_SIZE 256

extern uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];
extern uint16_t uart1_rx_len;
extern SemaphoreHandle_t xUart1RxSemaphore;

void Uart1_Init(void);
```

#### usart.c
```c
uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];
uint16_t uart1_rx_len = 0;
SemaphoreHandle_t xUart1RxSemaphore = NULL;

void Uart1_Init(void)
{
    xUart1RxSemaphore = xSemaphoreCreateBinary();
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart1_rx_buf, UART1_RX_BUF_SIZE);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if(huart->Instance == USART1)
    {
        uart1_rx_len = size;  // 记录接收长度
        
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xSemaphoreGiveFromISR(xUart1RxSemaphore, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart1_rx_buf, UART1_RX_BUF_SIZE);
    }
}
```

#### serial_task.c
```c
void Serial_Task(void *paramater)
{
    while(1)
    {
        if(xSemaphoreTake(xUart1RxSemaphore, portMAX_DELAY) == pdPASS)
        {
            // 只处理 uart1_rx_len 个字节
            for(uint16_t i = 0; i < uart1_rx_len; i++)
            {
                process(uart1_rx_buf[i]);
            }
            // 不需要memset清除！
            // 下次新数据会自动覆盖，用len控制有效范围
        }
    }
}
```

### 注意事项
- 信号量只是"通知"，数据在共享缓冲区
- 必须快速处理，否则新数据会覆盖旧数据
- 用 `uart1_rx_len` 控制处理范围，不需要 memset

---

## 三、队列+DMA+空闲中断方案（推荐）

### 工作流程
```
DMA接收 → 空闲中断触发 → 数据拷贝入队 → 任务从队列取出处理
```

### 代码实现

#### usart.h
```c
#define UART1_RX_BUF_SIZE 256

typedef struct {
    uint8_t data[UART1_RX_BUF_SIZE];
    uint16_t len;
} UartRxMsg_t;

extern QueueHandle_t xUart1RxQueue;

void Uart1_Init(void);
```

#### usart.c
```c
QueueHandle_t xUart1RxQueue = NULL;
static uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];

void Uart1_Init(void)
{
    // 创建队列，深度5，每项大小为UartRxMsg_t
    xUart1RxQueue = xQueueCreate(5, sizeof(UartRxMsg_t));
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart1_rx_buf, UART1_RX_BUF_SIZE);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    if(huart->Instance == USART1)
    {
        UartRxMsg_t rx_msg;
        rx_msg.len = size;
        memcpy(rx_msg.data, uart1_rx_buf, size);  // 只拷贝有效数据
        
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(xUart1RxQueue, &rx_msg, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, uart1_rx_buf, UART1_RX_BUF_SIZE);
    }
}
```

#### serial_task.c
```c
void Serial_Task(void *paramater)
{
    UartRxMsg_t rx_msg;  // 局部变量
    
    while(1)
    {
        if(xQueueReceive(xUart1RxQueue, &rx_msg, portMAX_DELAY) == pdPASS)
        {
            // 数据已在rx_msg中，可以慢慢处理
            for(uint16_t i = 0; i < rx_msg.len; i++)
            {
                process(rx_msg.data[i]);
            }
            // 不需要清除，下次xQueueReceive会自动覆盖
        }
    }
}
```

### 优点
- 队列可缓存多条消息（深度决定）
- 每条消息独立，不会互相覆盖
- 数据安全，适合多任务环境

---

## 四、裸机定时器处理串口数据

### 原理
```
定时器中断(如1ms) → 检查接收标志 → 超时判断一帧结束 → 处理数据
```

### 代码实现

#### usart.h
```c
#define UART1_RX_BUF_SIZE 256

extern uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];
extern uint16_t uart1_rx_len;
extern uint8_t uart1_rx_flag;  // 接收完成标志

void Uart1_Init(void);
```

#### usart.c
```c
uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];
uint16_t uart1_rx_len = 0;
uint8_t uart1_rx_flag = 0;
static uint16_t uart1_rx_timeout = 0;

void Uart1_Init(void)
{
    HAL_UART_Receive_IT(&huart1, &uart1_rx_tmp, 1);  // 开启接收中断
}

// 串口接收中断回调
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        uart1_rx_buf[uart1_rx_len++] = uart1_rx_tmp;
        uart1_rx_timeout = 0;  // 重置超时计数
        HAL_UART_Receive_IT(&huart1, &uart1_rx_tmp, 1);
    }
}

// 定时器中断中调用（如1ms）
void Uart1_Timeout_Check(void)
{
    if(uart1_rx_len > 0)
    {
        uart1_rx_timeout++;
        if(uart1_rx_timeout >= 10)  // 10ms无新数据，认为一帧结束
        {
            uart1_rx_flag = 1;  // 标记接收完成
            uart1_rx_timeout = 0;
        }
    }
}
```

#### main.c
```c
// 定时器中断回调
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        Uart1_Timeout_Check();
    }
}

// 主循环中处理
while(1)
{
    if(uart1_rx_flag)
    {
        uart1_rx_flag = 0;
        
        // 处理数据
        for(uint16_t i = 0; i < uart1_rx_len; i++)
        {
            process(uart1_rx_buf[i]);
        }
        uart1_rx_len = 0;  // 重置长度
    }
}
```

### 缺点
- CPU占用高（每个字节都触发中断）
- 定时器超时时间需要根据波特率调整
- 不适合高速通信

---

## 五、核心概念总结

### 信号量 vs 队列

| 特性 | 信号量 | 队列 |
|------|--------|------|
| 传递内容 | 只通知"有数据" | 传递数据副本 |
| 数据安全 | 共享缓冲区，需快速处理 | 独立副本，安全 |
| 缓冲能力 | 无 | 有（队列深度） |
| 内存占用 | 小 | 较大 |

### 数据处理原则
```
收多少 → 存多少 → 传多少 → 处理多少
              ↓
        用 len 变量记录有效长度
```

### 不需要 memset 的原因
- 用长度变量 `len` 控制有效数据范围
- 新数据会自动覆盖旧数据
- 只处理 `len` 个字节即可

---

## 六、学习路线

```
信号量+DMA  →  队列+DMA  →  环形缓冲区(FIFO)  →  DMA循环+环形缓冲区
   ↑              ↑              ↑                    ↑
 已掌握          当前学习         下一步               工业级方案
```

---

## 七、工业推荐方案

**DMA循环模式 + 环形缓冲区 + 空闲中断**

- 可靠性高：DMA自动搬运
- 效率高：环形缓冲区内存利用率高
- 实时性好：空闲中断及时通知
- 稳定性强：长时间运行无内存碎片