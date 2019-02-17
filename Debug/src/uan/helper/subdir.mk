################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/uan/helper/acoustic-modem-energy-model-helper.cc \
../src/uan/helper/uan-helper.cc 

CC_DEPS += \
./src/uan/helper/acoustic-modem-energy-model-helper.d \
./src/uan/helper/uan-helper.d 

OBJS += \
./src/uan/helper/acoustic-modem-energy-model-helper.o \
./src/uan/helper/uan-helper.o 


# Each subdirectory must supply rules for building sources it contributes
src/uan/helper/%.o: ../src/uan/helper/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


