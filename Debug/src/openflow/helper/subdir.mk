################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/openflow/helper/openflow-switch-helper.cc 

CC_DEPS += \
./src/openflow/helper/openflow-switch-helper.d 

OBJS += \
./src/openflow/helper/openflow-switch-helper.o 


# Each subdirectory must supply rules for building sources it contributes
src/openflow/helper/%.o: ../src/openflow/helper/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


