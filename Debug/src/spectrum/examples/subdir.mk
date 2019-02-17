################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/spectrum/examples/adhoc-aloha-ideal-phy-matrix-propagation-loss-model.cc \
../src/spectrum/examples/adhoc-aloha-ideal-phy-with-microwave-oven.cc \
../src/spectrum/examples/adhoc-aloha-ideal-phy.cc \
../src/spectrum/examples/tv-trans-example.cc \
../src/spectrum/examples/tv-trans-regional-example.cc 

CC_DEPS += \
./src/spectrum/examples/adhoc-aloha-ideal-phy-matrix-propagation-loss-model.d \
./src/spectrum/examples/adhoc-aloha-ideal-phy-with-microwave-oven.d \
./src/spectrum/examples/adhoc-aloha-ideal-phy.d \
./src/spectrum/examples/tv-trans-example.d \
./src/spectrum/examples/tv-trans-regional-example.d 

OBJS += \
./src/spectrum/examples/adhoc-aloha-ideal-phy-matrix-propagation-loss-model.o \
./src/spectrum/examples/adhoc-aloha-ideal-phy-with-microwave-oven.o \
./src/spectrum/examples/adhoc-aloha-ideal-phy.o \
./src/spectrum/examples/tv-trans-example.o \
./src/spectrum/examples/tv-trans-regional-example.o 


# Each subdirectory must supply rules for building sources it contributes
src/spectrum/examples/%.o: ../src/spectrum/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


