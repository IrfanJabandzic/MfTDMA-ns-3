################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../scratch/subdir/scratch-simulator-subdir.cc 

CC_DEPS += \
./scratch/subdir/scratch-simulator-subdir.d 

OBJS += \
./scratch/subdir/scratch-simulator-subdir.o 


# Each subdirectory must supply rules for building sources it contributes
scratch/subdir/%.o: ../scratch/subdir/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


