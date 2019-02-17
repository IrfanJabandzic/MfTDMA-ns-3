################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/spectrum/test/spectrum-ideal-phy-test.cc \
../src/spectrum/test/spectrum-interference-test.cc \
../src/spectrum/test/spectrum-value-test.cc \
../src/spectrum/test/spectrum-waveform-generator-test.cc \
../src/spectrum/test/tv-helper-distribution-test.cc \
../src/spectrum/test/tv-spectrum-transmitter-test.cc 

CC_DEPS += \
./src/spectrum/test/spectrum-ideal-phy-test.d \
./src/spectrum/test/spectrum-interference-test.d \
./src/spectrum/test/spectrum-value-test.d \
./src/spectrum/test/spectrum-waveform-generator-test.d \
./src/spectrum/test/tv-helper-distribution-test.d \
./src/spectrum/test/tv-spectrum-transmitter-test.d 

OBJS += \
./src/spectrum/test/spectrum-ideal-phy-test.o \
./src/spectrum/test/spectrum-interference-test.o \
./src/spectrum/test/spectrum-value-test.o \
./src/spectrum/test/spectrum-waveform-generator-test.o \
./src/spectrum/test/tv-helper-distribution-test.o \
./src/spectrum/test/tv-spectrum-transmitter-test.o 


# Each subdirectory must supply rules for building sources it contributes
src/spectrum/test/%.o: ../src/spectrum/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


