################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/propagation/test/itu-r-1411-los-test-suite.cc \
../src/propagation/test/itu-r-1411-nlos-over-rooftop-test-suite.cc \
../src/propagation/test/kun-2600-mhz-test-suite.cc \
../src/propagation/test/okumura-hata-test-suite.cc \
../src/propagation/test/propagation-loss-model-test-suite.cc 

CC_DEPS += \
./src/propagation/test/itu-r-1411-los-test-suite.d \
./src/propagation/test/itu-r-1411-nlos-over-rooftop-test-suite.d \
./src/propagation/test/kun-2600-mhz-test-suite.d \
./src/propagation/test/okumura-hata-test-suite.d \
./src/propagation/test/propagation-loss-model-test-suite.d 

OBJS += \
./src/propagation/test/itu-r-1411-los-test-suite.o \
./src/propagation/test/itu-r-1411-nlos-over-rooftop-test-suite.o \
./src/propagation/test/kun-2600-mhz-test-suite.o \
./src/propagation/test/okumura-hata-test-suite.o \
./src/propagation/test/propagation-loss-model-test-suite.o 


# Each subdirectory must supply rules for building sources it contributes
src/propagation/test/%.o: ../src/propagation/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


