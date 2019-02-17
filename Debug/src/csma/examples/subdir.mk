################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/csma/examples/csma-broadcast.cc \
../src/csma/examples/csma-multicast.cc \
../src/csma/examples/csma-one-subnet.cc \
../src/csma/examples/csma-packet-socket.cc \
../src/csma/examples/csma-ping.cc \
../src/csma/examples/csma-raw-ip-socket.cc 

CC_DEPS += \
./src/csma/examples/csma-broadcast.d \
./src/csma/examples/csma-multicast.d \
./src/csma/examples/csma-one-subnet.d \
./src/csma/examples/csma-packet-socket.d \
./src/csma/examples/csma-ping.d \
./src/csma/examples/csma-raw-ip-socket.d 

OBJS += \
./src/csma/examples/csma-broadcast.o \
./src/csma/examples/csma-multicast.o \
./src/csma/examples/csma-one-subnet.o \
./src/csma/examples/csma-packet-socket.o \
./src/csma/examples/csma-ping.o \
./src/csma/examples/csma-raw-ip-socket.o 


# Each subdirectory must supply rules for building sources it contributes
src/csma/examples/%.o: ../src/csma/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


