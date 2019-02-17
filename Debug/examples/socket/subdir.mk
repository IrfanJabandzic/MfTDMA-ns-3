################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../examples/socket/socket-bound-static-routing.cc \
../examples/socket/socket-bound-tcp-static-routing.cc \
../examples/socket/socket-options-ipv4.cc \
../examples/socket/socket-options-ipv6.cc 

CC_DEPS += \
./examples/socket/socket-bound-static-routing.d \
./examples/socket/socket-bound-tcp-static-routing.d \
./examples/socket/socket-options-ipv4.d \
./examples/socket/socket-options-ipv6.d 

OBJS += \
./examples/socket/socket-bound-static-routing.o \
./examples/socket/socket-bound-tcp-static-routing.o \
./examples/socket/socket-options-ipv4.o \
./examples/socket/socket-options-ipv6.o 


# Each subdirectory must supply rules for building sources it contributes
examples/socket/%.o: ../examples/socket/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


