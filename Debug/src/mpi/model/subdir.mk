################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/mpi/model/distributed-simulator-impl.cc \
../src/mpi/model/granted-time-window-mpi-interface.cc \
../src/mpi/model/mpi-interface.cc \
../src/mpi/model/mpi-receiver.cc \
../src/mpi/model/null-message-mpi-interface.cc \
../src/mpi/model/null-message-simulator-impl.cc \
../src/mpi/model/remote-channel-bundle-manager.cc \
../src/mpi/model/remote-channel-bundle.cc 

CC_DEPS += \
./src/mpi/model/distributed-simulator-impl.d \
./src/mpi/model/granted-time-window-mpi-interface.d \
./src/mpi/model/mpi-interface.d \
./src/mpi/model/mpi-receiver.d \
./src/mpi/model/null-message-mpi-interface.d \
./src/mpi/model/null-message-simulator-impl.d \
./src/mpi/model/remote-channel-bundle-manager.d \
./src/mpi/model/remote-channel-bundle.d 

OBJS += \
./src/mpi/model/distributed-simulator-impl.o \
./src/mpi/model/granted-time-window-mpi-interface.o \
./src/mpi/model/mpi-interface.o \
./src/mpi/model/mpi-receiver.o \
./src/mpi/model/null-message-mpi-interface.o \
./src/mpi/model/null-message-simulator-impl.o \
./src/mpi/model/remote-channel-bundle-manager.o \
./src/mpi/model/remote-channel-bundle.o 


# Each subdirectory must supply rules for building sources it contributes
src/mpi/model/%.o: ../src/mpi/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


