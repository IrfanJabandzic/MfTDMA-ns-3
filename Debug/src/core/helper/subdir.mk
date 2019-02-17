################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/core/helper/event-garbage-collector.cc \
../src/core/helper/random-variable-stream-helper.cc 

CC_DEPS += \
./src/core/helper/event-garbage-collector.d \
./src/core/helper/random-variable-stream-helper.d 

OBJS += \
./src/core/helper/event-garbage-collector.o \
./src/core/helper/random-variable-stream-helper.o 


# Each subdirectory must supply rules for building sources it contributes
src/core/helper/%.o: ../src/core/helper/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


