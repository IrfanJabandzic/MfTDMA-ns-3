################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/darpa/test/darpa-test-suite.cc 

CC_DEPS += \
./src/darpa/test/darpa-test-suite.d 

OBJS += \
./src/darpa/test/darpa-test-suite.o 


# Each subdirectory must supply rules for building sources it contributes
src/darpa/test/%.o: ../src/darpa/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


