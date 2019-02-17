/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef TDMA_TABLE_H
#define TDMA_TABLE_H

#include "ns3/object.h"
#include "neighbour_table.h"
#include "zmq_mac_interface.h"
#include "tdma_queue.h"
#include <map>
#include <utility>
#include <memory>
#include "slot_manage_protocol.h"
#include "helpers.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"

namespace ns3 {

//#define USE_TABLE_COUNTER
#define SUPERFRAME_DURATION 1000
#define DEFAULT_SLOT_COUNT SUPERFRAME_DURATION/TDMA_SLOT_DURATION
#define DEFAULT_CB_SLOT_COUNT 4
#define TDMA_TABLE_CHANNELS 14
#define TX_TIMER_OFFSET_MS 1
#define TX_OFFSET_MS 1 //0 //before ns3 value: 1
#define ROUND_TX_MS_TO_HIGHER_VALUE 1 //before ns3 value: 1
#define RX_TIMER_OFFSET_MS 2  //1
#define RX_TIMER_WAIT RX_TIMER_OFFSET_MS + TX_OFFSET_MS + 3
#define ALLIGN_OFFSET 0
#define GUARD_MS_COUNT 1

#define USE_SLOTTED_ALOHA
#define RANDOMIZED_BC_SLOTS_INTERVAL 10
#define LAST_ACK_CREATE 1

//Choose change rx_channel function to use
#define USE_RX_N01 1
#define USE_RX_NO2 2
#define USE_RX USE_RX_NO2

#if USE_RX == USE_RX_NO2
#undef RX_TIMER_WAIT
#define RX_TIMER_WAIT 0
#endif

#define STATIC_TABLE_CRISS_CROSS_ONE_CH 1
#define STATIC_TABLE_CRISS_CROSS_TWO_CH 2
#define STATIC_TABLE_FULL_TWO_CH 3
#define STATIC_TABLE_CRISS_CROSS_TWO_CH_THREE_NODES 4
#define STATIC_TABLE_ONE_RX_ONE_TX 5
#define STATIC_TABLE_RANDOM 6


//#define STATIC_TABLE_TYPE STATIC_TABLE_RANDOM

//Definitions for dropped packets filtering to initiate a new slot allocation procedure
#define PACKETS_DROPPED_THRESHOLD 2 //6

class MFTDMAMac;

class TDMA_table  : public Object {
public:

	enum table_operation_status:uint8_t {
		SUCCESS = 0,
		CHANNEL_OUT_OF_BOUNDS = 1,
		SLOT_OUT_OF_BOUNDS = 2,
		PARAMETERS_NOT_DIVISIBLE = 3,
		WRONG_SLOT_TYPE = 4,
		SLOT_USED = 5,
		SLOT_UNUSED = 6,
		NO_SLOTS_AVAILABLE = 7,
		NOT_ENOUGH_SPACE = 8,
		NOTHING_TO_SEND = 9,
		QUEUE_EXIST = 10
	};

	enum table_priority:uint8_t {
		LOW_PRIORITY = 0,
		HIGH_PRIORITY = 1
	};

	typedef struct {
		uint8_t rx_number_of_changes;
		mac_container rx_first_container;
		mac_container rx_second_container;
		uint16_t rx_first_change;
		uint16_t rx_second_change;
		channel_t rx_first_channel;
		channel_t rx_second_channel;
		uint16_t next_rx_wkup;
	} rx_timer_container;

	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);

	TDMA_table();
	//~TDMA_table();
	void TDMA_table_init(mac_address_t own_mac_address, uint8_t channel_count, uint8_t slot_count, uint16_t sframe_duration, uint8_t table_min_mcs);

	Ptr<MFTDMAMac> GetMac (void) const;
	void SetMac (Ptr<MFTDMAMac> device);

	mac_address_t get_own_mac_address() const { return own_mac_address; }
	uint8_t get_table_channel_count() { return table_channel_count; }
	uint8_t get_table_slot_count() const { return table_slot_count; }
	uint16_t get_table_sframe_duration() const { return table_sframe_duration; }
	uint8_t get_active_send_slots()const {return send_slots_active;}
	uint8_t get_active_send_slots_to(mac_address_t dest_mac);

	void set_channel_count(uint8_t channel_count);
	table_operation_status set_slot_count(uint8_t slot_count);
	table_operation_status set_sframe_duration(uint16_t sframe_duration);
	table_operation_status set_slot_count_fixed(uint8_t slot_count);
	table_operation_status set_sframe_duration_fixed(uint16_t sframe_duration);

	table_operation_status add_slot(channel_t channel, slot_number_t slot_number, TDMA_slot::tdma_slot_type slot_type, mac_address_t remote1 = 0 , mac_address_t remote2 = 0 );
	table_operation_status update_slot(channel_t channel, slot_number_t slot_number, TDMA_slot::tdma_slot_type slot_type, mac_address_t remote1 = 0, mac_address_t remote2 = 0);
	table_operation_status remove_slot(channel_t channel, slot_number_t slot_number);
	bool slot_exists_to(mac_address_t destination);
	bool check_specific_slot_exists_to(mac_address_t destination, uint8_t channel, uint8_t slot);
	bool check_specific_slot_exists_from(mac_address_t remote, uint8_t channel, uint8_t slot);
	std::pair<uint8_t, slot_reference*> get_broadcast_slots();
	bool table_get_active_slots(std::vector<std::pair<channel_t, slot_number_t>> &slots);
	bool table_get_active_slots_more(std::vector<std::pair<std::pair<channel_t, slot_number_t>, std::pair<TDMA_slot::tdma_slot_type, uint16_t>>> &slots);

	table_operation_status test_table();

	void table_align();
	int32_t table_check_drift();
	void update_table_current_position(uint64_t value);

	table_operation_status table_get_next_rx_wkup(rx_timer_container &rx_container, channel_t &rx_channel);
	table_operation_status table_get_next_rx_message_no1(rx_timer_container &rx_container, channel_t &rx_channel);
	table_operation_status table_get_next_rx_message_no2(rx_timer_container &rx_container, channel_t &rx_channel);
	table_operation_status table_get_next_tx_wkup(uint16_t &next_tx_wkup);
	table_operation_status table_get_next_tx_message(mac_container &container, uint16_t &next_tx_wkup);
#ifdef USE_QUEUES
	table_operation_status table_get_cb_message(mac_container &container, uint16_t &next_tx_wkup);
	table_operation_status table_add_packets(std::vector<Ptr<Packet>> data, mcs_t mcs, uint8_t tx_gain, udp_port_t port, std::vector<uint32_t> payload_size, TDMA_queue::tdma_queue_packet_type packet_type, mac_address_t mac_address = 0);
	table_operation_status table_add_packet(Ptr<Packet> data, mcs_t mcs, uint8_t tx_gain, udp_port_t port, uint32_t payload_size, TDMA_queue::tdma_queue_packet_type packet_type, mac_address_t mac_address = 0);
	table_operation_status table_add_control_ack(Ptr<Packet> data, mac_address_t mac_address, uint8_t tx_gain, mcs_t mcs);
	table_operation_status table_add_unicast_queue(mac_address_t remote_mac_address);
	std::map<mac_address_t,uint16_t>* table_queue_check_timeout(timestamp_t timestamp);
#endif
	table_operation_status table_add_ack(Ptr<Packet> data, mac_address_t mac_address, std::pair<channel_t, slot_number_t> &slot_info, uint8_t tx_gain, mcs_t mcs);

	table_operation_status table_create_static(mac_address_t own_mac_address, mac_address_t remote_mac_address, uint8_t cb_slots_count);
	table_operation_status table_create_static_bc_slots(uint8_t cb_slots_count);
	void print_table_info();
#ifndef USE_SLOTTED_ALOHA
	void test_table_randomize_bc_slot();
#endif

#ifdef USE_QUEUES
	void print_queues_info();
#endif

#ifdef USE_SLOTTED_ALOHA
	Ptr<neighbour_table> ntable;
	//std::unique_ptr<neighbour_table> ntable;
#endif

//with every send or receive slot added update parameters for callbacks above (if needed to be called earlier on these just added slots)
//get_whole message (mcs needs to be checked for every packet)
//TODO add debug messages, test everything
private:

	table_operation_status check_if_divisible(uint16_t first_number, uint16_t second_number);
	table_operation_status check_channel_and_slot_number(channel_t channel, slot_number_t slot_number);
	table_operation_status check_data_size(uint16_t data_size, std::vector<uint16_t> &slot_free_sizes, std::vector<Ptr<TDMA_slot>> slots, bool check_type);
	table_operation_status check_data_size_high_priority(uint16_t data_size, std::vector<uint16_t> &slot_free_sizes, std::vector<Ptr<TDMA_slot>> slots, bool check_type);
	uint16_t table_get_ms_in_sframe();
	uint16_t table_get_ms_in_sframe_unicast();
	uint16_t table_get_ms_in_slot();
	uint8_t table_get_slot_number();
	uint8_t table_get_slot_number_unicast();
	uint16_t table_get_tx_ms_in_sframe();
	uint16_t table_get_tx_ms_in_slot();
	uint8_t table_get_tx_slot_number();
	table_operation_status table_get_slots(std::vector<Ptr<TDMA_slot>> &slots, 	std::vector<std::pair<channel_t, slot_number_t>> &slots_info, TDMA_slot::tdma_slot_type slot_type, bool &current_slot, mac_address_t mac_address);
	uint64_t clock_get_time_ms();
#ifndef USE_SLOTTED_ALOHA
	std::vector<uint16_t> specific_values;
	bool table_randomize_bc_slot(uint16_t &sleep_for, uint16_t ms_in_slot);
	void table_generate_specific_values();
#endif

	Ptr<MFTDMAMac> m_mac;

	mac_address_t own_mac_address;
	uint8_t table_channel_count;
	uint8_t table_slot_count;
	uint16_t table_sframe_duration;
	uint8_t broadcast_slots_count;
	slot_reference broadcast_slots[4];
	uint8_t min_mcs;
	TracedValue<uint8_t> send_slots_active;
	TracedValue<uint8_t> receive_slots_active;
	TracedValue<uint32_t> timeout_drop_count;

	uint64_t table_current_pos;

  std::map<std::pair<channel_t, slot_number_t>, Ptr<TDMA_slot>> table;

#ifdef USE_QUEUES
    std::map<mac_address_t, Ptr<TDMA_queue>> table_queues;
#endif
};

}

#endif /* TDMA_TABLE_H */
