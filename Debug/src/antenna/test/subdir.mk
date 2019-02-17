################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/antenna/test/test-angles.cc \
../src/antenna/test/test-cosine-antenna.cc \
../src/antenna/test/test-degrees-radians.cc \
../src/antenna/test/test-isotropic-antenna.cc \
../src/antenna/test/test-parabolic-antenna.cc 

CC_DEPS += \
./src/antenna/test/test-angles.d \
./src/antenna/test/test-cosine-antenna.d \
./src/antenna/test/test-degrees-radians.d \
./src/antenna/test/test-isotropic-antenna.d \
./src/antenna/test/test-parabolic-antenna.d 

OBJS += \
./src/antenna/test/test-angles.o \
./src/antenna/test/test-cosine-antenna.o \
./src/antenna/test/test-degrees-radians.o \
./src/antenna/test/test-isotropic-antenna.o \
./src/antenna/test/test-parabolic-antenna.o 


# Each subdirectory must supply rules for building sources it contributes
src/antenna/test/%.o: ../src/antenna/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


