################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../examples/routing/dynamic-global-routing.cc \
../examples/routing/global-injection-slash32.cc \
../examples/routing/global-routing-multi-switch-plus-router.cc \
../examples/routing/global-routing-slash32.cc \
../examples/routing/manet-routing-compare.cc \
../examples/routing/mixed-global-routing.cc \
../examples/routing/rip-simple-network.cc \
../examples/routing/ripng-simple-network.cc \
../examples/routing/simple-alternate-routing.cc \
../examples/routing/simple-global-routing.cc \
../examples/routing/simple-routing-ping6.cc \
../examples/routing/static-routing-slash32.cc 

CC_DEPS += \
./examples/routing/dynamic-global-routing.d \
./examples/routing/global-injection-slash32.d \
./examples/routing/global-routing-multi-switch-plus-router.d \
./examples/routing/global-routing-slash32.d \
./examples/routing/manet-routing-compare.d \
./examples/routing/mixed-global-routing.d \
./examples/routing/rip-simple-network.d \
./examples/routing/ripng-simple-network.d \
./examples/routing/simple-alternate-routing.d \
./examples/routing/simple-global-routing.d \
./examples/routing/simple-routing-ping6.d \
./examples/routing/static-routing-slash32.d 

OBJS += \
./examples/routing/dynamic-global-routing.o \
./examples/routing/global-injection-slash32.o \
./examples/routing/global-routing-multi-switch-plus-router.o \
./examples/routing/global-routing-slash32.o \
./examples/routing/manet-routing-compare.o \
./examples/routing/mixed-global-routing.o \
./examples/routing/rip-simple-network.o \
./examples/routing/ripng-simple-network.o \
./examples/routing/simple-alternate-routing.o \
./examples/routing/simple-global-routing.o \
./examples/routing/simple-routing-ping6.o \
./examples/routing/static-routing-slash32.o 


# Each subdirectory must supply rules for building sources it contributes
examples/routing/%.o: ../examples/routing/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


