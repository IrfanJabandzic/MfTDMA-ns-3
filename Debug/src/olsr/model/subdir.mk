################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/olsr/model/olsr-header.cc \
../src/olsr/model/olsr-routing-protocol.cc \
../src/olsr/model/olsr-state.cc 

CC_DEPS += \
./src/olsr/model/olsr-header.d \
./src/olsr/model/olsr-routing-protocol.d \
./src/olsr/model/olsr-state.d 

OBJS += \
./src/olsr/model/olsr-header.o \
./src/olsr/model/olsr-routing-protocol.o \
./src/olsr/model/olsr-state.o 


# Each subdirectory must supply rules for building sources it contributes
src/olsr/model/%.o: ../src/olsr/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


