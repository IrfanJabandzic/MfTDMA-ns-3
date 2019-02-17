################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/config-store/model/attribute-default-iterator.cc \
../src/config-store/model/attribute-iterator.cc \
../src/config-store/model/config-store.cc \
../src/config-store/model/display-functions.cc \
../src/config-store/model/file-config.cc \
../src/config-store/model/gtk-config-store.cc \
../src/config-store/model/model-node-creator.cc \
../src/config-store/model/model-typeid-creator.cc \
../src/config-store/model/raw-text-config.cc \
../src/config-store/model/xml-config.cc 

CC_DEPS += \
./src/config-store/model/attribute-default-iterator.d \
./src/config-store/model/attribute-iterator.d \
./src/config-store/model/config-store.d \
./src/config-store/model/display-functions.d \
./src/config-store/model/file-config.d \
./src/config-store/model/gtk-config-store.d \
./src/config-store/model/model-node-creator.d \
./src/config-store/model/model-typeid-creator.d \
./src/config-store/model/raw-text-config.d \
./src/config-store/model/xml-config.d 

OBJS += \
./src/config-store/model/attribute-default-iterator.o \
./src/config-store/model/attribute-iterator.o \
./src/config-store/model/config-store.o \
./src/config-store/model/display-functions.o \
./src/config-store/model/file-config.o \
./src/config-store/model/gtk-config-store.o \
./src/config-store/model/model-node-creator.o \
./src/config-store/model/model-typeid-creator.o \
./src/config-store/model/raw-text-config.o \
./src/config-store/model/xml-config.o 


# Each subdirectory must supply rules for building sources it contributes
src/config-store/model/%.o: ../src/config-store/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


