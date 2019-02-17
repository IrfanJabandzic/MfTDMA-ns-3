################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/buildings/examples/buildings-pathloss-profiler.cc 

CC_DEPS += \
./src/buildings/examples/buildings-pathloss-profiler.d 

OBJS += \
./src/buildings/examples/buildings-pathloss-profiler.o 


# Each subdirectory must supply rules for building sources it contributes
src/buildings/examples/%.o: ../src/buildings/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


