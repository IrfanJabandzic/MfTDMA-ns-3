################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/spectrum/helper/adhoc-aloha-noack-ideal-phy-helper.cc \
../src/spectrum/helper/spectrum-analyzer-helper.cc \
../src/spectrum/helper/spectrum-helper.cc \
../src/spectrum/helper/tv-spectrum-transmitter-helper.cc \
../src/spectrum/helper/waveform-generator-helper.cc 

CC_DEPS += \
./src/spectrum/helper/adhoc-aloha-noack-ideal-phy-helper.d \
./src/spectrum/helper/spectrum-analyzer-helper.d \
./src/spectrum/helper/spectrum-helper.d \
./src/spectrum/helper/tv-spectrum-transmitter-helper.d \
./src/spectrum/helper/waveform-generator-helper.d 

OBJS += \
./src/spectrum/helper/adhoc-aloha-noack-ideal-phy-helper.o \
./src/spectrum/helper/spectrum-analyzer-helper.o \
./src/spectrum/helper/spectrum-helper.o \
./src/spectrum/helper/tv-spectrum-transmitter-helper.o \
./src/spectrum/helper/waveform-generator-helper.o 


# Each subdirectory must supply rules for building sources it contributes
src/spectrum/helper/%.o: ../src/spectrum/helper/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


