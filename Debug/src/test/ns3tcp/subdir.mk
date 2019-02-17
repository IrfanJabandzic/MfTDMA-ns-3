################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/test/ns3tcp/ns3tcp-cwnd-test-suite.cc \
../src/test/ns3tcp/ns3tcp-interop-test-suite.cc \
../src/test/ns3tcp/ns3tcp-loss-test-suite.cc \
../src/test/ns3tcp/ns3tcp-no-delay-test-suite.cc \
../src/test/ns3tcp/ns3tcp-socket-test-suite.cc \
../src/test/ns3tcp/ns3tcp-socket-writer.cc \
../src/test/ns3tcp/ns3tcp-state-test-suite.cc \
../src/test/ns3tcp/nsctcp-loss-test-suite.cc 

CC_DEPS += \
./src/test/ns3tcp/ns3tcp-cwnd-test-suite.d \
./src/test/ns3tcp/ns3tcp-interop-test-suite.d \
./src/test/ns3tcp/ns3tcp-loss-test-suite.d \
./src/test/ns3tcp/ns3tcp-no-delay-test-suite.d \
./src/test/ns3tcp/ns3tcp-socket-test-suite.d \
./src/test/ns3tcp/ns3tcp-socket-writer.d \
./src/test/ns3tcp/ns3tcp-state-test-suite.d \
./src/test/ns3tcp/nsctcp-loss-test-suite.d 

OBJS += \
./src/test/ns3tcp/ns3tcp-cwnd-test-suite.o \
./src/test/ns3tcp/ns3tcp-interop-test-suite.o \
./src/test/ns3tcp/ns3tcp-loss-test-suite.o \
./src/test/ns3tcp/ns3tcp-no-delay-test-suite.o \
./src/test/ns3tcp/ns3tcp-socket-test-suite.o \
./src/test/ns3tcp/ns3tcp-socket-writer.o \
./src/test/ns3tcp/ns3tcp-state-test-suite.o \
./src/test/ns3tcp/nsctcp-loss-test-suite.o 


# Each subdirectory must supply rules for building sources it contributes
src/test/ns3tcp/%.o: ../src/test/ns3tcp/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


