/*
 * slot_manage_protocol.h
 *
 *  Created on: Oct 24, 2017
 *      Author: spilios
 */

#ifndef MAC_SLOT_MANAGE_PROTOCOL_H_
#define MAC_SLOT_MANAGE_PROTOCOL_H_

#include "mac_types.h"
#include <vector>

#define SLOT_ALLOCATION_UPGRADE

//#define SLOT_ALLOCATION_EXPOSED_NODE

#define MAX_SUGGESTED_SLOTS 15 //Maximum suggestion of slots supported
#define MAX_NUMBER_PROPOSED_SLOTS 10 //Todo CHECK with Maxim if he agrees to put that to 6-8
#define MAX_USABLE_SLOTS 38 //18 //TODO make this adjustable based on SF slot count

#ifndef SLOT_ALLOCATION_UPGRADE
#define FIXED_ALLOCATE_SLOT_DELAY 3*1000000000UL
//#define FIXED_ALLOCATE_SLOT_DELAY 0
#define FIXED_TABLE_BROADCAST_DELAY 5*1000000000UL
#else
#define FIXED_ALLOCATE_SLOT_DELAY   25*100000000UL
#define FIXED_TABLE_BROADCAST_DELAY 40*100000000UL
#endif

#define PSEUDO_RANDOM_SLOT_ALLOCATION_DELAY 	(FIXED_ALLOCATE_SLOT_DELAY + 1000000UL*(rand()%1000UL))
#ifndef SLOT_ALLOCATION_UPGRADE
#define PSEUDO_RANDOM_TABLE_BROADCAST_DELAY 	(FIXED_TABLE_BROADCAST_DELAY + 7500000UL*(rand()%1000UL)) //Broadcast table on average once every 10 seconds
#else
#define PSEUDO_RANDOM_TABLE_BROADCAST_DELAY 	(FIXED_TABLE_BROADCAST_DELAY + 4000000UL*(rand()%1000UL)) //Broadcast table on average once every 6 seconds
#endif

enum slot_transaction_result:uint8_t{
	FAIL=0,
	SUCCESS=1
};

enum slot_action:uint8_t{
	add_slot=1,
	delete_slot=2,
	move_slot=3
};

enum SLOT_MODE {
    SLOT_MODE_UNKNOWN   = 0,
    RX                  = 1,
    TX                  = 2
};

//Packet formats
typedef struct __attribute__ ((packed)) {
	channel_t channel;
	slot_number_t timeslot;
	//uint8_t score;
}slot_reference;

//		ADD_SLOT=22,
typedef struct __attribute__ ((__packed__)) {
	mac_address_t initiator : 48;
	uint8_t number_of_suggested_slots;
	slot_reference slot_table[MAX_SUGGESTED_SLOTS];
}add_slot_payload;

//		DELETE_SLOT=23,
typedef struct __attribute__ ((__packed__)) {
	mac_address_t initiator : 48;
	slot_reference del_slot;
}del_slot_payload;

//		SLOT_MOVE=24
typedef struct __attribute__ ((__packed__)) {
	mac_address_t initiator : 48;
	slot_reference old_slot;
	uint8_t number_of_suggested_slots;
	slot_reference slot_table[MAX_SUGGESTED_SLOTS];
}slot_move_payload;

//		SLOT_TRANSACTION_RESULT=25
typedef struct __attribute__ ((__packed__)) {
	mac_address_t initiator : 48;
	slot_reference relevant_slot;
	slot_reference new_slot;
	slot_action action;
	slot_transaction_result result;
}slot_transaction_result_payload;

//		SLOT_TRANSACTION_ACK=26
typedef struct __attribute__ ((__packed__)) {
	mac_address_t initiator : 48;
	mac_address_t remote : 48;
	slot_reference relevant_slot;
	slot_reference new_slot;
	slot_action action;
	slot_transaction_result result;
#ifdef SLOT_ALLOCATION_EXPOSED_NODE
	SLOT_MODE mode;
#endif
}slot_transaction_ack_payload;

//		ADVERTISE TABLE=27,
typedef struct __attribute__ ((__packed__)) {
	mac_address_t table_owner : 48;
	uint8_t number_of_used_slots;
	slot_reference list_slots[MAX_USABLE_SLOTS];
#ifdef SLOT_ALLOCATION_EXPOSED_NODE
	SLOT_MODE list_slots_mode[MAX_USABLE_SLOTS];
#endif
}slot_table_advertisement_payload;

//MAC STATS
typedef struct {
	enum AckStatus {
			ACKSTATUS_UNKNOWN   = 0,
			ACKSTATUS_SUCCESS   = 1,
			ACKSTATUS_FAILED    = 2
	 };

	uint64_t destination_address; //Destination address where the ACK is received (Destination of the APP packet)
	AckStatus status; //Success or Failure (ACK received or timeout)
	uint32_t mcs; //MCS, [0, 28]
	uint32_t tx_gain; //0 - 89 dB range
	uint32_t port;
	uint32_t payload_size;
	uint32_t tx_channel; //Tx channel
	uint32_t slot_index; //Slot index
	uint32_t max_retries; //Max number or retransmissions
	uint32_t num_retries; //Number or retransmissions
	uint64_t bytes_transmitted; //Overall bytes of successfully transmitted ACKs

} Mac_ack_stat;

//MAC tx statistics
typedef struct {
    std::vector<Mac_ack_stat> ack_stat;
} Mac_tx_stat;

//MAC rx statistics
typedef struct {
	uint32_t tx_channel; 	//Tx channel
	uint32_t slot_index; 	//Slot index
} Mac_rx_stat;

typedef struct {
  uint32_t frequency_num;
  uint32_t timeslot_num;
} MFTDMA_Slot;

typedef struct {
	uint64_t mac;
	uint32_t num_slots;
} Proposed_Slots_Request;

typedef struct {
	uint64_t mac;
	std::vector<MFTDMA_Slot> slots;
} Proposed_Slots;

typedef struct {
	uint64_t mac;
	MFTDMA_Slot old_slot;
	std::vector<MFTDMA_Slot> slots;
} Proposed_Move_Slots;

typedef struct {
	uint64_t mac;
	MFTDMA_Slot slot;
} Selected_Slot;

typedef struct {
	uint64_t mac;
	MFTDMA_Slot old_slot;
	MFTDMA_Slot new_slot;
} Selected_Move_Slot;

typedef struct {
	uint64_t src_mac;
	uint64_t dst_mac;
	MFTDMA_Slot slot;
#ifdef SLOT_ALLOCATION_EXPOSED_NODE
	SLOT_MODE mode;
#endif
} External_Slot_Allocation;

typedef struct {
	uint64_t src_mac;
	uint64_t dst_mac;
	MFTDMA_Slot slot;
#ifdef SLOT_ALLOCATION_EXPOSED_NODE
	SLOT_MODE mode;
#endif
} External_Slot_Removal;

typedef struct {
	std::vector<MFTDMA_Slot> slots;
} BC_Slots_Announcement;

typedef struct {
	uint64_t mac; //Slot allocated for this mac
  MFTDMA_Slot slot; //Specified slot
  SLOT_MODE mode; //Mode: RX-TX
} Slot_Allocation;

typedef struct {
	uint64_t mac;
  MFTDMA_Slot slot;
} Slot_Removal;

typedef struct {
  uint64_t mac;
  MFTDMA_Slot old_slot;
  MFTDMA_Slot new_slot;
  SLOT_MODE mode;
} Slot_Move;

typedef struct {
	uint64_t src_mac;
	uint64_t dst_mac;
	MFTDMA_Slot old_slot;
	MFTDMA_Slot new_slot;
} External_Slot_Move;

enum SLOT_ATTRIBUTE {
	PROPOSED_SLOTS          	= 0x300,
	SELECTED_SLOT           	= 0x301,
	SELECTED_MOVE_SLOT      	= 0x302,
	EXTERNAL_SLOT_ALLOCATION  = 0x303,
	EXTERNAL_SLOT_REMOVAL     = 0x304,
	SLOT_ALLOCATION      			= 0x305,
	SLOT_REMOVAL      				= 0x306,
	BC_SLOTS_ANNOUNCEMENT     = 0x307,
	EXTERNAL_SLOT_MOVE     		= 0x308,
	SLOT_MOVE           			= 0x309
};

enum MAC_STAT_ATTRIBUTE {
	MAC_STAT_TX		= 0,
	MAC_STAT_RX		= 1
};

enum MESSAGE_ATTRIBUTE {
	MESSAGE_GET		= 0,
	MESSAGE_GETR		= 1,
	MESSAGE_SET		= 2,
	MESSAGE_SETR		= 3,
	MESSAGE_STATS		= 4
};

//MAC statistics
typedef struct {

	MAC_STAT_ATTRIBUTE attribute;

	Mac_tx_stat tx_stat;
	Mac_rx_stat rx_stat;

} Mac_stat;

typedef struct {

	SLOT_ATTRIBUTE attribute;

	Proposed_Slots_Request slot_request; //Payload for type PROPOSED_SLOTS
	Proposed_Slots proposed_slots; //Payload for type SELECTED_SLOT
	Proposed_Move_Slots proposed_move_slots; //Payload for type SELECTED_MOVE_SLOT

} Get;

typedef struct {

	SLOT_ATTRIBUTE attribute;

	Proposed_Slots 		proposed_slots; //Response for the PROPOSED_SLOTS get message
	Selected_Slot		selected_slot; //Response for the SELECTED_SLOT get message
	Selected_Move_Slot	selected_move_slot; //Response for the SELECTED_MOVE_SLOT get message
} Get_r;

typedef struct {

		SLOT_ATTRIBUTE attribute;

		External_Slot_Allocation external_slot_allocation;
		External_Slot_Removal external_slot_removal;
		Slot_Allocation slot_allocation;
		Slot_Removal    slot_removal;
    BC_Slots_Announcement bc_slots_announcement;
    External_Slot_Move external_slot_move;
    Proposed_Move_Slots proposed_move_slots;
    Slot_Move slot_move;

} Set;

enum MODULE {
	MODULE_UNKNOWN			= 0,
	MODULE_MAC 				= 1,
	MODULE_AI				= 2
};

enum TRANSACTION_RESULT {
	TRANSACTION_RESULT_UNKNOWN			= 0,
	OK						= 100,
	NOK						= 101,
	TIMEOUT				= 102,
	LBT_TIMEOUT	  = 103
};

typedef struct {
	TRANSACTION_RESULT result;
} Set_r;

typedef struct {

	uint64_t		mac_address;
	Mac_stat	mac_stat;

} Stats;

typedef struct {

	MESSAGE_ATTRIBUTE attribute;

	MODULE module;

	uint64_t		transaction_index;
	// See GET documentation
	Get 	get;

	// See GET_R documentation
	Get_r 	getr;

	// See SET documentation
	Set 	set;

	// See SET_R documentation
	Set_r 	setr;

	//See STATS documentation
	Stats 	stats;

} Internal;

#endif /* MAC_SLOT_MANAGE_PROTOCOL_H_ */
