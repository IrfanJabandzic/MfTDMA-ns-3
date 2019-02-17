################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/core/model/attribute-construction-list.cc \
../src/core/model/attribute.cc \
../src/core/model/boolean.cc \
../src/core/model/breakpoint.cc \
../src/core/model/calendar-scheduler.cc \
../src/core/model/callback.cc \
../src/core/model/command-line.cc \
../src/core/model/config.cc \
../src/core/model/default-simulator-impl.cc \
../src/core/model/des-metrics.cc \
../src/core/model/double.cc \
../src/core/model/enum.cc \
../src/core/model/event-id.cc \
../src/core/model/event-impl.cc \
../src/core/model/fatal-impl.cc \
../src/core/model/global-value.cc \
../src/core/model/hash-fnv.cc \
../src/core/model/hash-function.cc \
../src/core/model/hash-murmur3.cc \
../src/core/model/hash.cc \
../src/core/model/heap-scheduler.cc \
../src/core/model/int64x64-128.cc \
../src/core/model/int64x64-cairo.cc \
../src/core/model/int64x64.cc \
../src/core/model/integer.cc \
../src/core/model/list-scheduler.cc \
../src/core/model/log.cc \
../src/core/model/make-event.cc \
../src/core/model/map-scheduler.cc \
../src/core/model/names.cc \
../src/core/model/object-base.cc \
../src/core/model/object-factory.cc \
../src/core/model/object-ptr-container.cc \
../src/core/model/object.cc \
../src/core/model/pointer.cc \
../src/core/model/random-variable-stream.cc \
../src/core/model/realtime-simulator-impl.cc \
../src/core/model/ref-count-base.cc \
../src/core/model/rng-seed-manager.cc \
../src/core/model/rng-stream.cc \
../src/core/model/scheduler.cc \
../src/core/model/simulator-impl.cc \
../src/core/model/simulator.cc \
../src/core/model/string.cc \
../src/core/model/synchronizer.cc \
../src/core/model/system-path.cc \
../src/core/model/system-thread.cc \
../src/core/model/test.cc \
../src/core/model/time.cc \
../src/core/model/timer.cc \
../src/core/model/trace-source-accessor.cc \
../src/core/model/type-id.cc \
../src/core/model/type-name.cc \
../src/core/model/uinteger.cc \
../src/core/model/unix-fd-reader.cc \
../src/core/model/unix-system-condition.cc \
../src/core/model/unix-system-mutex.cc \
../src/core/model/unix-system-wall-clock-ms.cc \
../src/core/model/vector.cc \
../src/core/model/wall-clock-synchronizer.cc \
../src/core/model/watchdog.cc \
../src/core/model/win32-system-wall-clock-ms.cc 

C_SRCS += \
../src/core/model/cairo-wideint.c 

CC_DEPS += \
./src/core/model/attribute-construction-list.d \
./src/core/model/attribute.d \
./src/core/model/boolean.d \
./src/core/model/breakpoint.d \
./src/core/model/calendar-scheduler.d \
./src/core/model/callback.d \
./src/core/model/command-line.d \
./src/core/model/config.d \
./src/core/model/default-simulator-impl.d \
./src/core/model/des-metrics.d \
./src/core/model/double.d \
./src/core/model/enum.d \
./src/core/model/event-id.d \
./src/core/model/event-impl.d \
./src/core/model/fatal-impl.d \
./src/core/model/global-value.d \
./src/core/model/hash-fnv.d \
./src/core/model/hash-function.d \
./src/core/model/hash-murmur3.d \
./src/core/model/hash.d \
./src/core/model/heap-scheduler.d \
./src/core/model/int64x64-128.d \
./src/core/model/int64x64-cairo.d \
./src/core/model/int64x64.d \
./src/core/model/integer.d \
./src/core/model/list-scheduler.d \
./src/core/model/log.d \
./src/core/model/make-event.d \
./src/core/model/map-scheduler.d \
./src/core/model/names.d \
./src/core/model/object-base.d \
./src/core/model/object-factory.d \
./src/core/model/object-ptr-container.d \
./src/core/model/object.d \
./src/core/model/pointer.d \
./src/core/model/random-variable-stream.d \
./src/core/model/realtime-simulator-impl.d \
./src/core/model/ref-count-base.d \
./src/core/model/rng-seed-manager.d \
./src/core/model/rng-stream.d \
./src/core/model/scheduler.d \
./src/core/model/simulator-impl.d \
./src/core/model/simulator.d \
./src/core/model/string.d \
./src/core/model/synchronizer.d \
./src/core/model/system-path.d \
./src/core/model/system-thread.d \
./src/core/model/test.d \
./src/core/model/time.d \
./src/core/model/timer.d \
./src/core/model/trace-source-accessor.d \
./src/core/model/type-id.d \
./src/core/model/type-name.d \
./src/core/model/uinteger.d \
./src/core/model/unix-fd-reader.d \
./src/core/model/unix-system-condition.d \
./src/core/model/unix-system-mutex.d \
./src/core/model/unix-system-wall-clock-ms.d \
./src/core/model/vector.d \
./src/core/model/wall-clock-synchronizer.d \
./src/core/model/watchdog.d \
./src/core/model/win32-system-wall-clock-ms.d 

OBJS += \
./src/core/model/attribute-construction-list.o \
./src/core/model/attribute.o \
./src/core/model/boolean.o \
./src/core/model/breakpoint.o \
./src/core/model/cairo-wideint.o \
./src/core/model/calendar-scheduler.o \
./src/core/model/callback.o \
./src/core/model/command-line.o \
./src/core/model/config.o \
./src/core/model/default-simulator-impl.o \
./src/core/model/des-metrics.o \
./src/core/model/double.o \
./src/core/model/enum.o \
./src/core/model/event-id.o \
./src/core/model/event-impl.o \
./src/core/model/fatal-impl.o \
./src/core/model/global-value.o \
./src/core/model/hash-fnv.o \
./src/core/model/hash-function.o \
./src/core/model/hash-murmur3.o \
./src/core/model/hash.o \
./src/core/model/heap-scheduler.o \
./src/core/model/int64x64-128.o \
./src/core/model/int64x64-cairo.o \
./src/core/model/int64x64.o \
./src/core/model/integer.o \
./src/core/model/list-scheduler.o \
./src/core/model/log.o \
./src/core/model/make-event.o \
./src/core/model/map-scheduler.o \
./src/core/model/names.o \
./src/core/model/object-base.o \
./src/core/model/object-factory.o \
./src/core/model/object-ptr-container.o \
./src/core/model/object.o \
./src/core/model/pointer.o \
./src/core/model/random-variable-stream.o \
./src/core/model/realtime-simulator-impl.o \
./src/core/model/ref-count-base.o \
./src/core/model/rng-seed-manager.o \
./src/core/model/rng-stream.o \
./src/core/model/scheduler.o \
./src/core/model/simulator-impl.o \
./src/core/model/simulator.o \
./src/core/model/string.o \
./src/core/model/synchronizer.o \
./src/core/model/system-path.o \
./src/core/model/system-thread.o \
./src/core/model/test.o \
./src/core/model/time.o \
./src/core/model/timer.o \
./src/core/model/trace-source-accessor.o \
./src/core/model/type-id.o \
./src/core/model/type-name.o \
./src/core/model/uinteger.o \
./src/core/model/unix-fd-reader.o \
./src/core/model/unix-system-condition.o \
./src/core/model/unix-system-mutex.o \
./src/core/model/unix-system-wall-clock-ms.o \
./src/core/model/vector.o \
./src/core/model/wall-clock-synchronizer.o \
./src/core/model/watchdog.o \
./src/core/model/win32-system-wall-clock-ms.o 

C_DEPS += \
./src/core/model/cairo-wideint.d 


# Each subdirectory must supply rules for building sources it contributes
src/core/model/%.o: ../src/core/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/core/model/%.o: ../src/core/model/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


