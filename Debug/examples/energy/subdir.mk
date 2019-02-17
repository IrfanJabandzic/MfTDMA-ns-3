################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../examples/energy/energy-model-example.cc \
../examples/energy/energy-model-with-harvesting-example.cc 

CC_DEPS += \
./examples/energy/energy-model-example.d \
./examples/energy/energy-model-with-harvesting-example.d 

OBJS += \
./examples/energy/energy-model-example.o \
./examples/energy/energy-model-with-harvesting-example.o 


# Each subdirectory must supply rules for building sources it contributes
examples/energy/%.o: ../examples/energy/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


