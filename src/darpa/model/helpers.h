#ifndef HELPERS_H_
#define HELPERS_H_

#include "slot_manage_protocol.h"
#include <random>
#include <boost/circular_buffer.hpp>

namespace ns3 {

#define usleep(delay_us) 	uint64_t time_now=clock_get_time_ns(); \
							while(time_now+(delay_us)*1000>clock_get_time_ns()){}; \
							printf("SLEPT FOR %ld\n",clock_get_time_ns()-time_now);\

#define DEFAULT_TX_GAIN 20 //0 - 89 dB range
#define DEFAULT_RX_GAIN -1 //0 - 76 dB range

#define FDMA_DEFAULT_BW_INDEX Five
#define DEFAULT_MAX_BANDWIDTH 80000000ul // was 80MHz
#define DEFAULT_CHANNEL_STEP 5000000ul //5MHz channel step, fixed for now

#define RX_CHANNEL 0
#define TX_CHANNEL 0
#define CHANNEL_MIN 0

#define BROADCAST_MAC_ADDRESS 0xffffffffffff
#define MULTICAST_MAC_START 0x01005e000000
#define MULTICAST_MAC_END 0x01005e7fffff

#define RECEIVED_APP_PACKETS_SIZE 100
//#define RECEIVED_CONTROL_ACKS_SIZE 50
#define RECEIVED_CONTROL_ACKS_SIZE 10

#define TDMA_MAX_QUEUE_DELAY 55*1000000UL

#define TEST_OWN_MAC_ADDRESS 258102110846977UL
#define TEST_REMOTE_MAC_ADDRESS 258102110846978UL

#define TEST_MAC_ADDRESS1 258102110846977UL
#define TEST_MAC_ADDRESS2 258102110846978UL
#define TEST_MAC_ADDRESS3 258102110846979UL

#define BROADCAST_DEFAULT_TX_GAIN 29

#define DEFAULT_SLOT_COUNT SUPERFRAME_DURATION/TDMA_SLOT_DURATION

timestamp_t clock_get_time_ns();
timestamp_t clock_get_realtime_ns();

uint16_t get_random_value(uint16_t max_value);
bool calculate_probability(uint16_t count);

mac_address_t get_mac_address_from_array(uint8_t array[6]);
void get_array_from_mac_address(mac_address_t mac, uint8_t array[6]);

//CRC16 CCITT
unsigned short crc16(const unsigned char* data_p, unsigned short length);

//MCS-BW_Index lookup table
uint16_t get_bytes_in_slot(BW_INDEX bw_index, mcs_t MCS);
mcs_t get_minimum_mcs_to_fit_packet(BW_INDEX bw_index, uint16_t data_size);

//Printing packet data
void print_packet_data(const std::string* data);

/**
 * Destination-MCS table manager and access functions
 */
void initialize_dest_mcs_tupple(mac_address_t mac_addr_v, uint32_t default_gain, uint8_t min_mcs);
// Get the assigned MCS or RX channel for a specific destination
uint32_t get_mcs_based_on_destination(mac_address_t mac_addr_v, uint8_t min_mcs);
uint32_t get_channel_based_on_destination(mac_address_t mac_addr_v);
uint32_t get_txgain_based_on_destination(mac_address_t mac_addr_v, uint32_t default_gain);
// Add or update if existing the value of MCS and RX channel for a specific mac destination
void add_dest_mcs_tupple(mac_address_t mac_addr_v, uint32_t mcs_v, uint32_t default_gain);
void add_dest_channel_tupple(mac_address_t mac_addr_v, uint32_t channel, uint32_t default_gain, uint8_t min_mcs);
void add_dest_txgain_tupple(mac_address_t mac_addr_v, uint32_t tx_gain, uint8_t min_mcs);
//Remove an element destination-mcs
bool remove_dest_mcs_tupple(mac_address_t mac_addr_v);
void print_dest_mcs_table();
/*
 * END of Destination-MCS table manager and access functions
 */

//Mapping possible max active slots based on channel witdh to achieve more than 1Mbps
uint8_t get_limit_of_active_slots(uint32_t channel_width);

//Testing dummy function
void test_RX_channel_functions();
void test_MCS_functions();

/*bool received_app_packet_in_list(mac_address_t source_address, mac_index_t mac_packet_index);
bool control_ack_in_list(mac_address_t source_address, mac_index_t mac_packet_index);
void remove_control_ack_in_list(mac_address_t source_address, mac_index_t mac_packet_index);
bool check_received_control_ack(mac_address_t source_address, mac_index_t mac_packet_index);
void add_received_control_ack(mac_address_t source_address, mac_index_t mac_packet_index);*/

}

#endif /* HELPERS_H_ */
