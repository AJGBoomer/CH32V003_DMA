################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
/home/griggs/Code/CH32V/ch32v003/EVT/EXAM/SRC/Startup/startup_ch32v00x.S 

OBJS += \
./Startup/startup_ch32v00x.o 

S_UPPER_DEPS += \
./Startup/startup_ch32v00x.d 


# Each subdirectory must supply rules for building sources it contributes
Startup/startup_ch32v00x.o: /home/griggs/Code/CH32V/ch32v003/EVT/EXAM/SRC/Startup/startup_ch32v00x.S
	@echo 'Building file: $<'
	@echo 'Invoking: GNU RISC-V Cross Assembler'
	riscv-none-embed-gcc -march=rv32ec -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -Wall  -g3 -x assembler-with-cpp -I"/home/griggs/Code/CH32V/ch32v003/EVT/EXAM/SRC/Startup" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


