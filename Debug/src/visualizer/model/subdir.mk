################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/visualizer/model/dummy-file-for-static-builds.cc \
../src/visualizer/model/pyviz.cc \
../src/visualizer/model/visual-simulator-impl.cc 

CC_DEPS += \
./src/visualizer/model/dummy-file-for-static-builds.d \
./src/visualizer/model/pyviz.d \
./src/visualizer/model/visual-simulator-impl.d 

OBJS += \
./src/visualizer/model/dummy-file-for-static-builds.o \
./src/visualizer/model/pyviz.o \
./src/visualizer/model/visual-simulator-impl.o 


# Each subdirectory must supply rules for building sources it contributes
src/visualizer/model/%.o: ../src/visualizer/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


