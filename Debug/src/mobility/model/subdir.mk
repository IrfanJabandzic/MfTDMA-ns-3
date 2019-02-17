################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mobility/model/box.cc \
../src/mobility/model/constant-acceleration-mobility-model.cc \
../src/mobility/model/constant-position-mobility-model.cc \
../src/mobility/model/constant-velocity-helper.cc \
../src/mobility/model/constant-velocity-mobility-model.cc \
../src/mobility/model/gauss-markov-mobility-model.cc \
../src/mobility/model/geographic-positions.cc \
../src/mobility/model/hierarchical-mobility-model.cc \
../src/mobility/model/mobility-model.cc \
../src/mobility/model/position-allocator.cc \
../src/mobility/model/random-direction-2d-mobility-model.cc \
../src/mobility/model/random-walk-2d-mobility-model.cc \
../src/mobility/model/random-waypoint-mobility-model.cc \
../src/mobility/model/rectangle.cc \
../src/mobility/model/steady-state-random-waypoint-mobility-model.cc \
../src/mobility/model/waypoint-mobility-model.cc \
../src/mobility/model/waypoint.cc 

CC_DEPS += \
./src/mobility/model/box.d \
./src/mobility/model/constant-acceleration-mobility-model.d \
./src/mobility/model/constant-position-mobility-model.d \
./src/mobility/model/constant-velocity-helper.d \
./src/mobility/model/constant-velocity-mobility-model.d \
./src/mobility/model/gauss-markov-mobility-model.d \
./src/mobility/model/geographic-positions.d \
./src/mobility/model/hierarchical-mobility-model.d \
./src/mobility/model/mobility-model.d \
./src/mobility/model/position-allocator.d \
./src/mobility/model/random-direction-2d-mobility-model.d \
./src/mobility/model/random-walk-2d-mobility-model.d \
./src/mobility/model/random-waypoint-mobility-model.d \
./src/mobility/model/rectangle.d \
./src/mobility/model/steady-state-random-waypoint-mobility-model.d \
./src/mobility/model/waypoint-mobility-model.d \
./src/mobility/model/waypoint.d 

OBJS += \
./src/mobility/model/box.o \
./src/mobility/model/constant-acceleration-mobility-model.o \
./src/mobility/model/constant-position-mobility-model.o \
./src/mobility/model/constant-velocity-helper.o \
./src/mobility/model/constant-velocity-mobility-model.o \
./src/mobility/model/gauss-markov-mobility-model.o \
./src/mobility/model/geographic-positions.o \
./src/mobility/model/hierarchical-mobility-model.o \
./src/mobility/model/mobility-model.o \
./src/mobility/model/position-allocator.o \
./src/mobility/model/random-direction-2d-mobility-model.o \
./src/mobility/model/random-walk-2d-mobility-model.o \
./src/mobility/model/random-waypoint-mobility-model.o \
./src/mobility/model/rectangle.o \
./src/mobility/model/steady-state-random-waypoint-mobility-model.o \
./src/mobility/model/waypoint-mobility-model.o \
./src/mobility/model/waypoint.o 


# Each subdirectory must supply rules for building sources it contributes
src/mobility/model/%.o: ../src/mobility/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


