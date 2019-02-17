################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/stats/test/average-test-suite.cc \
../src/stats/test/basic-data-calculators-test-suite.cc \
../src/stats/test/double-probe-test-suite.cc 

CC_DEPS += \
./src/stats/test/average-test-suite.d \
./src/stats/test/basic-data-calculators-test-suite.d \
./src/stats/test/double-probe-test-suite.d 

OBJS += \
./src/stats/test/average-test-suite.o \
./src/stats/test/basic-data-calculators-test-suite.o \
./src/stats/test/double-probe-test-suite.o 


# Each subdirectory must supply rules for building sources it contributes
src/stats/test/%.o: ../src/stats/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


