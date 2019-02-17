################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mesh/test/dot11s/dot11s-test-suite.cc \
../src/mesh/test/dot11s/hwmp-proactive-regression.cc \
../src/mesh/test/dot11s/hwmp-reactive-regression.cc \
../src/mesh/test/dot11s/hwmp-simplest-regression.cc \
../src/mesh/test/dot11s/hwmp-target-flags-regression.cc \
../src/mesh/test/dot11s/pmp-regression.cc \
../src/mesh/test/dot11s/regression.cc 

CC_DEPS += \
./src/mesh/test/dot11s/dot11s-test-suite.d \
./src/mesh/test/dot11s/hwmp-proactive-regression.d \
./src/mesh/test/dot11s/hwmp-reactive-regression.d \
./src/mesh/test/dot11s/hwmp-simplest-regression.d \
./src/mesh/test/dot11s/hwmp-target-flags-regression.d \
./src/mesh/test/dot11s/pmp-regression.d \
./src/mesh/test/dot11s/regression.d 

OBJS += \
./src/mesh/test/dot11s/dot11s-test-suite.o \
./src/mesh/test/dot11s/hwmp-proactive-regression.o \
./src/mesh/test/dot11s/hwmp-reactive-regression.o \
./src/mesh/test/dot11s/hwmp-simplest-regression.o \
./src/mesh/test/dot11s/hwmp-target-flags-regression.o \
./src/mesh/test/dot11s/pmp-regression.o \
./src/mesh/test/dot11s/regression.o 


# Each subdirectory must supply rules for building sources it contributes
src/mesh/test/dot11s/%.o: ../src/mesh/test/dot11s/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


