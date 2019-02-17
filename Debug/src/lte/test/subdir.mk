################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/lte/test/epc-test-gtpu.cc \
../src/lte/test/epc-test-s1u-downlink.cc \
../src/lte/test/epc-test-s1u-uplink.cc \
../src/lte/test/lte-ffr-simple.cc \
../src/lte/test/lte-simple-helper.cc \
../src/lte/test/lte-simple-net-device.cc \
../src/lte/test/lte-simple-spectrum-phy.cc \
../src/lte/test/lte-test-aggregation-throughput-scale.cc \
../src/lte/test/lte-test-carrier-aggregation.cc \
../src/lte/test/lte-test-cell-selection.cc \
../src/lte/test/lte-test-cqa-ff-mac-scheduler.cc \
../src/lte/test/lte-test-cqi-generation.cc \
../src/lte/test/lte-test-deactivate-bearer.cc \
../src/lte/test/lte-test-downlink-power-control.cc \
../src/lte/test/lte-test-downlink-sinr.cc \
../src/lte/test/lte-test-earfcn.cc \
../src/lte/test/lte-test-entities.cc \
../src/lte/test/lte-test-fading.cc \
../src/lte/test/lte-test-fdbet-ff-mac-scheduler.cc \
../src/lte/test/lte-test-fdmt-ff-mac-scheduler.cc \
../src/lte/test/lte-test-fdtbfq-ff-mac-scheduler.cc \
../src/lte/test/lte-test-frequency-reuse.cc \
../src/lte/test/lte-test-harq.cc \
../src/lte/test/lte-test-interference-fr.cc \
../src/lte/test/lte-test-interference.cc \
../src/lte/test/lte-test-link-adaptation.cc \
../src/lte/test/lte-test-mimo.cc \
../src/lte/test/lte-test-pathloss-model.cc \
../src/lte/test/lte-test-pf-ff-mac-scheduler.cc \
../src/lte/test/lte-test-phy-error-model.cc \
../src/lte/test/lte-test-pss-ff-mac-scheduler.cc \
../src/lte/test/lte-test-rlc-am-e2e.cc \
../src/lte/test/lte-test-rlc-am-transmitter.cc \
../src/lte/test/lte-test-rlc-um-e2e.cc \
../src/lte/test/lte-test-rlc-um-transmitter.cc \
../src/lte/test/lte-test-rr-ff-mac-scheduler.cc \
../src/lte/test/lte-test-secondary-cell-selection.cc \
../src/lte/test/lte-test-spectrum-value-helper.cc \
../src/lte/test/lte-test-tdbet-ff-mac-scheduler.cc \
../src/lte/test/lte-test-tdmt-ff-mac-scheduler.cc \
../src/lte/test/lte-test-tdtbfq-ff-mac-scheduler.cc \
../src/lte/test/lte-test-tta-ff-mac-scheduler.cc \
../src/lte/test/lte-test-ue-measurements.cc \
../src/lte/test/lte-test-ue-phy.cc \
../src/lte/test/lte-test-uplink-power-control.cc \
../src/lte/test/lte-test-uplink-sinr.cc \
../src/lte/test/test-asn1-encoding.cc \
../src/lte/test/test-epc-tft-classifier.cc \
../src/lte/test/test-lte-antenna.cc \
../src/lte/test/test-lte-epc-e2e-data.cc \
../src/lte/test/test-lte-handover-delay.cc \
../src/lte/test/test-lte-handover-target.cc \
../src/lte/test/test-lte-rlc-header.cc \
../src/lte/test/test-lte-rrc.cc \
../src/lte/test/test-lte-x2-handover-measures.cc \
../src/lte/test/test-lte-x2-handover.cc 

CC_DEPS += \
./src/lte/test/epc-test-gtpu.d \
./src/lte/test/epc-test-s1u-downlink.d \
./src/lte/test/epc-test-s1u-uplink.d \
./src/lte/test/lte-ffr-simple.d \
./src/lte/test/lte-simple-helper.d \
./src/lte/test/lte-simple-net-device.d \
./src/lte/test/lte-simple-spectrum-phy.d \
./src/lte/test/lte-test-aggregation-throughput-scale.d \
./src/lte/test/lte-test-carrier-aggregation.d \
./src/lte/test/lte-test-cell-selection.d \
./src/lte/test/lte-test-cqa-ff-mac-scheduler.d \
./src/lte/test/lte-test-cqi-generation.d \
./src/lte/test/lte-test-deactivate-bearer.d \
./src/lte/test/lte-test-downlink-power-control.d \
./src/lte/test/lte-test-downlink-sinr.d \
./src/lte/test/lte-test-earfcn.d \
./src/lte/test/lte-test-entities.d \
./src/lte/test/lte-test-fading.d \
./src/lte/test/lte-test-fdbet-ff-mac-scheduler.d \
./src/lte/test/lte-test-fdmt-ff-mac-scheduler.d \
./src/lte/test/lte-test-fdtbfq-ff-mac-scheduler.d \
./src/lte/test/lte-test-frequency-reuse.d \
./src/lte/test/lte-test-harq.d \
./src/lte/test/lte-test-interference-fr.d \
./src/lte/test/lte-test-interference.d \
./src/lte/test/lte-test-link-adaptation.d \
./src/lte/test/lte-test-mimo.d \
./src/lte/test/lte-test-pathloss-model.d \
./src/lte/test/lte-test-pf-ff-mac-scheduler.d \
./src/lte/test/lte-test-phy-error-model.d \
./src/lte/test/lte-test-pss-ff-mac-scheduler.d \
./src/lte/test/lte-test-rlc-am-e2e.d \
./src/lte/test/lte-test-rlc-am-transmitter.d \
./src/lte/test/lte-test-rlc-um-e2e.d \
./src/lte/test/lte-test-rlc-um-transmitter.d \
./src/lte/test/lte-test-rr-ff-mac-scheduler.d \
./src/lte/test/lte-test-secondary-cell-selection.d \
./src/lte/test/lte-test-spectrum-value-helper.d \
./src/lte/test/lte-test-tdbet-ff-mac-scheduler.d \
./src/lte/test/lte-test-tdmt-ff-mac-scheduler.d \
./src/lte/test/lte-test-tdtbfq-ff-mac-scheduler.d \
./src/lte/test/lte-test-tta-ff-mac-scheduler.d \
./src/lte/test/lte-test-ue-measurements.d \
./src/lte/test/lte-test-ue-phy.d \
./src/lte/test/lte-test-uplink-power-control.d \
./src/lte/test/lte-test-uplink-sinr.d \
./src/lte/test/test-asn1-encoding.d \
./src/lte/test/test-epc-tft-classifier.d \
./src/lte/test/test-lte-antenna.d \
./src/lte/test/test-lte-epc-e2e-data.d \
./src/lte/test/test-lte-handover-delay.d \
./src/lte/test/test-lte-handover-target.d \
./src/lte/test/test-lte-rlc-header.d \
./src/lte/test/test-lte-rrc.d \
./src/lte/test/test-lte-x2-handover-measures.d \
./src/lte/test/test-lte-x2-handover.d 

OBJS += \
./src/lte/test/epc-test-gtpu.o \
./src/lte/test/epc-test-s1u-downlink.o \
./src/lte/test/epc-test-s1u-uplink.o \
./src/lte/test/lte-ffr-simple.o \
./src/lte/test/lte-simple-helper.o \
./src/lte/test/lte-simple-net-device.o \
./src/lte/test/lte-simple-spectrum-phy.o \
./src/lte/test/lte-test-aggregation-throughput-scale.o \
./src/lte/test/lte-test-carrier-aggregation.o \
./src/lte/test/lte-test-cell-selection.o \
./src/lte/test/lte-test-cqa-ff-mac-scheduler.o \
./src/lte/test/lte-test-cqi-generation.o \
./src/lte/test/lte-test-deactivate-bearer.o \
./src/lte/test/lte-test-downlink-power-control.o \
./src/lte/test/lte-test-downlink-sinr.o \
./src/lte/test/lte-test-earfcn.o \
./src/lte/test/lte-test-entities.o \
./src/lte/test/lte-test-fading.o \
./src/lte/test/lte-test-fdbet-ff-mac-scheduler.o \
./src/lte/test/lte-test-fdmt-ff-mac-scheduler.o \
./src/lte/test/lte-test-fdtbfq-ff-mac-scheduler.o \
./src/lte/test/lte-test-frequency-reuse.o \
./src/lte/test/lte-test-harq.o \
./src/lte/test/lte-test-interference-fr.o \
./src/lte/test/lte-test-interference.o \
./src/lte/test/lte-test-link-adaptation.o \
./src/lte/test/lte-test-mimo.o \
./src/lte/test/lte-test-pathloss-model.o \
./src/lte/test/lte-test-pf-ff-mac-scheduler.o \
./src/lte/test/lte-test-phy-error-model.o \
./src/lte/test/lte-test-pss-ff-mac-scheduler.o \
./src/lte/test/lte-test-rlc-am-e2e.o \
./src/lte/test/lte-test-rlc-am-transmitter.o \
./src/lte/test/lte-test-rlc-um-e2e.o \
./src/lte/test/lte-test-rlc-um-transmitter.o \
./src/lte/test/lte-test-rr-ff-mac-scheduler.o \
./src/lte/test/lte-test-secondary-cell-selection.o \
./src/lte/test/lte-test-spectrum-value-helper.o \
./src/lte/test/lte-test-tdbet-ff-mac-scheduler.o \
./src/lte/test/lte-test-tdmt-ff-mac-scheduler.o \
./src/lte/test/lte-test-tdtbfq-ff-mac-scheduler.o \
./src/lte/test/lte-test-tta-ff-mac-scheduler.o \
./src/lte/test/lte-test-ue-measurements.o \
./src/lte/test/lte-test-ue-phy.o \
./src/lte/test/lte-test-uplink-power-control.o \
./src/lte/test/lte-test-uplink-sinr.o \
./src/lte/test/test-asn1-encoding.o \
./src/lte/test/test-epc-tft-classifier.o \
./src/lte/test/test-lte-antenna.o \
./src/lte/test/test-lte-epc-e2e-data.o \
./src/lte/test/test-lte-handover-delay.o \
./src/lte/test/test-lte-handover-target.o \
./src/lte/test/test-lte-rlc-header.o \
./src/lte/test/test-lte-rrc.o \
./src/lte/test/test-lte-x2-handover-measures.o \
./src/lte/test/test-lte-x2-handover.o 


# Each subdirectory must supply rules for building sources it contributes
src/lte/test/%.o: ../src/lte/test/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/include/libxml2 -I/home/irfan/workspace/bake/source/ns-3.27/build -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


