################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/core/test/attribute-test-suite.cc \
../src/core/test/build-profile-test-suite.cc \
../src/core/test/callback-test-suite.cc \
../src/core/test/command-line-test-suite.cc \
../src/core/test/config-test-suite.cc \
../src/core/test/event-garbage-collector-test-suite.cc \
../src/core/test/global-value-test-suite.cc \
../src/core/test/hash-test-suite.cc \
../src/core/test/int64x64-test-suite.cc \
../src/core/test/many-uniform-random-variables-one-get-value-call-test-suite.cc \
../src/core/test/names-test-suite.cc \
../src/core/test/object-test-suite.cc \
../src/core/test/one-uniform-random-variable-many-get-value-calls-test-suite.cc \
../src/core/test/ptr-test-suite.cc \
../src/core/test/random-variable-stream-test-suite.cc \
../src/core/test/rng-test-suite.cc \
../src/core/test/sample-test-suite.cc \
../src/core/test/simulator-test-suite.cc \
../src/core/test/threaded-test-suite.cc \
../src/core/test/time-test-suite.cc \
../src/core/test/timer-test-suite.cc \
../src/core/test/traced-callback-test-suite.cc \
../src/core/test/type-id-test-suite.cc \
../src/core/test/type-traits-test-suite.cc \
../src/core/test/watchdog-test-suite.cc 

CC_DEPS += \
./src/core/test/attribute-test-suite.d \
./src/core/test/build-profile-test-suite.d \
./src/core/test/callback-test-suite.d \
./src/core/test/command-line-test-suite.d \
./src/core/test/config-test-suite.d \
./src/core/test/event-garbage-collector-test-suite.d \
./src/core/test/global-value-test-suite.d \
./src/core/test/hash-test-suite.d \
./src/core/test/int64x64-test-suite.d \
./src/core/test/many-uniform-random-variables-one-get-value-call-test-suite.d \
./src/core/test/names-test-suite.d \
./src/core/test/object-test-suite.d \
./src/core/test/one-uniform-random-variable-many-get-value-calls-test-suite.d \
./src/core/test/ptr-test-suite.d \
./src/core/test/random-variable-stream-test-suite.d \
./src/core/test/rng-test-suite.d \
./src/core/test/sample-test-suite.d \
./src/core/test/simulator-test-suite.d \
./src/core/test/threaded-test-suite.d \
./src/core/test/time-test-suite.d \
./src/core/test/timer-test-suite.d \
./src/core/test/traced-callback-test-suite.d \
./src/core/test/type-id-test-suite.d \
./src/core/test/type-traits-test-suite.d \
./src/core/test/watchdog-test-suite.d 

OBJS += \
./src/core/test/attribute-test-suite.o \
./src/core/test/build-profile-test-suite.o \
./src/core/test/callback-test-suite.o \
./src/core/test/command-line-test-suite.o \
./src/core/test/config-test-suite.o \
./src/core/test/event-garbage-collector-test-suite.o \
./src/core/test/global-value-test-suite.o \
./src/core/test/hash-test-suite.o \
./src/core/test/int64x64-test-suite.o \
./src/core/test/many-uniform-random-variables-one-get-value-call-test-suite.o \
./src/core/test/names-test-suite.o \
./src/core/test/object-test-suite.o \
./src/core/test/one-uniform-random-variable-many-get-value-calls-test-suite.o \
./src/core/test/ptr-test-suite.o \
./src/core/test/random-variable-stream-test-suite.o \
./src/core/test/rng-test-suite.o \
./src/core/test/sample-test-suite.o \
./src/core/test/simulator-test-suite.o \
./src/core/test/threaded-test-suite.o \
./src/core/test/time-test-suite.o \
./src/core/test/timer-test-suite.o \
./src/core/test/traced-callback-test-suite.o \
./src/core/test/type-id-test-suite.o \
./src/core/test/type-traits-test-suite.o \
./src/core/test/watchdog-test-suite.o 


# Each subdirectory must supply rules for building sources it contributes
src/core/test/%.o: ../src/core/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


