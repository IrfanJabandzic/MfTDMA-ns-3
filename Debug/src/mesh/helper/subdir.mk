################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mesh/helper/mesh-helper.cc \
../src/mesh/helper/mesh-stack-installer.cc 

CC_DEPS += \
./src/mesh/helper/mesh-helper.d \
./src/mesh/helper/mesh-stack-installer.d 

OBJS += \
./src/mesh/helper/mesh-helper.o \
./src/mesh/helper/mesh-stack-installer.o 


# Each subdirectory must supply rules for building sources it contributes
src/mesh/helper/%.o: ../src/mesh/helper/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


