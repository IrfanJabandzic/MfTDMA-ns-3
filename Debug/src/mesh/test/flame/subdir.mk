################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mesh/test/flame/flame-regression.cc \
../src/mesh/test/flame/flame-test-suite.cc \
../src/mesh/test/flame/regression.cc 

CC_DEPS += \
./src/mesh/test/flame/flame-regression.d \
./src/mesh/test/flame/flame-test-suite.d \
./src/mesh/test/flame/regression.d 

OBJS += \
./src/mesh/test/flame/flame-regression.o \
./src/mesh/test/flame/flame-test-suite.o \
./src/mesh/test/flame/regression.o 


# Each subdirectory must supply rules for building sources it contributes
src/mesh/test/flame/%.o: ../src/mesh/test/flame/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


