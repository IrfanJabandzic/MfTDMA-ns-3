################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mobility/examples/bonnmotion-ns2-example.cc \
../src/mobility/examples/main-grid-topology.cc \
../src/mobility/examples/main-random-topology.cc \
../src/mobility/examples/main-random-walk.cc \
../src/mobility/examples/mobility-trace-example.cc \
../src/mobility/examples/ns2-mobility-trace.cc 

CC_DEPS += \
./src/mobility/examples/bonnmotion-ns2-example.d \
./src/mobility/examples/main-grid-topology.d \
./src/mobility/examples/main-random-topology.d \
./src/mobility/examples/main-random-walk.d \
./src/mobility/examples/mobility-trace-example.d \
./src/mobility/examples/ns2-mobility-trace.d 

OBJS += \
./src/mobility/examples/bonnmotion-ns2-example.o \
./src/mobility/examples/main-grid-topology.o \
./src/mobility/examples/main-random-topology.o \
./src/mobility/examples/main-random-walk.o \
./src/mobility/examples/mobility-trace-example.o \
./src/mobility/examples/ns2-mobility-trace.o 


# Each subdirectory must supply rules for building sources it contributes
src/mobility/examples/%.o: ../src/mobility/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


