################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/darpa/model/acknowledgment.cc \
../src/darpa/model/darpa-wireless-channel.cc \
../src/darpa/model/helpers.cc \
../src/darpa/model/mftdma-controller.cc \
../src/darpa/model/mftdma-header.cc \
../src/darpa/model/mftdma-mac-net-device.cc \
../src/darpa/model/mftdma-mac.cc \
../src/darpa/model/neighbour_table.cc \
../src/darpa/model/tdma_queue.cc \
../src/darpa/model/tdma_slot.cc \
../src/darpa/model/tdma_table.cc \
../src/darpa/model/zmq_mac_interface.cc 

CC_DEPS += \
./src/darpa/model/acknowledgment.d \
./src/darpa/model/darpa-wireless-channel.d \
./src/darpa/model/helpers.d \
./src/darpa/model/mftdma-controller.d \
./src/darpa/model/mftdma-header.d \
./src/darpa/model/mftdma-mac-net-device.d \
./src/darpa/model/mftdma-mac.d \
./src/darpa/model/neighbour_table.d \
./src/darpa/model/tdma_queue.d \
./src/darpa/model/tdma_slot.d \
./src/darpa/model/tdma_table.d \
./src/darpa/model/zmq_mac_interface.d 

OBJS += \
./src/darpa/model/acknowledgment.o \
./src/darpa/model/darpa-wireless-channel.o \
./src/darpa/model/helpers.o \
./src/darpa/model/mftdma-controller.o \
./src/darpa/model/mftdma-header.o \
./src/darpa/model/mftdma-mac-net-device.o \
./src/darpa/model/mftdma-mac.o \
./src/darpa/model/neighbour_table.o \
./src/darpa/model/tdma_queue.o \
./src/darpa/model/tdma_slot.o \
./src/darpa/model/tdma_table.o \
./src/darpa/model/zmq_mac_interface.o 


# Each subdirectory must supply rules for building sources it contributes
src/darpa/model/%.o: ../src/darpa/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


