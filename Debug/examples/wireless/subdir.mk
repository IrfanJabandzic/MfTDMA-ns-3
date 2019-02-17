################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../examples/wireless/80211e-txop.cc \
../examples/wireless/80211n-mimo.cc \
../examples/wireless/he-wifi-network.cc \
../examples/wireless/ht-wifi-network.cc \
../examples/wireless/mixed-network.cc \
../examples/wireless/mixed-wired-wireless.cc \
../examples/wireless/multirate.cc \
../examples/wireless/ofdm-he-validation.cc \
../examples/wireless/ofdm-ht-validation.cc \
../examples/wireless/ofdm-validation.cc \
../examples/wireless/ofdm-vht-validation.cc \
../examples/wireless/power-adaptation-distance.cc \
../examples/wireless/power-adaptation-interference.cc \
../examples/wireless/rate-adaptation-distance.cc \
../examples/wireless/simple-ht-hidden-stations.cc \
../examples/wireless/vht-wifi-network.cc \
../examples/wireless/wifi-adhoc.cc \
../examples/wireless/wifi-aggregation.cc \
../examples/wireless/wifi-ap.cc \
../examples/wireless/wifi-backward-compatibility.cc \
../examples/wireless/wifi-blockack.cc \
../examples/wireless/wifi-clear-channel-cmu.cc \
../examples/wireless/wifi-hidden-terminal.cc \
../examples/wireless/wifi-multi-tos.cc \
../examples/wireless/wifi-simple-adhoc-grid.cc \
../examples/wireless/wifi-simple-adhoc.cc \
../examples/wireless/wifi-simple-infra.cc \
../examples/wireless/wifi-simple-interference.cc \
../examples/wireless/wifi-sleep.cc \
../examples/wireless/wifi-spectrum-per-example.cc \
../examples/wireless/wifi-spectrum-per-interference.cc \
../examples/wireless/wifi-spectrum-saturation-example.cc \
../examples/wireless/wifi-tcp.cc \
../examples/wireless/wifi-timing-attributes.cc \
../examples/wireless/wifi-wired-bridging.cc 

CC_DEPS += \
./examples/wireless/80211e-txop.d \
./examples/wireless/80211n-mimo.d \
./examples/wireless/he-wifi-network.d \
./examples/wireless/ht-wifi-network.d \
./examples/wireless/mixed-network.d \
./examples/wireless/mixed-wired-wireless.d \
./examples/wireless/multirate.d \
./examples/wireless/ofdm-he-validation.d \
./examples/wireless/ofdm-ht-validation.d \
./examples/wireless/ofdm-validation.d \
./examples/wireless/ofdm-vht-validation.d \
./examples/wireless/power-adaptation-distance.d \
./examples/wireless/power-adaptation-interference.d \
./examples/wireless/rate-adaptation-distance.d \
./examples/wireless/simple-ht-hidden-stations.d \
./examples/wireless/vht-wifi-network.d \
./examples/wireless/wifi-adhoc.d \
./examples/wireless/wifi-aggregation.d \
./examples/wireless/wifi-ap.d \
./examples/wireless/wifi-backward-compatibility.d \
./examples/wireless/wifi-blockack.d \
./examples/wireless/wifi-clear-channel-cmu.d \
./examples/wireless/wifi-hidden-terminal.d \
./examples/wireless/wifi-multi-tos.d \
./examples/wireless/wifi-simple-adhoc-grid.d \
./examples/wireless/wifi-simple-adhoc.d \
./examples/wireless/wifi-simple-infra.d \
./examples/wireless/wifi-simple-interference.d \
./examples/wireless/wifi-sleep.d \
./examples/wireless/wifi-spectrum-per-example.d \
./examples/wireless/wifi-spectrum-per-interference.d \
./examples/wireless/wifi-spectrum-saturation-example.d \
./examples/wireless/wifi-tcp.d \
./examples/wireless/wifi-timing-attributes.d \
./examples/wireless/wifi-wired-bridging.d 

OBJS += \
./examples/wireless/80211e-txop.o \
./examples/wireless/80211n-mimo.o \
./examples/wireless/he-wifi-network.o \
./examples/wireless/ht-wifi-network.o \
./examples/wireless/mixed-network.o \
./examples/wireless/mixed-wired-wireless.o \
./examples/wireless/multirate.o \
./examples/wireless/ofdm-he-validation.o \
./examples/wireless/ofdm-ht-validation.o \
./examples/wireless/ofdm-validation.o \
./examples/wireless/ofdm-vht-validation.o \
./examples/wireless/power-adaptation-distance.o \
./examples/wireless/power-adaptation-interference.o \
./examples/wireless/rate-adaptation-distance.o \
./examples/wireless/simple-ht-hidden-stations.o \
./examples/wireless/vht-wifi-network.o \
./examples/wireless/wifi-adhoc.o \
./examples/wireless/wifi-aggregation.o \
./examples/wireless/wifi-ap.o \
./examples/wireless/wifi-backward-compatibility.o \
./examples/wireless/wifi-blockack.o \
./examples/wireless/wifi-clear-channel-cmu.o \
./examples/wireless/wifi-hidden-terminal.o \
./examples/wireless/wifi-multi-tos.o \
./examples/wireless/wifi-simple-adhoc-grid.o \
./examples/wireless/wifi-simple-adhoc.o \
./examples/wireless/wifi-simple-infra.o \
./examples/wireless/wifi-simple-interference.o \
./examples/wireless/wifi-sleep.o \
./examples/wireless/wifi-spectrum-per-example.o \
./examples/wireless/wifi-spectrum-per-interference.o \
./examples/wireless/wifi-spectrum-saturation-example.o \
./examples/wireless/wifi-tcp.o \
./examples/wireless/wifi-timing-attributes.o \
./examples/wireless/wifi-wired-bridging.o 


# Each subdirectory must supply rules for building sources it contributes
examples/wireless/%.o: ../examples/wireless/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


