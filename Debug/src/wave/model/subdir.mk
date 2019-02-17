################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/wave/model/bsm-application.cc \
../src/wave/model/channel-coordinator.cc \
../src/wave/model/channel-manager.cc \
../src/wave/model/channel-scheduler.cc \
../src/wave/model/default-channel-scheduler.cc \
../src/wave/model/higher-tx-tag.cc \
../src/wave/model/ocb-wifi-mac.cc \
../src/wave/model/vendor-specific-action.cc \
../src/wave/model/vsa-manager.cc \
../src/wave/model/wave-mac-low.cc \
../src/wave/model/wave-net-device.cc 

CC_DEPS += \
./src/wave/model/bsm-application.d \
./src/wave/model/channel-coordinator.d \
./src/wave/model/channel-manager.d \
./src/wave/model/channel-scheduler.d \
./src/wave/model/default-channel-scheduler.d \
./src/wave/model/higher-tx-tag.d \
./src/wave/model/ocb-wifi-mac.d \
./src/wave/model/vendor-specific-action.d \
./src/wave/model/vsa-manager.d \
./src/wave/model/wave-mac-low.d \
./src/wave/model/wave-net-device.d 

OBJS += \
./src/wave/model/bsm-application.o \
./src/wave/model/channel-coordinator.o \
./src/wave/model/channel-manager.o \
./src/wave/model/channel-scheduler.o \
./src/wave/model/default-channel-scheduler.o \
./src/wave/model/higher-tx-tag.o \
./src/wave/model/ocb-wifi-mac.o \
./src/wave/model/vendor-specific-action.o \
./src/wave/model/vsa-manager.o \
./src/wave/model/wave-mac-low.o \
./src/wave/model/wave-net-device.o 


# Each subdirectory must supply rules for building sources it contributes
src/wave/model/%.o: ../src/wave/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


