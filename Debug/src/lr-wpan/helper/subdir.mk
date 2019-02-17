################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/lr-wpan/helper/lr-wpan-helper.cc 

CC_DEPS += \
./src/lr-wpan/helper/lr-wpan-helper.d 

OBJS += \
./src/lr-wpan/helper/lr-wpan-helper.o 


# Each subdirectory must supply rules for building sources it contributes
src/lr-wpan/helper/%.o: ../src/lr-wpan/helper/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


