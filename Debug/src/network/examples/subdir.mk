################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/network/examples/main-packet-header.cc \
../src/network/examples/main-packet-tag.cc \
../src/network/examples/packet-socket-apps.cc 

CC_DEPS += \
./src/network/examples/main-packet-header.d \
./src/network/examples/main-packet-tag.d \
./src/network/examples/packet-socket-apps.d 

OBJS += \
./src/network/examples/main-packet-header.o \
./src/network/examples/main-packet-tag.o \
./src/network/examples/packet-socket-apps.o 


# Each subdirectory must supply rules for building sources it contributes
src/network/examples/%.o: ../src/network/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


