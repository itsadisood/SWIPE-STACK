################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32.s 

OBJS += \
./startup/startup_stm32.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -I"/Users/vishnulagudu/Documents/projects/SWIPE-STACK/STM32 Code/Glove_Code/StdPeriph_Driver/inc" -I"/Users/vishnulagudu/Documents/projects/SWIPE-STACK/STM32 Code/Glove_Code/inc" -I"/Users/vishnulagudu/Documents/projects/SWIPE-STACK/STM32 Code/Glove_Code/CMSIS/device" -I"/Users/vishnulagudu/Documents/projects/SWIPE-STACK/STM32 Code/Glove_Code/CMSIS/core" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


