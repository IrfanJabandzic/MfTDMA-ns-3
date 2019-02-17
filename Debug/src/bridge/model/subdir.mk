################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/bridge/model/bridge-channel.cc \
../src/bridge/model/bridge-net-device.cc 

CC_DEPS += \
./src/bridge/model/bridge-channel.d \
./src/bridge/model/bridge-net-device.d 

OBJS += \
./src/bridge/model/bridge-channel.o \
./src/bridge/model/bridge-net-device.o 


# Each subdirectory must supply rules for building sources it contributes
src/bridge/model/%.o: ../src/bridge/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


