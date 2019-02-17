################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/dsdv/model/dsdv-packet-queue.cc \
../src/dsdv/model/dsdv-packet.cc \
../src/dsdv/model/dsdv-routing-protocol.cc \
../src/dsdv/model/dsdv-rtable.cc 

CC_DEPS += \
./src/dsdv/model/dsdv-packet-queue.d \
./src/dsdv/model/dsdv-packet.d \
./src/dsdv/model/dsdv-routing-protocol.d \
./src/dsdv/model/dsdv-rtable.d 

OBJS += \
./src/dsdv/model/dsdv-packet-queue.o \
./src/dsdv/model/dsdv-packet.o \
./src/dsdv/model/dsdv-routing-protocol.o \
./src/dsdv/model/dsdv-rtable.o 


# Each subdirectory must supply rules for building sources it contributes
src/dsdv/model/%.o: ../src/dsdv/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


