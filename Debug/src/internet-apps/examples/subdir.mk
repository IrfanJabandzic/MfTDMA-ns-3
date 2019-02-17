################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/internet-apps/examples/dhcp-example.cc 

CC_DEPS += \
./src/internet-apps/examples/dhcp-example.d 

OBJS += \
./src/internet-apps/examples/dhcp-example.o 


# Each subdirectory must supply rules for building sources it contributes
src/internet-apps/examples/%.o: ../src/internet-apps/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


