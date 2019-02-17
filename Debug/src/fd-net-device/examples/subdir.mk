################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/fd-net-device/examples/dummy-network.cc \
../src/fd-net-device/examples/fd-emu-onoff.cc \
../src/fd-net-device/examples/fd-emu-ping.cc \
../src/fd-net-device/examples/fd-emu-udp-echo.cc \
../src/fd-net-device/examples/fd-planetlab-ping.cc \
../src/fd-net-device/examples/fd-tap-ping.cc \
../src/fd-net-device/examples/fd-tap-ping6.cc \
../src/fd-net-device/examples/fd2fd-onoff.cc \
../src/fd-net-device/examples/realtime-dummy-network.cc \
../src/fd-net-device/examples/realtime-fd2fd-onoff.cc 

CC_DEPS += \
./src/fd-net-device/examples/dummy-network.d \
./src/fd-net-device/examples/fd-emu-onoff.d \
./src/fd-net-device/examples/fd-emu-ping.d \
./src/fd-net-device/examples/fd-emu-udp-echo.d \
./src/fd-net-device/examples/fd-planetlab-ping.d \
./src/fd-net-device/examples/fd-tap-ping.d \
./src/fd-net-device/examples/fd-tap-ping6.d \
./src/fd-net-device/examples/fd2fd-onoff.d \
./src/fd-net-device/examples/realtime-dummy-network.d \
./src/fd-net-device/examples/realtime-fd2fd-onoff.d 

OBJS += \
./src/fd-net-device/examples/dummy-network.o \
./src/fd-net-device/examples/fd-emu-onoff.o \
./src/fd-net-device/examples/fd-emu-ping.o \
./src/fd-net-device/examples/fd-emu-udp-echo.o \
./src/fd-net-device/examples/fd-planetlab-ping.o \
./src/fd-net-device/examples/fd-tap-ping.o \
./src/fd-net-device/examples/fd-tap-ping6.o \
./src/fd-net-device/examples/fd2fd-onoff.o \
./src/fd-net-device/examples/realtime-dummy-network.o \
./src/fd-net-device/examples/realtime-fd2fd-onoff.o 


# Each subdirectory must supply rules for building sources it contributes
src/fd-net-device/examples/%.o: ../src/fd-net-device/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


