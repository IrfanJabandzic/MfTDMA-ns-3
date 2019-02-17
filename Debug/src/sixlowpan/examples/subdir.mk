################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/sixlowpan/examples/example-ping-lr-wpan.cc \
../src/sixlowpan/examples/example-sixlowpan.cc 

CC_DEPS += \
./src/sixlowpan/examples/example-ping-lr-wpan.d \
./src/sixlowpan/examples/example-sixlowpan.d 

OBJS += \
./src/sixlowpan/examples/example-ping-lr-wpan.o \
./src/sixlowpan/examples/example-sixlowpan.o 


# Each subdirectory must supply rules for building sources it contributes
src/sixlowpan/examples/%.o: ../src/sixlowpan/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


