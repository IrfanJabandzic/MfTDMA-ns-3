################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/wifi/helper/athstats-helper.cc \
../src/wifi/helper/ht-wifi-mac-helper.cc \
../src/wifi/helper/nqos-wifi-mac-helper.cc \
../src/wifi/helper/qos-wifi-mac-helper.cc \
../src/wifi/helper/spectrum-wifi-helper.cc \
../src/wifi/helper/vht-wifi-mac-helper.cc \
../src/wifi/helper/wifi-helper.cc \
../src/wifi/helper/wifi-mac-helper.cc \
../src/wifi/helper/wifi-radio-energy-model-helper.cc \
../src/wifi/helper/yans-wifi-helper.cc 

CC_DEPS += \
./src/wifi/helper/athstats-helper.d \
./src/wifi/helper/ht-wifi-mac-helper.d \
./src/wifi/helper/nqos-wifi-mac-helper.d \
./src/wifi/helper/qos-wifi-mac-helper.d \
./src/wifi/helper/spectrum-wifi-helper.d \
./src/wifi/helper/vht-wifi-mac-helper.d \
./src/wifi/helper/wifi-helper.d \
./src/wifi/helper/wifi-mac-helper.d \
./src/wifi/helper/wifi-radio-energy-model-helper.d \
./src/wifi/helper/yans-wifi-helper.d 

OBJS += \
./src/wifi/helper/athstats-helper.o \
./src/wifi/helper/ht-wifi-mac-helper.o \
./src/wifi/helper/nqos-wifi-mac-helper.o \
./src/wifi/helper/qos-wifi-mac-helper.o \
./src/wifi/helper/spectrum-wifi-helper.o \
./src/wifi/helper/vht-wifi-mac-helper.o \
./src/wifi/helper/wifi-helper.o \
./src/wifi/helper/wifi-mac-helper.o \
./src/wifi/helper/wifi-radio-energy-model-helper.o \
./src/wifi/helper/yans-wifi-helper.o 


# Each subdirectory must supply rules for building sources it contributes
src/wifi/helper/%.o: ../src/wifi/helper/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


