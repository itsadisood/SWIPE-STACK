################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/main.c \
../src/syscalls.c \
../src/system_stm32f0xx.c 

S_UPPER_SRCS += \
../src/nano_wait.S 

OBJS += \
./src/main.o \
./src/nano_wait.o \
./src/syscalls.o \
./src/system_stm32f0xx.o 

S_UPPER_DEPS += \
./src/nano_wait.d 

C_DEPS += \
./src/main.d \
./src/syscalls.d \
./src/system_stm32f0xx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F091RCTx -DDEBUG -DSTM32F091 -DUSE_STDPERIPH_DRIVER -I"C:/Users/Aditya/workspace/SWIPE-STACK/STM32 Code/IMU_UART_RVC/StdPeriph_Driver/inc" -I"C:/Users/Aditya/workspace/SWIPE-STACK/STM32 Code/IMU_UART_RVC/inc" -I"C:/Users/Aditya/workspace/SWIPE-STACK/STM32 Code/IMU_UART_RVC/CMSIS/device" -I"C:/Users/Aditya/workspace/SWIPE-STACK/STM32 Code/IMU_UART_RVC/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F091RCTx -DDEBUG -DSTM32F091 -DUSE_STDPERIPH_DRIVER -I"C:/Users/Aditya/workspace/SWIPE-STACK/STM32 Code/IMU_UART_RVC/StdPeriph_Driver/inc" -I"C:/Users/Aditya/workspace/SWIPE-STACK/STM32 Code/IMU_UART_RVC/inc" -I"C:/Users/Aditya/workspace/SWIPE-STACK/STM32 Code/IMU_UART_RVC/CMSIS/device" -I"C:/Users/Aditya/workspace/SWIPE-STACK/STM32 Code/IMU_UART_RVC/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


