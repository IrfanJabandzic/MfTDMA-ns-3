################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/network/model/address.cc \
../src/network/model/application.cc \
../src/network/model/buffer.cc \
../src/network/model/byte-tag-list.cc \
../src/network/model/channel-list.cc \
../src/network/model/channel.cc \
../src/network/model/chunk.cc \
../src/network/model/header.cc \
../src/network/model/net-device.cc \
../src/network/model/nix-vector.cc \
../src/network/model/node-list.cc \
../src/network/model/node.cc \
../src/network/model/packet-metadata.cc \
../src/network/model/packet-tag-list.cc \
../src/network/model/packet.cc \
../src/network/model/socket-factory.cc \
../src/network/model/socket.cc \
../src/network/model/tag-buffer.cc \
../src/network/model/tag.cc \
../src/network/model/trailer.cc 

CC_DEPS += \
./src/network/model/address.d \
./src/network/model/application.d \
./src/network/model/buffer.d \
./src/network/model/byte-tag-list.d \
./src/network/model/channel-list.d \
./src/network/model/channel.d \
./src/network/model/chunk.d \
./src/network/model/header.d \
./src/network/model/net-device.d \
./src/network/model/nix-vector.d \
./src/network/model/node-list.d \
./src/network/model/node.d \
./src/network/model/packet-metadata.d \
./src/network/model/packet-tag-list.d \
./src/network/model/packet.d \
./src/network/model/socket-factory.d \
./src/network/model/socket.d \
./src/network/model/tag-buffer.d \
./src/network/model/tag.d \
./src/network/model/trailer.d 

OBJS += \
./src/network/model/address.o \
./src/network/model/application.o \
./src/network/model/buffer.o \
./src/network/model/byte-tag-list.o \
./src/network/model/channel-list.o \
./src/network/model/channel.o \
./src/network/model/chunk.o \
./src/network/model/header.o \
./src/network/model/net-device.o \
./src/network/model/nix-vector.o \
./src/network/model/node-list.o \
./src/network/model/node.o \
./src/network/model/packet-metadata.o \
./src/network/model/packet-tag-list.o \
./src/network/model/packet.o \
./src/network/model/socket-factory.o \
./src/network/model/socket.o \
./src/network/model/tag-buffer.o \
./src/network/model/tag.o \
./src/network/model/trailer.o 


# Each subdirectory must supply rules for building sources it contributes
src/network/model/%.o: ../src/network/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


