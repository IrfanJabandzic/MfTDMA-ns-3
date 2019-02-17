################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/traffic-control/model/codel-queue-disc.cc \
../src/traffic-control/model/fq-codel-queue-disc.cc \
../src/traffic-control/model/mq-queue-disc.cc \
../src/traffic-control/model/packet-filter.cc \
../src/traffic-control/model/pfifo-fast-queue-disc.cc \
../src/traffic-control/model/pie-queue-disc.cc \
../src/traffic-control/model/queue-disc.cc \
../src/traffic-control/model/red-queue-disc.cc \
../src/traffic-control/model/traffic-control-layer.cc 

CC_DEPS += \
./src/traffic-control/model/codel-queue-disc.d \
./src/traffic-control/model/fq-codel-queue-disc.d \
./src/traffic-control/model/mq-queue-disc.d \
./src/traffic-control/model/packet-filter.d \
./src/traffic-control/model/pfifo-fast-queue-disc.d \
./src/traffic-control/model/pie-queue-disc.d \
./src/traffic-control/model/queue-disc.d \
./src/traffic-control/model/red-queue-disc.d \
./src/traffic-control/model/traffic-control-layer.d 

OBJS += \
./src/traffic-control/model/codel-queue-disc.o \
./src/traffic-control/model/fq-codel-queue-disc.o \
./src/traffic-control/model/mq-queue-disc.o \
./src/traffic-control/model/packet-filter.o \
./src/traffic-control/model/pfifo-fast-queue-disc.o \
./src/traffic-control/model/pie-queue-disc.o \
./src/traffic-control/model/queue-disc.o \
./src/traffic-control/model/red-queue-disc.o \
./src/traffic-control/model/traffic-control-layer.o 


# Each subdirectory must supply rules for building sources it contributes
src/traffic-control/model/%.o: ../src/traffic-control/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


