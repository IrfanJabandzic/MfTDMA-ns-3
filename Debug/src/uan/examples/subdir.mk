################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/uan/examples/uan-cw-example.cc \
../src/uan/examples/uan-rc-example.cc 

CC_DEPS += \
./src/uan/examples/uan-cw-example.d \
./src/uan/examples/uan-rc-example.d 

OBJS += \
./src/uan/examples/uan-cw-example.o \
./src/uan/examples/uan-rc-example.o 


# Each subdirectory must supply rules for building sources it contributes
src/uan/examples/%.o: ../src/uan/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


