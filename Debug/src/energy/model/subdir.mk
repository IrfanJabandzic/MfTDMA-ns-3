################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/energy/model/basic-energy-harvester.cc \
../src/energy/model/basic-energy-source.cc \
../src/energy/model/device-energy-model-container.cc \
../src/energy/model/device-energy-model.cc \
../src/energy/model/energy-harvester.cc \
../src/energy/model/energy-source.cc \
../src/energy/model/li-ion-energy-source.cc \
../src/energy/model/rv-battery-model.cc \
../src/energy/model/simple-device-energy-model.cc 

CC_DEPS += \
./src/energy/model/basic-energy-harvester.d \
./src/energy/model/basic-energy-source.d \
./src/energy/model/device-energy-model-container.d \
./src/energy/model/device-energy-model.d \
./src/energy/model/energy-harvester.d \
./src/energy/model/energy-source.d \
./src/energy/model/li-ion-energy-source.d \
./src/energy/model/rv-battery-model.d \
./src/energy/model/simple-device-energy-model.d 

OBJS += \
./src/energy/model/basic-energy-harvester.o \
./src/energy/model/basic-energy-source.o \
./src/energy/model/device-energy-model-container.o \
./src/energy/model/device-energy-model.o \
./src/energy/model/energy-harvester.o \
./src/energy/model/energy-source.o \
./src/energy/model/li-ion-energy-source.o \
./src/energy/model/rv-battery-model.o \
./src/energy/model/simple-device-energy-model.o 


# Each subdirectory must supply rules for building sources it contributes
src/energy/model/%.o: ../src/energy/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


