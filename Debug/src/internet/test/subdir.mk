################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/internet/test/global-route-manager-impl-test-suite.cc \
../src/internet/test/ipv4-address-generator-test-suite.cc \
../src/internet/test/ipv4-address-helper-test-suite.cc \
../src/internet/test/ipv4-forwarding-test.cc \
../src/internet/test/ipv4-fragmentation-test.cc \
../src/internet/test/ipv4-global-routing-test-suite.cc \
../src/internet/test/ipv4-header-test.cc \
../src/internet/test/ipv4-list-routing-test-suite.cc \
../src/internet/test/ipv4-packet-info-tag-test-suite.cc \
../src/internet/test/ipv4-raw-test.cc \
../src/internet/test/ipv4-rip-test.cc \
../src/internet/test/ipv4-static-routing-test-suite.cc \
../src/internet/test/ipv4-test.cc \
../src/internet/test/ipv6-address-generator-test-suite.cc \
../src/internet/test/ipv6-address-helper-test-suite.cc \
../src/internet/test/ipv6-dual-stack-test-suite.cc \
../src/internet/test/ipv6-extension-header-test-suite.cc \
../src/internet/test/ipv6-forwarding-test.cc \
../src/internet/test/ipv6-fragmentation-test.cc \
../src/internet/test/ipv6-list-routing-test-suite.cc \
../src/internet/test/ipv6-packet-info-tag-test-suite.cc \
../src/internet/test/ipv6-raw-test.cc \
../src/internet/test/ipv6-ripng-test.cc \
../src/internet/test/ipv6-test.cc \
../src/internet/test/rtt-test.cc \
../src/internet/test/tcp-advertised-window-test.cc \
../src/internet/test/tcp-bic-test.cc \
../src/internet/test/tcp-bytes-in-flight-test.cc \
../src/internet/test/tcp-cong-avoid-test.cc \
../src/internet/test/tcp-datasentcb-test.cc \
../src/internet/test/tcp-endpoint-bug2211.cc \
../src/internet/test/tcp-error-model.cc \
../src/internet/test/tcp-fast-retr-test.cc \
../src/internet/test/tcp-general-test.cc \
../src/internet/test/tcp-header-test.cc \
../src/internet/test/tcp-highspeed-test.cc \
../src/internet/test/tcp-htcp-test.cc \
../src/internet/test/tcp-hybla-test.cc \
../src/internet/test/tcp-illinois-test.cc \
../src/internet/test/tcp-ledbat-test.cc \
../src/internet/test/tcp-option-test.cc \
../src/internet/test/tcp-pkts-acked-test.cc \
../src/internet/test/tcp-rto-test.cc \
../src/internet/test/tcp-rtt-estimation.cc \
../src/internet/test/tcp-rx-buffer-test.cc \
../src/internet/test/tcp-sack-permitted-test.cc \
../src/internet/test/tcp-scalable-test.cc \
../src/internet/test/tcp-slow-start-test.cc \
../src/internet/test/tcp-test.cc \
../src/internet/test/tcp-timestamp-test.cc \
../src/internet/test/tcp-tx-buffer-test.cc \
../src/internet/test/tcp-vegas-test.cc \
../src/internet/test/tcp-veno-test.cc \
../src/internet/test/tcp-wscaling-test.cc \
../src/internet/test/tcp-yeah-test.cc \
../src/internet/test/tcp-zero-window-test.cc \
../src/internet/test/udp-test.cc 

CC_DEPS += \
./src/internet/test/global-route-manager-impl-test-suite.d \
./src/internet/test/ipv4-address-generator-test-suite.d \
./src/internet/test/ipv4-address-helper-test-suite.d \
./src/internet/test/ipv4-forwarding-test.d \
./src/internet/test/ipv4-fragmentation-test.d \
./src/internet/test/ipv4-global-routing-test-suite.d \
./src/internet/test/ipv4-header-test.d \
./src/internet/test/ipv4-list-routing-test-suite.d \
./src/internet/test/ipv4-packet-info-tag-test-suite.d \
./src/internet/test/ipv4-raw-test.d \
./src/internet/test/ipv4-rip-test.d \
./src/internet/test/ipv4-static-routing-test-suite.d \
./src/internet/test/ipv4-test.d \
./src/internet/test/ipv6-address-generator-test-suite.d \
./src/internet/test/ipv6-address-helper-test-suite.d \
./src/internet/test/ipv6-dual-stack-test-suite.d \
./src/internet/test/ipv6-extension-header-test-suite.d \
./src/internet/test/ipv6-forwarding-test.d \
./src/internet/test/ipv6-fragmentation-test.d \
./src/internet/test/ipv6-list-routing-test-suite.d \
./src/internet/test/ipv6-packet-info-tag-test-suite.d \
./src/internet/test/ipv6-raw-test.d \
./src/internet/test/ipv6-ripng-test.d \
./src/internet/test/ipv6-test.d \
./src/internet/test/rtt-test.d \
./src/internet/test/tcp-advertised-window-test.d \
./src/internet/test/tcp-bic-test.d \
./src/internet/test/tcp-bytes-in-flight-test.d \
./src/internet/test/tcp-cong-avoid-test.d \
./src/internet/test/tcp-datasentcb-test.d \
./src/internet/test/tcp-endpoint-bug2211.d \
./src/internet/test/tcp-error-model.d \
./src/internet/test/tcp-fast-retr-test.d \
./src/internet/test/tcp-general-test.d \
./src/internet/test/tcp-header-test.d \
./src/internet/test/tcp-highspeed-test.d \
./src/internet/test/tcp-htcp-test.d \
./src/internet/test/tcp-hybla-test.d \
./src/internet/test/tcp-illinois-test.d \
./src/internet/test/tcp-ledbat-test.d \
./src/internet/test/tcp-option-test.d \
./src/internet/test/tcp-pkts-acked-test.d \
./src/internet/test/tcp-rto-test.d \
./src/internet/test/tcp-rtt-estimation.d \
./src/internet/test/tcp-rx-buffer-test.d \
./src/internet/test/tcp-sack-permitted-test.d \
./src/internet/test/tcp-scalable-test.d \
./src/internet/test/tcp-slow-start-test.d \
./src/internet/test/tcp-test.d \
./src/internet/test/tcp-timestamp-test.d \
./src/internet/test/tcp-tx-buffer-test.d \
./src/internet/test/tcp-vegas-test.d \
./src/internet/test/tcp-veno-test.d \
./src/internet/test/tcp-wscaling-test.d \
./src/internet/test/tcp-yeah-test.d \
./src/internet/test/tcp-zero-window-test.d \
./src/internet/test/udp-test.d 

OBJS += \
./src/internet/test/global-route-manager-impl-test-suite.o \
./src/internet/test/ipv4-address-generator-test-suite.o \
./src/internet/test/ipv4-address-helper-test-suite.o \
./src/internet/test/ipv4-forwarding-test.o \
./src/internet/test/ipv4-fragmentation-test.o \
./src/internet/test/ipv4-global-routing-test-suite.o \
./src/internet/test/ipv4-header-test.o \
./src/internet/test/ipv4-list-routing-test-suite.o \
./src/internet/test/ipv4-packet-info-tag-test-suite.o \
./src/internet/test/ipv4-raw-test.o \
./src/internet/test/ipv4-rip-test.o \
./src/internet/test/ipv4-static-routing-test-suite.o \
./src/internet/test/ipv4-test.o \
./src/internet/test/ipv6-address-generator-test-suite.o \
./src/internet/test/ipv6-address-helper-test-suite.o \
./src/internet/test/ipv6-dual-stack-test-suite.o \
./src/internet/test/ipv6-extension-header-test-suite.o \
./src/internet/test/ipv6-forwarding-test.o \
./src/internet/test/ipv6-fragmentation-test.o \
./src/internet/test/ipv6-list-routing-test-suite.o \
./src/internet/test/ipv6-packet-info-tag-test-suite.o \
./src/internet/test/ipv6-raw-test.o \
./src/internet/test/ipv6-ripng-test.o \
./src/internet/test/ipv6-test.o \
./src/internet/test/rtt-test.o \
./src/internet/test/tcp-advertised-window-test.o \
./src/internet/test/tcp-bic-test.o \
./src/internet/test/tcp-bytes-in-flight-test.o \
./src/internet/test/tcp-cong-avoid-test.o \
./src/internet/test/tcp-datasentcb-test.o \
./src/internet/test/tcp-endpoint-bug2211.o \
./src/internet/test/tcp-error-model.o \
./src/internet/test/tcp-fast-retr-test.o \
./src/internet/test/tcp-general-test.o \
./src/internet/test/tcp-header-test.o \
./src/internet/test/tcp-highspeed-test.o \
./src/internet/test/tcp-htcp-test.o \
./src/internet/test/tcp-hybla-test.o \
./src/internet/test/tcp-illinois-test.o \
./src/internet/test/tcp-ledbat-test.o \
./src/internet/test/tcp-option-test.o \
./src/internet/test/tcp-pkts-acked-test.o \
./src/internet/test/tcp-rto-test.o \
./src/internet/test/tcp-rtt-estimation.o \
./src/internet/test/tcp-rx-buffer-test.o \
./src/internet/test/tcp-sack-permitted-test.o \
./src/internet/test/tcp-scalable-test.o \
./src/internet/test/tcp-slow-start-test.o \
./src/internet/test/tcp-test.o \
./src/internet/test/tcp-timestamp-test.o \
./src/internet/test/tcp-tx-buffer-test.o \
./src/internet/test/tcp-vegas-test.o \
./src/internet/test/tcp-veno-test.o \
./src/internet/test/tcp-wscaling-test.o \
./src/internet/test/tcp-yeah-test.o \
./src/internet/test/tcp-zero-window-test.o \
./src/internet/test/udp-test.o 


# Each subdirectory must supply rules for building sources it contributes
src/internet/test/%.o: ../src/internet/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


