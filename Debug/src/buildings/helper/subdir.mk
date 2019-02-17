################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/buildings/helper/building-allocator.cc \
../src/buildings/helper/building-container.cc \
../src/buildings/helper/building-position-allocator.cc \
../src/buildings/helper/buildings-helper.cc 

CC_DEPS += \
./src/buildings/helper/building-allocator.d \
./src/buildings/helper/building-container.d \
./src/buildings/helper/building-position-allocator.d \
./src/buildings/helper/buildings-helper.d 

OBJS += \
./src/buildings/helper/building-allocator.o \
./src/buildings/helper/building-container.o \
./src/buildings/helper/building-position-allocator.o \
./src/buildings/helper/buildings-helper.o 


# Each subdirectory must supply rules for building sources it contributes
src/buildings/helper/%.o: ../src/buildings/helper/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


