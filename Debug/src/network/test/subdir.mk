################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/network/test/buffer-test.cc \
../src/network/test/drop-tail-queue-test-suite.cc \
../src/network/test/error-model-test-suite.cc \
../src/network/test/ipv6-address-test-suite.cc \
../src/network/test/packet-metadata-test.cc \
../src/network/test/packet-socket-apps-test-suite.cc \
../src/network/test/packet-test-suite.cc \
../src/network/test/packetbb-test-suite.cc \
../src/network/test/pcap-file-test-suite.cc \
../src/network/test/sequence-number-test-suite.cc 

CC_DEPS += \
./src/network/test/buffer-test.d \
./src/network/test/drop-tail-queue-test-suite.d \
./src/network/test/error-model-test-suite.d \
./src/network/test/ipv6-address-test-suite.d \
./src/network/test/packet-metadata-test.d \
./src/network/test/packet-socket-apps-test-suite.d \
./src/network/test/packet-test-suite.d \
./src/network/test/packetbb-test-suite.d \
./src/network/test/pcap-file-test-suite.d \
./src/network/test/sequence-number-test-suite.d 

OBJS += \
./src/network/test/buffer-test.o \
./src/network/test/drop-tail-queue-test-suite.o \
./src/network/test/error-model-test-suite.o \
./src/network/test/ipv6-address-test-suite.o \
./src/network/test/packet-metadata-test.o \
./src/network/test/packet-socket-apps-test-suite.o \
./src/network/test/packet-test-suite.o \
./src/network/test/packetbb-test-suite.o \
./src/network/test/pcap-file-test-suite.o \
./src/network/test/sequence-number-test-suite.o 


# Each subdirectory must supply rules for building sources it contributes
src/network/test/%.o: ../src/network/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


