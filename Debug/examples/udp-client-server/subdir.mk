################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../examples/udp-client-server/udp-client-server.cc \
../examples/udp-client-server/udp-trace-client-server.cc 

CC_DEPS += \
./examples/udp-client-server/udp-client-server.d \
./examples/udp-client-server/udp-trace-client-server.d 

OBJS += \
./examples/udp-client-server/udp-client-server.o \
./examples/udp-client-server/udp-trace-client-server.o 


# Each subdirectory must supply rules for building sources it contributes
examples/udp-client-server/%.o: ../examples/udp-client-server/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


