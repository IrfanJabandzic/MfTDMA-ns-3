################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/wave/helper/wave-bsm-helper.cc \
../src/wave/helper/wave-bsm-stats.cc \
../src/wave/helper/wave-helper.cc \
../src/wave/helper/wave-mac-helper.cc \
../src/wave/helper/wifi-80211p-helper.cc 

CC_DEPS += \
./src/wave/helper/wave-bsm-helper.d \
./src/wave/helper/wave-bsm-stats.d \
./src/wave/helper/wave-helper.d \
./src/wave/helper/wave-mac-helper.d \
./src/wave/helper/wifi-80211p-helper.d 

OBJS += \
./src/wave/helper/wave-bsm-helper.o \
./src/wave/helper/wave-bsm-stats.o \
./src/wave/helper/wave-helper.o \
./src/wave/helper/wave-mac-helper.o \
./src/wave/helper/wifi-80211p-helper.o 


# Each subdirectory must supply rules for building sources it contributes
src/wave/helper/%.o: ../src/wave/helper/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


