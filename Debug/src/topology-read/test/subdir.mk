################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/topology-read/test/rocketfuel-topology-reader-test-suite.cc 

CC_DEPS += \
./src/topology-read/test/rocketfuel-topology-reader-test-suite.d 

OBJS += \
./src/topology-read/test/rocketfuel-topology-reader-test-suite.o 


# Each subdirectory must supply rules for building sources it contributes
src/topology-read/test/%.o: ../src/topology-read/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


