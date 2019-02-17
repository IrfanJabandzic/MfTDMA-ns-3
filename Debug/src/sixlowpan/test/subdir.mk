################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/sixlowpan/test/sixlowpan-fragmentation-test.cc \
../src/sixlowpan/test/sixlowpan-hc1-test.cc \
../src/sixlowpan/test/sixlowpan-iphc-test.cc 

CC_DEPS += \
./src/sixlowpan/test/sixlowpan-fragmentation-test.d \
./src/sixlowpan/test/sixlowpan-hc1-test.d \
./src/sixlowpan/test/sixlowpan-iphc-test.d 

OBJS += \
./src/sixlowpan/test/sixlowpan-fragmentation-test.o \
./src/sixlowpan/test/sixlowpan-hc1-test.o \
./src/sixlowpan/test/sixlowpan-iphc-test.o 


# Each subdirectory must supply rules for building sources it contributes
src/sixlowpan/test/%.o: ../src/sixlowpan/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


