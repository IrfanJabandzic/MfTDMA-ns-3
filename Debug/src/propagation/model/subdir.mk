################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/propagation/model/cost231-propagation-loss-model.cc \
../src/propagation/model/itu-r-1411-los-propagation-loss-model.cc \
../src/propagation/model/itu-r-1411-nlos-over-rooftop-propagation-loss-model.cc \
../src/propagation/model/jakes-process.cc \
../src/propagation/model/jakes-propagation-loss-model.cc \
../src/propagation/model/kun-2600-mhz-propagation-loss-model.cc \
../src/propagation/model/okumura-hata-propagation-loss-model.cc \
../src/propagation/model/propagation-delay-model.cc \
../src/propagation/model/propagation-loss-model.cc 

CC_DEPS += \
./src/propagation/model/cost231-propagation-loss-model.d \
./src/propagation/model/itu-r-1411-los-propagation-loss-model.d \
./src/propagation/model/itu-r-1411-nlos-over-rooftop-propagation-loss-model.d \
./src/propagation/model/jakes-process.d \
./src/propagation/model/jakes-propagation-loss-model.d \
./src/propagation/model/kun-2600-mhz-propagation-loss-model.d \
./src/propagation/model/okumura-hata-propagation-loss-model.d \
./src/propagation/model/propagation-delay-model.d \
./src/propagation/model/propagation-loss-model.d 

OBJS += \
./src/propagation/model/cost231-propagation-loss-model.o \
./src/propagation/model/itu-r-1411-los-propagation-loss-model.o \
./src/propagation/model/itu-r-1411-nlos-over-rooftop-propagation-loss-model.o \
./src/propagation/model/jakes-process.o \
./src/propagation/model/jakes-propagation-loss-model.o \
./src/propagation/model/kun-2600-mhz-propagation-loss-model.o \
./src/propagation/model/okumura-hata-propagation-loss-model.o \
./src/propagation/model/propagation-delay-model.o \
./src/propagation/model/propagation-loss-model.o 


# Each subdirectory must supply rules for building sources it contributes
src/propagation/model/%.o: ../src/propagation/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


