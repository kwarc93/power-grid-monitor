################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middlewares/STemWin/Config/GUIConf.c \
../Middlewares/STemWin/Config/GUI_X_OS.c \
../Middlewares/STemWin/Config/LCDConf_stm32f429i_disco_MB1075.c \
../Middlewares/STemWin/Config/SIMConf.c 

OBJS += \
./Middlewares/STemWin/Config/GUIConf.o \
./Middlewares/STemWin/Config/GUI_X_OS.o \
./Middlewares/STemWin/Config/LCDConf_stm32f429i_disco_MB1075.o \
./Middlewares/STemWin/Config/SIMConf.o 

C_DEPS += \
./Middlewares/STemWin/Config/GUIConf.d \
./Middlewares/STemWin/Config/GUI_X_OS.d \
./Middlewares/STemWin/Config/LCDConf_stm32f429i_disco_MB1075.d \
./Middlewares/STemWin/Config/SIMConf.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/STemWin/Config/%.o: ../Middlewares/STemWin/Config/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c11 -D__weak=__attribute__((weak)) -D__packed=__attribute__((__packed__)) -DUSE_HAL_DRIVER -DSTM32F429xx -DARM_MATH_CM4 -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Inc" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Middlewares/Third_Party/FatFs/src/drivers" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Middlewares/ST/STM32_USB_Host_Library/Core/Inc" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Middlewares/Third_Party/FatFs/src" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Drivers/CMSIS/Include" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/SW4STM32/Power_Grid_Monitor_RTOS/Drivers/BSP" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/SW4STM32/Power_Grid_Monitor_RTOS/Middlewares/STemWin/Config" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/SW4STM32/Power_Grid_Monitor_RTOS/Middlewares/STemWin/inc"  -Og -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


