################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/point-to-point-layout/model/point-to-point-dumbbell.cc \
../src/point-to-point-layout/model/point-to-point-grid.cc \
../src/point-to-point-layout/model/point-to-point-star.cc 

CC_DEPS += \
./src/point-to-point-layout/model/point-to-point-dumbbell.d \
./src/point-to-point-layout/model/point-to-point-grid.d \
./src/point-to-point-layout/model/point-to-point-star.d 

OBJS += \
./src/point-to-point-layout/model/point-to-point-dumbbell.o \
./src/point-to-point-layout/model/point-to-point-grid.o \
./src/point-to-point-layout/model/point-to-point-star.o 


# Each subdirectory must supply rules for building sources it contributes
src/point-to-point-layout/model/%.o: ../src/point-to-point-layout/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


