################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/bitTable.c \
../User/ch32v00x_it.c \
../User/main.c \
../User/patterns.c \
../User/system_ch32v00x.c 

OBJS += \
./User/bitTable.o \
./User/ch32v00x_it.o \
./User/main.o \
./User/patterns.o \
./User/system_ch32v00x.o 

C_DEPS += \
./User/bitTable.d \
./User/ch32v00x_it.d \
./User/main.d \
./User/patterns.d \
./User/system_ch32v00x.d 


# Each subdirectory must supply rules for building sources it contributes
User/%.o: ../User/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross C Compiler'
	riscv-none-embed-gcc -march=rv32ec -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -Wall  -g3 -I"/home/griggs/Code/CH32V/ch32v003/EVT/EXAM/SRC/Debug" -I"/home/griggs/Code/CH32V/ch32v003/EVT/EXAM/SRC/Core" -I"/home/griggs/Code/CH32V/ch32v003/EVT/EXAM/SPI/1Lines_half-duplex/User" -I"/home/griggs/Code/CH32V/ch32v003/EVT/EXAM/SRC/Peripheral/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


