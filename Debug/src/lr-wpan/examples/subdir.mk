################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/lr-wpan/examples/lr-wpan-data.cc \
../src/lr-wpan/examples/lr-wpan-error-distance-plot.cc \
../src/lr-wpan/examples/lr-wpan-error-model-plot.cc \
../src/lr-wpan/examples/lr-wpan-packet-print.cc \
../src/lr-wpan/examples/lr-wpan-phy-test.cc 

CC_DEPS += \
./src/lr-wpan/examples/lr-wpan-data.d \
./src/lr-wpan/examples/lr-wpan-error-distance-plot.d \
./src/lr-wpan/examples/lr-wpan-error-model-plot.d \
./src/lr-wpan/examples/lr-wpan-packet-print.d \
./src/lr-wpan/examples/lr-wpan-phy-test.d 

OBJS += \
./src/lr-wpan/examples/lr-wpan-data.o \
./src/lr-wpan/examples/lr-wpan-error-distance-plot.o \
./src/lr-wpan/examples/lr-wpan-error-model-plot.o \
./src/lr-wpan/examples/lr-wpan-packet-print.o \
./src/lr-wpan/examples/lr-wpan-phy-test.o 


# Each subdirectory must supply rules for building sources it contributes
src/lr-wpan/examples/%.o: ../src/lr-wpan/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


