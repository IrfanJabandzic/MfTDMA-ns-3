################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/stats/examples/double-probe-example.cc \
../src/stats/examples/file-aggregator-example.cc \
../src/stats/examples/file-helper-example.cc \
../src/stats/examples/gnuplot-aggregator-example.cc \
../src/stats/examples/gnuplot-example.cc \
../src/stats/examples/gnuplot-helper-example.cc \
../src/stats/examples/time-probe-example.cc 

CC_DEPS += \
./src/stats/examples/double-probe-example.d \
./src/stats/examples/file-aggregator-example.d \
./src/stats/examples/file-helper-example.d \
./src/stats/examples/gnuplot-aggregator-example.d \
./src/stats/examples/gnuplot-example.d \
./src/stats/examples/gnuplot-helper-example.d \
./src/stats/examples/time-probe-example.d 

OBJS += \
./src/stats/examples/double-probe-example.o \
./src/stats/examples/file-aggregator-example.o \
./src/stats/examples/file-helper-example.o \
./src/stats/examples/gnuplot-aggregator-example.o \
./src/stats/examples/gnuplot-example.o \
./src/stats/examples/gnuplot-helper-example.o \
./src/stats/examples/time-probe-example.o 


# Each subdirectory must supply rules for building sources it contributes
src/stats/examples/%.o: ../src/stats/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


