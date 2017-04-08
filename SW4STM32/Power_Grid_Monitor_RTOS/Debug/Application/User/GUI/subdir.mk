################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Application/User/GUI/HarmonicsDLG.c \
../Application/User/GUI/ImpedancesDLG.c \
../Application/User/GUI/MainWindowDLG.c \
../Application/User/GUI/SetDateTimeDLG.c \
../Application/User/GUI/SettingsDLG.c \
../Application/User/GUI/WaveformsDLG.c \
../Application/User/GUI/agh_r.c \
../Application/User/GUI/emwin_functions.c 

OBJS += \
./Application/User/GUI/HarmonicsDLG.o \
./Application/User/GUI/ImpedancesDLG.o \
./Application/User/GUI/MainWindowDLG.o \
./Application/User/GUI/SetDateTimeDLG.o \
./Application/User/GUI/SettingsDLG.o \
./Application/User/GUI/WaveformsDLG.o \
./Application/User/GUI/agh_r.o \
./Application/User/GUI/emwin_functions.o 

C_DEPS += \
./Application/User/GUI/HarmonicsDLG.d \
./Application/User/GUI/ImpedancesDLG.d \
./Application/User/GUI/MainWindowDLG.d \
./Application/User/GUI/SetDateTimeDLG.d \
./Application/User/GUI/SettingsDLG.d \
./Application/User/GUI/WaveformsDLG.d \
./Application/User/GUI/agh_r.d \
./Application/User/GUI/emwin_functions.d 


# Each subdirectory must supply rules for building sources it contributes
Application/User/GUI/%.o: ../Application/User/GUI/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo %cd%
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -std=c11 -D__weak=__attribute__((weak)) -D__packed=__attribute__((__packed__)) -DUSE_HAL_DRIVER -DSTM32F429xx -DARM_MATH_CM4 -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Inc" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/SW4STM32/Power_Grid_Monitor_RTOS/Middlewares/STemWin/inc" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/SW4STM32/Power_Grid_Monitor_RTOS/Middlewares/STemWin/Config" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/SW4STM32/Power_Grid_Monitor_RTOS/Drivers/BSP" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Drivers/STM32F4xx_HAL_Driver/Inc" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Middlewares/Third_Party/FatFs/src/drivers" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Middlewares/ST/STM32_USB_Host_Library/Core/Inc" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Drivers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Middlewares/Third_Party/FatFs/src" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Middlewares/Third_Party/FreeRTOS/Source/include" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS" -I"C:/Users/Kwarc/Documents/uC/STM32/PRACA MAGISTERSKA/power_grid_monitor_RTOS/Drivers/CMSIS/Include"  -O2 -Wall -fmessage-length=0 -ffunction-sections -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


