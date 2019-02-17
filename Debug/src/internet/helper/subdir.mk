################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/internet/helper/internet-stack-helper.cc \
../src/internet/helper/internet-trace-helper.cc \
../src/internet/helper/ipv4-address-helper.cc \
../src/internet/helper/ipv4-global-routing-helper.cc \
../src/internet/helper/ipv4-interface-container.cc \
../src/internet/helper/ipv4-list-routing-helper.cc \
../src/internet/helper/ipv4-routing-helper.cc \
../src/internet/helper/ipv4-static-routing-helper.cc \
../src/internet/helper/ipv6-address-helper.cc \
../src/internet/helper/ipv6-interface-container.cc \
../src/internet/helper/ipv6-list-routing-helper.cc \
../src/internet/helper/ipv6-routing-helper.cc \
../src/internet/helper/ipv6-static-routing-helper.cc \
../src/internet/helper/rip-helper.cc \
../src/internet/helper/ripng-helper.cc 

CC_DEPS += \
./src/internet/helper/internet-stack-helper.d \
./src/internet/helper/internet-trace-helper.d \
./src/internet/helper/ipv4-address-helper.d \
./src/internet/helper/ipv4-global-routing-helper.d \
./src/internet/helper/ipv4-interface-container.d \
./src/internet/helper/ipv4-list-routing-helper.d \
./src/internet/helper/ipv4-routing-helper.d \
./src/internet/helper/ipv4-static-routing-helper.d \
./src/internet/helper/ipv6-address-helper.d \
./src/internet/helper/ipv6-interface-container.d \
./src/internet/helper/ipv6-list-routing-helper.d \
./src/internet/helper/ipv6-routing-helper.d \
./src/internet/helper/ipv6-static-routing-helper.d \
./src/internet/helper/rip-helper.d \
./src/internet/helper/ripng-helper.d 

OBJS += \
./src/internet/helper/internet-stack-helper.o \
./src/internet/helper/internet-trace-helper.o \
./src/internet/helper/ipv4-address-helper.o \
./src/internet/helper/ipv4-global-routing-helper.o \
./src/internet/helper/ipv4-interface-container.o \
./src/internet/helper/ipv4-list-routing-helper.o \
./src/internet/helper/ipv4-routing-helper.o \
./src/internet/helper/ipv4-static-routing-helper.o \
./src/internet/helper/ipv6-address-helper.o \
./src/internet/helper/ipv6-interface-container.o \
./src/internet/helper/ipv6-list-routing-helper.o \
./src/internet/helper/ipv6-routing-helper.o \
./src/internet/helper/ipv6-static-routing-helper.o \
./src/internet/helper/rip-helper.o \
./src/internet/helper/ripng-helper.o 


# Each subdirectory must supply rules for building sources it contributes
src/internet/helper/%.o: ../src/internet/helper/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


