################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../examples/traffic-control/queue-discs-benchmark.cc \
../examples/traffic-control/red-vs-fengadaptive.cc \
../examples/traffic-control/red-vs-nlred.cc \
../examples/traffic-control/traffic-control.cc 

CC_DEPS += \
./examples/traffic-control/queue-discs-benchmark.d \
./examples/traffic-control/red-vs-fengadaptive.d \
./examples/traffic-control/red-vs-nlred.d \
./examples/traffic-control/traffic-control.d 

OBJS += \
./examples/traffic-control/queue-discs-benchmark.o \
./examples/traffic-control/red-vs-fengadaptive.o \
./examples/traffic-control/red-vs-nlred.o \
./examples/traffic-control/traffic-control.o 


# Each subdirectory must supply rules for building sources it contributes
examples/traffic-control/%.o: ../examples/traffic-control/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


