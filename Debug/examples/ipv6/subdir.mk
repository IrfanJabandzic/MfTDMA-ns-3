################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../examples/ipv6/fragmentation-ipv6-two-MTU.cc \
../examples/ipv6/fragmentation-ipv6.cc \
../examples/ipv6/icmpv6-redirect.cc \
../examples/ipv6/loose-routing-ipv6.cc \
../examples/ipv6/ping6.cc \
../examples/ipv6/radvd-two-prefix.cc \
../examples/ipv6/radvd.cc \
../examples/ipv6/test-ipv6.cc \
../examples/ipv6/wsn-ping6.cc 

CC_DEPS += \
./examples/ipv6/fragmentation-ipv6-two-MTU.d \
./examples/ipv6/fragmentation-ipv6.d \
./examples/ipv6/icmpv6-redirect.d \
./examples/ipv6/loose-routing-ipv6.d \
./examples/ipv6/ping6.d \
./examples/ipv6/radvd-two-prefix.d \
./examples/ipv6/radvd.d \
./examples/ipv6/test-ipv6.d \
./examples/ipv6/wsn-ping6.d 

OBJS += \
./examples/ipv6/fragmentation-ipv6-two-MTU.o \
./examples/ipv6/fragmentation-ipv6.o \
./examples/ipv6/icmpv6-redirect.o \
./examples/ipv6/loose-routing-ipv6.o \
./examples/ipv6/ping6.o \
./examples/ipv6/radvd-two-prefix.o \
./examples/ipv6/radvd.o \
./examples/ipv6/test-ipv6.o \
./examples/ipv6/wsn-ping6.o 


# Each subdirectory must supply rules for building sources it contributes
examples/ipv6/%.o: ../examples/ipv6/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


