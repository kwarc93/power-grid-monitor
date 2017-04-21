################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/USB_Host_Library/subdir.mk
-include Middlewares/STemWin/Config/subdir.mk
-include Middlewares/FreeRTOS/subdir.mk
-include Middlewares/FatFs/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/subdir.mk
-include Drivers/CMSIS/subdir.mk
-include Drivers/BSP/subdir.mk
-include Application/User/GUI/subdir.mk
-include Application/User/subdir.mk
-include Application/SW4STM32/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Power_Grid_Monitor_RTOS.elf

# Tool invocations
Power_Grid_Monitor_RTOS.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU GCC Linker'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -L"C:\Users\Kwarc\Documents\uC\STM32\PRACA MAGISTERSKA\power_grid_monitor_RTOS\SW4STM32\Power_Grid_Monitor_RTOS\Drivers\CMSIS\Lib\GCC" -L"C:\Users\Kwarc\Documents\uC\STM32\PRACA MAGISTERSKA\power_grid_monitor_RTOS\SW4STM32\Power_Grid_Monitor_RTOS\Middlewares\STemWin\Lib" -specs=nosys.specs -specs=nano.specs -Xlinker --print-memory-usage -T"../STM32F429ZITx_FLASH.ld" -Wl,-Map=output.map -Wl,--gc-sections -lm -o "Power_Grid_Monitor_RTOS.elf" @"objects.list" $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

post-build:
	-@echo 'Generating binary and Printing size information:'
	arm-none-eabi-objcopy -O binary "Power_Grid_Monitor_RTOS.elf" "Power_Grid_Monitor_RTOS.bin"
	arm-none-eabi-size "Power_Grid_Monitor_RTOS.elf"
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY: post-build

-include ../makefile.targets
