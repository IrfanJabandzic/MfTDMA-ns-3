################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../examples/stats/wifi-example-apps.cc \
../examples/stats/wifi-example-sim.cc 

CC_DEPS += \
./examples/stats/wifi-example-apps.d \
./examples/stats/wifi-example-sim.d 

OBJS += \
./examples/stats/wifi-example-apps.o \
./examples/stats/wifi-example-sim.o 


# Each subdirectory must supply rules for building sources it contributes
examples/stats/%.o: ../examples/stats/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


