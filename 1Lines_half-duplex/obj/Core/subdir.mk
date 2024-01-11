################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/griggs/Code/CH32V/ch32v003/EVT/EXAM/SRC/Core/core_riscv.c 

OBJS += \
./Core/core_riscv.o 

C_DEPS += \
./Core/core_riscv.d 


# Each subdirectory must supply rules for building sources it contributes
Core/core_riscv.o: /home/griggs/Code/CH32V/ch32v003/EVT/EXAM/SRC/Core/core_riscv.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32ec -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -Wall  -g3 -I"/home/griggs/Code/CH32V/ch32v003/EVT/EXAM/SRC/Debug" -I"/home/griggs/Code/CH32V/ch32v003/EVT/EXAM/SRC/Core" -I"/home/griggs/Code/CH32V/ch32v003/EVT/EXAM/SPI/1Lines_half-duplex/User" -I"/home/griggs/Code/CH32V/ch32v003/EVT/EXAM/SRC/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


