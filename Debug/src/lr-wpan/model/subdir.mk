################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/lr-wpan/model/lr-wpan-csmaca.cc \
../src/lr-wpan/model/lr-wpan-error-model.cc \
../src/lr-wpan/model/lr-wpan-interference-helper.cc \
../src/lr-wpan/model/lr-wpan-lqi-tag.cc \
../src/lr-wpan/model/lr-wpan-mac-header.cc \
../src/lr-wpan/model/lr-wpan-mac-trailer.cc \
../src/lr-wpan/model/lr-wpan-mac.cc \
../src/lr-wpan/model/lr-wpan-net-device.cc \
../src/lr-wpan/model/lr-wpan-phy.cc \
../src/lr-wpan/model/lr-wpan-spectrum-signal-parameters.cc \
../src/lr-wpan/model/lr-wpan-spectrum-value-helper.cc 

CC_DEPS += \
./src/lr-wpan/model/lr-wpan-csmaca.d \
./src/lr-wpan/model/lr-wpan-error-model.d \
./src/lr-wpan/model/lr-wpan-interference-helper.d \
./src/lr-wpan/model/lr-wpan-lqi-tag.d \
./src/lr-wpan/model/lr-wpan-mac-header.d \
./src/lr-wpan/model/lr-wpan-mac-trailer.d \
./src/lr-wpan/model/lr-wpan-mac.d \
./src/lr-wpan/model/lr-wpan-net-device.d \
./src/lr-wpan/model/lr-wpan-phy.d \
./src/lr-wpan/model/lr-wpan-spectrum-signal-parameters.d \
./src/lr-wpan/model/lr-wpan-spectrum-value-helper.d 

OBJS += \
./src/lr-wpan/model/lr-wpan-csmaca.o \
./src/lr-wpan/model/lr-wpan-error-model.o \
./src/lr-wpan/model/lr-wpan-interference-helper.o \
./src/lr-wpan/model/lr-wpan-lqi-tag.o \
./src/lr-wpan/model/lr-wpan-mac-header.o \
./src/lr-wpan/model/lr-wpan-mac-trailer.o \
./src/lr-wpan/model/lr-wpan-mac.o \
./src/lr-wpan/model/lr-wpan-net-device.o \
./src/lr-wpan/model/lr-wpan-phy.o \
./src/lr-wpan/model/lr-wpan-spectrum-signal-parameters.o \
./src/lr-wpan/model/lr-wpan-spectrum-value-helper.o 


# Each subdirectory must supply rules for building sources it contributes
src/lr-wpan/model/%.o: ../src/lr-wpan/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


