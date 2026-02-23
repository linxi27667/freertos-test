cmake_minimum_required(VERSION 3.28)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_VERSION 1)

# Specify the toolchain settings directly
set(CMAKE_C_COMPILER "arm-none-eabi-gcc.exe")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++.exe")
set(CMAKE_ASM_COMPILER "arm-none-eabi-gcc.exe")
set(CMAKE_AR "arm-none-eabi-ar.exe")
set(CMAKE_OBJCOPY "arm-none-eabi-objcopy.exe")
set(CMAKE_OBJDUMP "arm-none-eabi-objdump.exe")
set(CMAKE_SIZE "arm-none-eabi-size.exe")

# Skip compiler check
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)









project(freertos-test C CXX ASM)

set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard")

set(CMAKE_C_FLAGS "${MCU_FLAGS} -Wall -fdata-sections -ffunction-sections -O2 -g -gdwarf-2")
set(CMAKE_CXX_FLAGS "${MCU_FLAGS} -Wall -fdata-sections -ffunction-sections -O2 -g -gdwarf-2")
set(CMAKE_ASM_FLAGS "${MCU_FLAGS} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -specs=nano.specs -Wl,--gc-sections -Wl,--print-memory-usage -Wl,-Map=${CMAKE_PROJECT_NAME}.map")

# Define C preprocessor definitions
add_definitions(
    -DUSE_HAL_DRIVER
    -DSTM32G431xx
)

# Include directories
include_directories(
    Core/Inc
    APP/Inc
    BSP/Inc
    Drivers/STM32G4xx_HAL_Driver/Inc
    Drivers/STM32G4xx_HAL_Driver/Inc/Legacy
    Drivers/CMSIS/Device/ST/STM32G4xx/Include
    Drivers/CMSIS/Include
    Middlewares/Third_Party/FreeRTOS/Source/include
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F
    Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F
)

# Source files
set(SOURCES
    Core/Src/main.c
    Core/Src/gpio.c
    Core/Src/app_freertos.c
    Core/Src/stm32g4xx_it.c
    Core/Src/stm32g4xx_hal_msp.c
    Core/Src/stm32g4xx_hal_timebase_tim.c
    Core/Src/system_stm32g4xx.c
    APP/Src/screen_task.c
    BSP/Src/lcd.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_tim_ex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_rcc_ex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_flash_ramfunc.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_gpio.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_exti.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_dma_ex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_pwr_ex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_cortex.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart.c
    Drivers/STM32G4xx_HAL_Driver/Src/stm32g4xx_hal_uart_ex.c
    Middlewares/Third_Party/FreeRTOS/Source/croutine.c
    Middlewares/Third_Party/FreeRTOS/Source/event_groups.c
    Middlewares/Third_Party/FreeRTOS/Source/list.c
    Middlewares/Third_Party/FreeRTOS/Source/queue.c
    Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c
    Middlewares/Third_Party/FreeRTOS/Source/tasks.c
    Middlewares/Third_Party/FreeRTOS/Source/timers.c
    Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c
    Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c
    Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c
    Middlewares/Third_Party/FreeRTOS/Source/portable/RVDS/ARM_CM4F/port.c
)

# Add startup file
set(STARTUP_FILE "startup_stm32g431xx.s")
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${STARTUP_FILE})
    # If startup file doesn't exist in root, try to find it
    file(GLOB_RECURSE STARTUP_FOUND "startup_stm32g431xx.s")
    if(STARTUP_FOUND)
        list(GET STARTUP_FOUND 0 STARTUP_FILE)
    else()
        message(WARNING "Startup file startup_stm32g431xx.s not found!")
    endif()
endif()
list(APPEND SOURCES ${STARTUP_FILE})

# Linker script
set(LINKER_SCRIPT "STM32G431RBTx_FLASH.ld")
if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${LINKER_SCRIPT})
     # If linker script doesn't exist in root, try to find it
    file(GLOB_RECURSE LINKER_FOUND "*.ld")
    if(LINKER_FOUND)
        list(GET LINKER_FOUND 0 LINKER_SCRIPT)
    else()
         message(WARNING "Linker script not found!")
    endif()
endif()
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${LINKER_SCRIPT}")


add_executable(${CMAKE_PROJECT_NAME}.elf ${SOURCES})

# Create hex and bin files
add_custom_command(TARGET ${CMAKE_PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
    COMMENT "Building ${CMAKE_PROJECT_NAME}.hex and ${CMAKE_PROJECT_NAME}.bin"
)
