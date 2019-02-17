################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/dsr/model/dsr-errorbuff.cc \
../src/dsr/model/dsr-fs-header.cc \
../src/dsr/model/dsr-gratuitous-reply-table.cc \
../src/dsr/model/dsr-maintain-buff.cc \
../src/dsr/model/dsr-network-queue.cc \
../src/dsr/model/dsr-option-header.cc \
../src/dsr/model/dsr-options.cc \
../src/dsr/model/dsr-passive-buff.cc \
../src/dsr/model/dsr-rcache.cc \
../src/dsr/model/dsr-routing.cc \
../src/dsr/model/dsr-rreq-table.cc \
../src/dsr/model/dsr-rsendbuff.cc 

CC_DEPS += \
./src/dsr/model/dsr-errorbuff.d \
./src/dsr/model/dsr-fs-header.d \
./src/dsr/model/dsr-gratuitous-reply-table.d \
./src/dsr/model/dsr-maintain-buff.d \
./src/dsr/model/dsr-network-queue.d \
./src/dsr/model/dsr-option-header.d \
./src/dsr/model/dsr-options.d \
./src/dsr/model/dsr-passive-buff.d \
./src/dsr/model/dsr-rcache.d \
./src/dsr/model/dsr-routing.d \
./src/dsr/model/dsr-rreq-table.d \
./src/dsr/model/dsr-rsendbuff.d 

OBJS += \
./src/dsr/model/dsr-errorbuff.o \
./src/dsr/model/dsr-fs-header.o \
./src/dsr/model/dsr-gratuitous-reply-table.o \
./src/dsr/model/dsr-maintain-buff.o \
./src/dsr/model/dsr-network-queue.o \
./src/dsr/model/dsr-option-header.o \
./src/dsr/model/dsr-options.o \
./src/dsr/model/dsr-passive-buff.o \
./src/dsr/model/dsr-rcache.o \
./src/dsr/model/dsr-routing.o \
./src/dsr/model/dsr-rreq-table.o \
./src/dsr/model/dsr-rsendbuff.o 


# Each subdirectory must supply rules for building sources it contributes
src/dsr/model/%.o: ../src/dsr/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


