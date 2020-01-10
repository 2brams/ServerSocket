################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/actionneur.c \
../src/capteur.c \
../src/main.c 

OBJS += \
./src/actionneur.o \
./src/capteur.o \
./src/main.o 

C_DEPS += \
./src/actionneur.d \
./src/capteur.d \
./src/main.d 


# Each subdirectory must supply rules for building sources it contributes
src/actionneur.o: ../src/actionneur.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-buildroot-linux-gnueabihf-gcc -I../~/armsystem/buildroot-rpi/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/actionneur.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	arm-buildroot-linux-gnueabihf-gcc -I../~/armsystem/buildroot-rpi/output/host/arm-buildroot-linux-gnueabihf/sysroot/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


