################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/antenna/model/angles.cc \
../src/antenna/model/antenna-model.cc \
../src/antenna/model/cosine-antenna-model.cc \
../src/antenna/model/isotropic-antenna-model.cc \
../src/antenna/model/parabolic-antenna-model.cc 

CC_DEPS += \
./src/antenna/model/angles.d \
./src/antenna/model/antenna-model.d \
./src/antenna/model/cosine-antenna-model.d \
./src/antenna/model/isotropic-antenna-model.d \
./src/antenna/model/parabolic-antenna-model.d 

OBJS += \
./src/antenna/model/angles.o \
./src/antenna/model/antenna-model.o \
./src/antenna/model/cosine-antenna-model.o \
./src/antenna/model/isotropic-antenna-model.o \
./src/antenna/model/parabolic-antenna-model.o 


# Each subdirectory must supply rules for building sources it contributes
src/antenna/model/%.o: ../src/antenna/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


