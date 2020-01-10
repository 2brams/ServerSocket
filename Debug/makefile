################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: wiringPiTest

# Tool invocations
wiringPiTest: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross GCC Linker'
	arm-buildroot-linux-gnueabihf-gcc -L../~/armsystem/buildroot-rpi/output/build/wiringpi-2.46/wiringPi -L../~/armsystem/buildroot-rpi/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/lib -Xlinker -l:libwiringPi.so.2.46 -Xlinker -lwiringPi -lm -o "wiringPiTest" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(EXECUTABLES)$(OBJS)$(C_DEPS) wiringPiTest
	-@echo ' '

.PHONY: all clean dependents

-include ../makefile.targets
