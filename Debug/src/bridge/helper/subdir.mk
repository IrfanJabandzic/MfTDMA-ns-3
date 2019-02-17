################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/bridge/helper/bridge-helper.cc 

CC_DEPS += \
./src/bridge/helper/bridge-helper.d 

OBJS += \
./src/bridge/helper/bridge-helper.o 


# Each subdirectory must supply rules for building sources it contributes
src/bridge/helper/%.o: ../src/bridge/helper/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


