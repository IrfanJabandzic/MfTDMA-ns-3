################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/wimax/examples/wimax-ipv4.cc \
../src/wimax/examples/wimax-multicast.cc \
../src/wimax/examples/wimax-simple.cc 

CC_DEPS += \
./src/wimax/examples/wimax-ipv4.d \
./src/wimax/examples/wimax-multicast.d \
./src/wimax/examples/wimax-simple.d 

OBJS += \
./src/wimax/examples/wimax-ipv4.o \
./src/wimax/examples/wimax-multicast.o \
./src/wimax/examples/wimax-simple.o 


# Each subdirectory must supply rules for building sources it contributes
src/wimax/examples/%.o: ../src/wimax/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


