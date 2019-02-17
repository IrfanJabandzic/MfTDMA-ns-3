################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/flow-monitor/model/flow-classifier.cc \
../src/flow-monitor/model/flow-monitor.cc \
../src/flow-monitor/model/flow-probe.cc \
../src/flow-monitor/model/histogram.cc \
../src/flow-monitor/model/ipv4-flow-classifier.cc \
../src/flow-monitor/model/ipv4-flow-probe.cc \
../src/flow-monitor/model/ipv6-flow-classifier.cc \
../src/flow-monitor/model/ipv6-flow-probe.cc 

CC_DEPS += \
./src/flow-monitor/model/flow-classifier.d \
./src/flow-monitor/model/flow-monitor.d \
./src/flow-monitor/model/flow-probe.d \
./src/flow-monitor/model/histogram.d \
./src/flow-monitor/model/ipv4-flow-classifier.d \
./src/flow-monitor/model/ipv4-flow-probe.d \
./src/flow-monitor/model/ipv6-flow-classifier.d \
./src/flow-monitor/model/ipv6-flow-probe.d 

OBJS += \
./src/flow-monitor/model/flow-classifier.o \
./src/flow-monitor/model/flow-monitor.o \
./src/flow-monitor/model/flow-probe.o \
./src/flow-monitor/model/histogram.o \
./src/flow-monitor/model/ipv4-flow-classifier.o \
./src/flow-monitor/model/ipv4-flow-probe.o \
./src/flow-monitor/model/ipv6-flow-classifier.o \
./src/flow-monitor/model/ipv6-flow-probe.o 


# Each subdirectory must supply rules for building sources it contributes
src/flow-monitor/model/%.o: ../src/flow-monitor/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


