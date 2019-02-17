################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/test/ns3tc/fq-codel-queue-disc-test-suite.cc \
../src/test/ns3tc/pfifo-fast-queue-disc-test-suite.cc 

CC_DEPS += \
./src/test/ns3tc/fq-codel-queue-disc-test-suite.d \
./src/test/ns3tc/pfifo-fast-queue-disc-test-suite.d 

OBJS += \
./src/test/ns3tc/fq-codel-queue-disc-test-suite.o \
./src/test/ns3tc/pfifo-fast-queue-disc-test-suite.o 


# Each subdirectory must supply rules for building sources it contributes
src/test/ns3tc/%.o: ../src/test/ns3tc/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


