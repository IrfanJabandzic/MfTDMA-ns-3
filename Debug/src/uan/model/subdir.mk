################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/uan/model/acoustic-modem-energy-model.cc \
../src/uan/model/uan-address.cc \
../src/uan/model/uan-channel.cc \
../src/uan/model/uan-header-common.cc \
../src/uan/model/uan-header-rc.cc \
../src/uan/model/uan-mac-aloha.cc \
../src/uan/model/uan-mac-cw.cc \
../src/uan/model/uan-mac-rc-gw.cc \
../src/uan/model/uan-mac-rc.cc \
../src/uan/model/uan-mac.cc \
../src/uan/model/uan-net-device.cc \
../src/uan/model/uan-noise-model-default.cc \
../src/uan/model/uan-noise-model.cc \
../src/uan/model/uan-phy-dual.cc \
../src/uan/model/uan-phy-gen.cc \
../src/uan/model/uan-phy.cc \
../src/uan/model/uan-prop-model-ideal.cc \
../src/uan/model/uan-prop-model-thorp.cc \
../src/uan/model/uan-prop-model.cc \
../src/uan/model/uan-transducer-hd.cc \
../src/uan/model/uan-transducer.cc \
../src/uan/model/uan-tx-mode.cc 

CC_DEPS += \
./src/uan/model/acoustic-modem-energy-model.d \
./src/uan/model/uan-address.d \
./src/uan/model/uan-channel.d \
./src/uan/model/uan-header-common.d \
./src/uan/model/uan-header-rc.d \
./src/uan/model/uan-mac-aloha.d \
./src/uan/model/uan-mac-cw.d \
./src/uan/model/uan-mac-rc-gw.d \
./src/uan/model/uan-mac-rc.d \
./src/uan/model/uan-mac.d \
./src/uan/model/uan-net-device.d \
./src/uan/model/uan-noise-model-default.d \
./src/uan/model/uan-noise-model.d \
./src/uan/model/uan-phy-dual.d \
./src/uan/model/uan-phy-gen.d \
./src/uan/model/uan-phy.d \
./src/uan/model/uan-prop-model-ideal.d \
./src/uan/model/uan-prop-model-thorp.d \
./src/uan/model/uan-prop-model.d \
./src/uan/model/uan-transducer-hd.d \
./src/uan/model/uan-transducer.d \
./src/uan/model/uan-tx-mode.d 

OBJS += \
./src/uan/model/acoustic-modem-energy-model.o \
./src/uan/model/uan-address.o \
./src/uan/model/uan-channel.o \
./src/uan/model/uan-header-common.o \
./src/uan/model/uan-header-rc.o \
./src/uan/model/uan-mac-aloha.o \
./src/uan/model/uan-mac-cw.o \
./src/uan/model/uan-mac-rc-gw.o \
./src/uan/model/uan-mac-rc.o \
./src/uan/model/uan-mac.o \
./src/uan/model/uan-net-device.o \
./src/uan/model/uan-noise-model-default.o \
./src/uan/model/uan-noise-model.o \
./src/uan/model/uan-phy-dual.o \
./src/uan/model/uan-phy-gen.o \
./src/uan/model/uan-phy.o \
./src/uan/model/uan-prop-model-ideal.o \
./src/uan/model/uan-prop-model-thorp.o \
./src/uan/model/uan-prop-model.o \
./src/uan/model/uan-transducer-hd.o \
./src/uan/model/uan-transducer.o \
./src/uan/model/uan-tx-mode.o 


# Each subdirectory must supply rules for building sources it contributes
src/uan/model/%.o: ../src/uan/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


