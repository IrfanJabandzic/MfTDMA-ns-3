################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mesh/model/flame/flame-header.cc \
../src/mesh/model/flame/flame-protocol-mac.cc \
../src/mesh/model/flame/flame-protocol.cc \
../src/mesh/model/flame/flame-rtable.cc 

CC_DEPS += \
./src/mesh/model/flame/flame-header.d \
./src/mesh/model/flame/flame-protocol-mac.d \
./src/mesh/model/flame/flame-protocol.d \
./src/mesh/model/flame/flame-rtable.d 

OBJS += \
./src/mesh/model/flame/flame-header.o \
./src/mesh/model/flame/flame-protocol-mac.o \
./src/mesh/model/flame/flame-protocol.o \
./src/mesh/model/flame/flame-rtable.o 


# Each subdirectory must supply rules for building sources it contributes
src/mesh/model/flame/%.o: ../src/mesh/model/flame/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


