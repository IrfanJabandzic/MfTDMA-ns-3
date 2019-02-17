################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/stats/model/boolean-probe.cc \
../src/stats/model/data-calculator.cc \
../src/stats/model/data-collection-object.cc \
../src/stats/model/data-collector.cc \
../src/stats/model/data-output-interface.cc \
../src/stats/model/double-probe.cc \
../src/stats/model/file-aggregator.cc \
../src/stats/model/get-wildcard-matches.cc \
../src/stats/model/gnuplot-aggregator.cc \
../src/stats/model/gnuplot.cc \
../src/stats/model/omnet-data-output.cc \
../src/stats/model/probe.cc \
../src/stats/model/sqlite-data-output.cc \
../src/stats/model/time-data-calculators.cc \
../src/stats/model/time-probe.cc \
../src/stats/model/time-series-adaptor.cc \
../src/stats/model/uinteger-16-probe.cc \
../src/stats/model/uinteger-32-probe.cc \
../src/stats/model/uinteger-8-probe.cc 

CC_DEPS += \
./src/stats/model/boolean-probe.d \
./src/stats/model/data-calculator.d \
./src/stats/model/data-collection-object.d \
./src/stats/model/data-collector.d \
./src/stats/model/data-output-interface.d \
./src/stats/model/double-probe.d \
./src/stats/model/file-aggregator.d \
./src/stats/model/get-wildcard-matches.d \
./src/stats/model/gnuplot-aggregator.d \
./src/stats/model/gnuplot.d \
./src/stats/model/omnet-data-output.d \
./src/stats/model/probe.d \
./src/stats/model/sqlite-data-output.d \
./src/stats/model/time-data-calculators.d \
./src/stats/model/time-probe.d \
./src/stats/model/time-series-adaptor.d \
./src/stats/model/uinteger-16-probe.d \
./src/stats/model/uinteger-32-probe.d \
./src/stats/model/uinteger-8-probe.d 

OBJS += \
./src/stats/model/boolean-probe.o \
./src/stats/model/data-calculator.o \
./src/stats/model/data-collection-object.o \
./src/stats/model/data-collector.o \
./src/stats/model/data-output-interface.o \
./src/stats/model/double-probe.o \
./src/stats/model/file-aggregator.o \
./src/stats/model/get-wildcard-matches.o \
./src/stats/model/gnuplot-aggregator.o \
./src/stats/model/gnuplot.o \
./src/stats/model/omnet-data-output.o \
./src/stats/model/probe.o \
./src/stats/model/sqlite-data-output.o \
./src/stats/model/time-data-calculators.o \
./src/stats/model/time-probe.o \
./src/stats/model/time-series-adaptor.o \
./src/stats/model/uinteger-16-probe.o \
./src/stats/model/uinteger-32-probe.o \
./src/stats/model/uinteger-8-probe.o 


# Each subdirectory must supply rules for building sources it contributes
src/stats/model/%.o: ../src/stats/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


