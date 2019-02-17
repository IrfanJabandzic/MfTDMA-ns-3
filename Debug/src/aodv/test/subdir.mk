################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/aodv/test/aodv-id-cache-test-suite.cc \
../src/aodv/test/aodv-regression.cc \
../src/aodv/test/aodv-test-suite.cc \
../src/aodv/test/bug-772.cc \
../src/aodv/test/loopback.cc 

CC_DEPS += \
./src/aodv/test/aodv-id-cache-test-suite.d \
./src/aodv/test/aodv-regression.d \
./src/aodv/test/aodv-test-suite.d \
./src/aodv/test/bug-772.d \
./src/aodv/test/loopback.d 

OBJS += \
./src/aodv/test/aodv-id-cache-test-suite.o \
./src/aodv/test/aodv-regression.o \
./src/aodv/test/aodv-test-suite.o \
./src/aodv/test/bug-772.o \
./src/aodv/test/loopback.o 


# Each subdirectory must supply rules for building sources it contributes
src/aodv/test/%.o: ../src/aodv/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


