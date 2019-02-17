################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/point-to-point/model/point-to-point-channel.cc \
../src/point-to-point/model/point-to-point-net-device.cc \
../src/point-to-point/model/point-to-point-remote-channel.cc \
../src/point-to-point/model/ppp-header.cc 

CC_DEPS += \
./src/point-to-point/model/point-to-point-channel.d \
./src/point-to-point/model/point-to-point-net-device.d \
./src/point-to-point/model/point-to-point-remote-channel.d \
./src/point-to-point/model/ppp-header.d 

OBJS += \
./src/point-to-point/model/point-to-point-channel.o \
./src/point-to-point/model/point-to-point-net-device.o \
./src/point-to-point/model/point-to-point-remote-channel.o \
./src/point-to-point/model/ppp-header.o 


# Each subdirectory must supply rules for building sources it contributes
src/point-to-point/model/%.o: ../src/point-to-point/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


