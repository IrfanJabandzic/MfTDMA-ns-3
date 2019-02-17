################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/applications/helper/bulk-send-helper.cc \
../src/applications/helper/on-off-helper.cc \
../src/applications/helper/packet-sink-helper.cc \
../src/applications/helper/udp-client-server-helper.cc \
../src/applications/helper/udp-echo-helper.cc 

CC_DEPS += \
./src/applications/helper/bulk-send-helper.d \
./src/applications/helper/on-off-helper.d \
./src/applications/helper/packet-sink-helper.d \
./src/applications/helper/udp-client-server-helper.d \
./src/applications/helper/udp-echo-helper.d 

OBJS += \
./src/applications/helper/bulk-send-helper.o \
./src/applications/helper/on-off-helper.o \
./src/applications/helper/packet-sink-helper.o \
./src/applications/helper/udp-client-server-helper.o \
./src/applications/helper/udp-echo-helper.o 


# Each subdirectory must supply rules for building sources it contributes
src/applications/helper/%.o: ../src/applications/helper/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


