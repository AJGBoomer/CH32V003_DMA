################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
1Lines_half-duplex.hex \

SECONDARY_LIST += \
1Lines_half-duplex.lst \

SECONDARY_SIZE += \
1Lines_half-duplex.siz \


# All Target
all: 1Lines_half-duplex.elf secondary-outputs

# Tool invocations
1Lines_half-duplex.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GNU RISC-V Cross C Linker'
	riscv-none-embed-gcc -march=rv32ec -mabi=ilp32e -msmall-data-limit=0 -msave-restore -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized -Wall  -g3 -T "/home/griggs/Code/CH32V/ch32v003/EVT/EXAM/SRC/Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"1Lines_half-duplex.map" --specs=nano.specs --specs=nosys.specs -o "1Lines_half-duplex.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

1Lines_half-duplex.hex: 1Lines_half-duplex.elf
	@echo 'Invoking: GNU RISC-V Cross Create Flash Image'
	riscv-none-embed-objcopy -O ihex "1Lines_half-duplex.elf"  "1Lines_half-duplex.hex"
	@echo 'Finished building: $@'
	@echo ' '

1Lines_half-duplex.lst: 1Lines_half-duplex.elf
	@echo 'Invoking: GNU RISC-V Cross Create Listing'
	riscv-none-embed-objdump --all-headers --demangle --disassemble "1Lines_half-duplex.elf" > "1Lines_half-duplex.lst"
	@echo 'Finished building: $@'
	@echo ' '

1Lines_half-duplex.siz: 1Lines_half-duplex.elf
	@echo 'Invoking: GNU RISC-V Cross Print Size'
	riscv-none-embed-size --format=berkeley "1Lines_half-duplex.elf"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) 1Lines_half-duplex.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
