################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/olsr/test/bug780-test.cc \
../src/olsr/test/hello-regression-test.cc \
../src/olsr/test/olsr-header-test-suite.cc \
../src/olsr/test/olsr-routing-protocol-test-suite.cc \
../src/olsr/test/regression-test-suite.cc \
../src/olsr/test/tc-regression-test.cc 

CC_DEPS += \
./src/olsr/test/bug780-test.d \
./src/olsr/test/hello-regression-test.d \
./src/olsr/test/olsr-header-test-suite.d \
./src/olsr/test/olsr-routing-protocol-test-suite.d \
./src/olsr/test/regression-test-suite.d \
./src/olsr/test/tc-regression-test.d 

OBJS += \
./src/olsr/test/bug780-test.o \
./src/olsr/test/hello-regression-test.o \
./src/olsr/test/olsr-header-test-suite.o \
./src/olsr/test/olsr-routing-protocol-test-suite.o \
./src/olsr/test/regression-test-suite.o \
./src/olsr/test/tc-regression-test.o 


# Each subdirectory must supply rules for building sources it contributes
src/olsr/test/%.o: ../src/olsr/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


