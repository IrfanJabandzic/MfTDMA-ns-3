################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/core/examples/command-line-example.cc \
../src/core/examples/hash-example.cc \
../src/core/examples/main-callback.cc \
../src/core/examples/main-ptr.cc \
../src/core/examples/main-random-variable-stream.cc \
../src/core/examples/main-random-variable.cc \
../src/core/examples/main-test-sync.cc \
../src/core/examples/sample-log-time-format.cc \
../src/core/examples/sample-random-variable-stream.cc \
../src/core/examples/sample-random-variable.cc \
../src/core/examples/sample-simulator.cc \
../src/core/examples/test-string-value-formatting.cc 

CC_DEPS += \
./src/core/examples/command-line-example.d \
./src/core/examples/hash-example.d \
./src/core/examples/main-callback.d \
./src/core/examples/main-ptr.d \
./src/core/examples/main-random-variable-stream.d \
./src/core/examples/main-random-variable.d \
./src/core/examples/main-test-sync.d \
./src/core/examples/sample-log-time-format.d \
./src/core/examples/sample-random-variable-stream.d \
./src/core/examples/sample-random-variable.d \
./src/core/examples/sample-simulator.d \
./src/core/examples/test-string-value-formatting.d 

OBJS += \
./src/core/examples/command-line-example.o \
./src/core/examples/hash-example.o \
./src/core/examples/main-callback.o \
./src/core/examples/main-ptr.o \
./src/core/examples/main-random-variable-stream.o \
./src/core/examples/main-random-variable.o \
./src/core/examples/main-test-sync.o \
./src/core/examples/sample-log-time-format.o \
./src/core/examples/sample-random-variable-stream.o \
./src/core/examples/sample-random-variable.o \
./src/core/examples/sample-simulator.o \
./src/core/examples/test-string-value-formatting.o 


# Each subdirectory must supply rules for building sources it contributes
src/core/examples/%.o: ../src/core/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


