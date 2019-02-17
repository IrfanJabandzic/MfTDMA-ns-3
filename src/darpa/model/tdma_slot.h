/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef TDMA_SLOT_H
#define TDMA_SLOT_H

#include "ns3/object.h"
#include <cstdint>
#include <vector>
#include "mac_types.h"
//#include "MacModule.h"
//#include "helpers.h"
#include <mutex>

namespace ns3 {
//#define USE_SLOTS

#define TDMA_SLOT_DURATION 50
#define TDMA_ACK_QUEUE_SIZE 6
#define TDMA_ACK_SLOT_GUARD_SPACE 1

class Packet;

class TDMA_slot : public Object
{
public:

	enum tdma_slot_type:uint8_t {
		UNUSED = 0,
		BLOCKED = 1,
		USED =2,
		CONTROL_BROADCAST = 3,
		RECEIVE = 4,
		UNICAST_SEND = 5
	};

	enum queue_operation_status:uint8_t {
		SUCCESS = 0,
		QUEUE_EMPTY = 1,
		QUEUE_FULL = 2,
		INDEX_OUT_OF_BOUNDS = 3
	};

	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);
	TDMA_slot();
	//~TDMA_slot();

	void TDMA_slot_init(uint16_t slot_size, uint8_t def_min_mcs);

	tdma_slot_type get_slot_type();
	mac_address_t get_own_mac_address() const { return own_mac_address; }
	mac_address_t get_remote_mac_address();
	uint16_t get_slot_size();
	void get_slot_info(tdma_slot_type &type, uint16_t &size, mac_address_t &remote_address);
	uint16_t get_slot_last_index() const { return slot_last_index; }
	uint16_t get_slot_sent_index() const { return slot_sent_index; }
	uint16_t get_free_size() const { return slot_size - slot_last_index - slot_sent_index; }
	bool is_empty() const { if(slot_last_index == 0) return true; else return false; }
	bool is_full() const { if(slot_last_index == slot_size) return true; else return false; }

	void block_slot();
	void remove_slot();
	void clear_slot();
	void set_control_broadcast_slot(mac_address_t own_mac_address);
	void set_receive_slot(mac_address_t own_mac_address, mac_address_t remote_mac_address);
	void set_unicast_send_slot(mac_address_t own_mac_address, mac_address_t remote_mac_address);
	void set_used_slot(mac_address_t remote_1, mac_address_t remote_2);

	void set_slot_size(uint16_t slot_size);
	void set_slot_sent_index(uint16_t slot_sent_index) { this->slot_sent_index = slot_sent_index; }

	queue_operation_status push_back(Ptr<Packet>, mcs_t mcs, uint32_t tx_gain);
	queue_operation_status push_front(Ptr<Packet>, mcs_t mcs, uint32_t tx_gain);
	queue_operation_status get_data(std::vector<Ptr<Packet>> &data, std::vector<std::pair<mcs_t, uint32_t>> &mcs_gain, uint16_t &last_index);
	queue_operation_status get_data_mcs_gain(std::vector<Ptr<Packet>> &data, std::pair<mcs_t, uint32_t> &mcs_gain, uint16_t &packet_count);

	void print_slot_info();

private:

	void push(Ptr<Packet>, uint16_t index, mcs_t mcs, uint32_t tx_gain);
	void push_with_erase(Ptr<Packet>, uint16_t index, mcs_t mcs, uint32_t tx_gain);
	void clear_data();

	tdma_slot_type slot_type;
	mac_address_t own_mac_address;
	mac_address_t remote_mac_address;
	uint16_t slot_size;
	uint16_t slot_last_index;
	uint16_t slot_sent_index;
	uint8_t min_mcs;
	uint16_t max_slot_size = 0;

	std::vector<Ptr<Packet>> slot_data;
	std::vector<std::pair<mcs_t, uint32_t>> slot_mcs_gain;

	std::mutex slot_mutex;
};

}

#endif /* TDMA_SLOT_H */
