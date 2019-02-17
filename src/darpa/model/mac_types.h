// This header file is used to define all the types of variables used within the MAC LAYER

#ifndef MAC_TYPES_H
#define MAC_TYPES_H

#include <cstdint>

typedef uint64_t byte_counter_t;
typedef uint8_t channel_t;
typedef uint8_t slot_number_t;
typedef uint8_t cqi_t;
typedef uint8_t fragment_index_t;
typedef uint8_t fragment_index_max_t;
typedef uint32_t gain_t;
typedef uint64_t mac_address_t;
typedef uint16_t mac_address_app_t;
typedef uint16_t mac_index_t;
typedef uint8_t mcs_t;
typedef uint64_t packet_counter_t;
typedef int8_t rssi_t;
typedef uint64_t timestamp_t;
typedef uint16_t udp_port_t;

enum type_of_message {

	APP = 0,
	APP_ARP = 1,
	AI_MESS = 2,
	//MAC packet types
	MAC_CONTROL_ACK = 19,
	MAC_ACK = 20,
	MAC_FDMA_CHANNEL_BROADCAST = 21,
	MAC_TDMA_ADD_SLOT=22,
	MAC_TDMA_DELETE_SLOT=23,
	MAC_TDMA_MOVE_SLOT=24,
	MAC_TDMA_SLOT_TRANSACTION_RESULT=25,
	MAC_TDMA_SLOT_TRANSACTION_ACK=26,
	MAC_TDMA_ADVERTISE_SLOT_TABLE=27
};

enum BW_INDEX {
		BW_INDEX_UNKNOWN		= 0,
		OneDotFour	= 1,	// 1.4 MHz
		Three	   		= 2,	// 3 MHz
		Five	   		= 3,	// 5 MHz
		Ten		   		= 4,	// 10 MHz
		Fifteen			= 5,	// 15 MHz
    Twenty			= 6	// 20 MHz
	};

struct __attribute__ ((__packed__)) mac_header{

	mac_address_t destination_mac_address;
	mac_address_t source_mac_address;
	mac_index_t mac_index;
	uint16_t num_pad_bytes;
	uint16_t mac_crc;
	bool need_ack;
	enum type_of_message mes_type;
	fragment_index_t fragment_index;
	fragment_index_max_t fragment_index_max;
};

struct __attribute__ ((__packed__)) mac_header_app{

	mac_address_app_t destination_mac_address;
	mac_address_app_t source_mac_address;
	mac_index_t mac_index;
	uint16_t num_pad_bytes;
	uint16_t mac_crc;
	bool need_ack;
	enum type_of_message mes_type;
	fragment_index_t fragment_index;
	fragment_index_max_t fragment_index_max;
};
#endif /* MAC_TYPES_H */
