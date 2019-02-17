################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/config-store/examples/config-store-save.cc 

CC_DEPS += \
./src/config-store/examples/config-store-save.d 

OBJS += \
./src/config-store/examples/config-store-save.o 


# Each subdirectory must supply rules for building sources it contributes
src/config-store/examples/%.o: ../src/config-store/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


