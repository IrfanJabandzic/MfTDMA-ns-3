################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mesh/model/mesh-information-element-vector.cc \
../src/mesh/model/mesh-l2-routing-protocol.cc \
../src/mesh/model/mesh-point-device.cc \
../src/mesh/model/mesh-wifi-beacon.cc \
../src/mesh/model/mesh-wifi-interface-mac.cc 

CC_DEPS += \
./src/mesh/model/mesh-information-element-vector.d \
./src/mesh/model/mesh-l2-routing-protocol.d \
./src/mesh/model/mesh-point-device.d \
./src/mesh/model/mesh-wifi-beacon.d \
./src/mesh/model/mesh-wifi-interface-mac.d 

OBJS += \
./src/mesh/model/mesh-information-element-vector.o \
./src/mesh/model/mesh-l2-routing-protocol.o \
./src/mesh/model/mesh-point-device.o \
./src/mesh/model/mesh-wifi-beacon.o \
./src/mesh/model/mesh-wifi-interface-mac.o 


# Each subdirectory must supply rules for building sources it contributes
src/mesh/model/%.o: ../src/mesh/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


