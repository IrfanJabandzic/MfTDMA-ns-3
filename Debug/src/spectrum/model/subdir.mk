################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/spectrum/model/aloha-noack-mac-header.cc \
../src/spectrum/model/aloha-noack-net-device.cc \
../src/spectrum/model/constant-spectrum-propagation-loss.cc \
../src/spectrum/model/friis-spectrum-propagation-loss.cc \
../src/spectrum/model/half-duplex-ideal-phy-signal-parameters.cc \
../src/spectrum/model/half-duplex-ideal-phy.cc \
../src/spectrum/model/microwave-oven-spectrum-value-helper.cc \
../src/spectrum/model/multi-model-spectrum-channel.cc \
../src/spectrum/model/non-communicating-net-device.cc \
../src/spectrum/model/single-model-spectrum-channel.cc \
../src/spectrum/model/spectrum-analyzer.cc \
../src/spectrum/model/spectrum-channel.cc \
../src/spectrum/model/spectrum-converter.cc \
../src/spectrum/model/spectrum-error-model.cc \
../src/spectrum/model/spectrum-interference.cc \
../src/spectrum/model/spectrum-model-300kHz-300GHz-log.cc \
../src/spectrum/model/spectrum-model-ism2400MHz-res1MHz.cc \
../src/spectrum/model/spectrum-model.cc \
../src/spectrum/model/spectrum-phy.cc \
../src/spectrum/model/spectrum-propagation-loss-model.cc \
../src/spectrum/model/spectrum-signal-parameters.cc \
../src/spectrum/model/spectrum-value.cc \
../src/spectrum/model/tv-spectrum-transmitter.cc \
../src/spectrum/model/waveform-generator.cc \
../src/spectrum/model/wifi-spectrum-value-helper.cc 

CC_DEPS += \
./src/spectrum/model/aloha-noack-mac-header.d \
./src/spectrum/model/aloha-noack-net-device.d \
./src/spectrum/model/constant-spectrum-propagation-loss.d \
./src/spectrum/model/friis-spectrum-propagation-loss.d \
./src/spectrum/model/half-duplex-ideal-phy-signal-parameters.d \
./src/spectrum/model/half-duplex-ideal-phy.d \
./src/spectrum/model/microwave-oven-spectrum-value-helper.d \
./src/spectrum/model/multi-model-spectrum-channel.d \
./src/spectrum/model/non-communicating-net-device.d \
./src/spectrum/model/single-model-spectrum-channel.d \
./src/spectrum/model/spectrum-analyzer.d \
./src/spectrum/model/spectrum-channel.d \
./src/spectrum/model/spectrum-converter.d \
./src/spectrum/model/spectrum-error-model.d \
./src/spectrum/model/spectrum-interference.d \
./src/spectrum/model/spectrum-model-300kHz-300GHz-log.d \
./src/spectrum/model/spectrum-model-ism2400MHz-res1MHz.d \
./src/spectrum/model/spectrum-model.d \
./src/spectrum/model/spectrum-phy.d \
./src/spectrum/model/spectrum-propagation-loss-model.d \
./src/spectrum/model/spectrum-signal-parameters.d \
./src/spectrum/model/spectrum-value.d \
./src/spectrum/model/tv-spectrum-transmitter.d \
./src/spectrum/model/waveform-generator.d \
./src/spectrum/model/wifi-spectrum-value-helper.d 

OBJS += \
./src/spectrum/model/aloha-noack-mac-header.o \
./src/spectrum/model/aloha-noack-net-device.o \
./src/spectrum/model/constant-spectrum-propagation-loss.o \
./src/spectrum/model/friis-spectrum-propagation-loss.o \
./src/spectrum/model/half-duplex-ideal-phy-signal-parameters.o \
./src/spectrum/model/half-duplex-ideal-phy.o \
./src/spectrum/model/microwave-oven-spectrum-value-helper.o \
./src/spectrum/model/multi-model-spectrum-channel.o \
./src/spectrum/model/non-communicating-net-device.o \
./src/spectrum/model/single-model-spectrum-channel.o \
./src/spectrum/model/spectrum-analyzer.o \
./src/spectrum/model/spectrum-channel.o \
./src/spectrum/model/spectrum-converter.o \
./src/spectrum/model/spectrum-error-model.o \
./src/spectrum/model/spectrum-interference.o \
./src/spectrum/model/spectrum-model-300kHz-300GHz-log.o \
./src/spectrum/model/spectrum-model-ism2400MHz-res1MHz.o \
./src/spectrum/model/spectrum-model.o \
./src/spectrum/model/spectrum-phy.o \
./src/spectrum/model/spectrum-propagation-loss-model.o \
./src/spectrum/model/spectrum-signal-parameters.o \
./src/spectrum/model/spectrum-value.o \
./src/spectrum/model/tv-spectrum-transmitter.o \
./src/spectrum/model/waveform-generator.o \
./src/spectrum/model/wifi-spectrum-value-helper.o 


# Each subdirectory must supply rules for building sources it contributes
src/spectrum/model/%.o: ../src/spectrum/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


