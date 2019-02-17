/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef TDMA_QUEUE_H
#define TDMA_QUEUE_H

#include "ns3/object.h"
#include <map>
#include <utility>
#include "tdma_slot.h"

namespace ns3 {

#define USE_QUEUES

#define OLDER_DATA_LAST 1
#define OLDER_DATA_FIRST 2
#define DATA_ORDER OLDER_DATA_LAST
//#define FILTER_HIGH_PRIORITY_APP_DATA

#define QUEUES_CHECK_TIMEOUT_PERIOD 50000000

class Packet;

class TDMA_queue : public Object {
public:
	enum queue_operation_status:uint8_t {
		SUCCESS = 0,
		QUEUE_EMPTY = 1,
		QUEUE_FULL = 2,
		QUEUE_DELAYED = 3,
		INDEX_OUT_OF_BOUNDS = 4
	};

	enum tdma_queue_type:uint8_t {
		UNICAST = 0,
		BROADCAST_CONTROL = 1
	};

	enum tdma_queue_packet_type:uint8_t {
		APP_HIGH_PRIORITY = 0,
		APP_LOW_PRIORITY = 1,
		BROADCAST = 2,
		CONTROL = 3,
		RETRANSMISSION = 4,
		AI = 5,
		CONTROL_ACK = 6
	};

	typedef struct {
		tdma_queue_packet_type packet_type;
		Ptr<Packet> data;
		std::pair<mcs_t, uint32_t> queue_mcs_gain;
		udp_port_t port;
		uint32_t payload_size;
		timestamp_t queue_timestamp;
	} queue_container;

	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);
	TDMA_queue();
	//virtual ~TDMA_queue();

	void TDMA_queue_init(tdma_queue_type queue_type, uint64_t queue_timeout, mac_address_t own_mac_address, mac_address_t remote_mac_address = 0);

	tdma_queue_type get_queue_type() const { return queue_type; }
	mac_address_t get_own_mac_address() const { return own_mac_address; }
	mac_address_t get_remote_mac_address() const { return remote_mac_address; }
	uint64_t get_queue_timeout() const { return queue_timeout; }
	void set_queue_timeout(uint64_t queue_timeout) { this->queue_timeout = queue_timeout; }
	uint16_t get_queue_high_priority_size() const { return queue_first_index; }
	uint16_t get_queue_retransmission_size() const;
	uint16_t get_queue_high_priority_data_size() const;
	uint16_t get_queue_broadcast_size() const;
	uint16_t get_queue_control_size() const;
	uint16_t get_queue_low_priority_size() const { return queue_last_index - queue_first_index; }
	uint16_t get_queue_size() const { return queue_last_index; }
	bool is_empty() const { if(queue_last_index == 0) return true; else return false; }

	queue_operation_status push_back_high_priority(Ptr<Packet> data, TDMA_queue::tdma_queue_packet_type packet_type, mcs_t mcs, uint32_t tx_gain, udp_port_t port, uint32_t payload_size, timestamp_t timestamp);
	queue_operation_status push_front_high_priority(Ptr<Packet> data, TDMA_queue::tdma_queue_packet_type packet_type, mcs_t mcs, uint32_t tx_gain, udp_port_t port, uint32_t payload_size, timestamp_t timestamp);
	queue_operation_status push_back_low_priority(Ptr<Packet> data, TDMA_queue::tdma_queue_packet_type packet_type, mcs_t mcs, uint32_t tx_gain, udp_port_t port, uint32_t payload_size, timestamp_t timestamp);
	queue_operation_status push_front_low_priority(Ptr<Packet> data, TDMA_queue::tdma_queue_packet_type packet_type, mcs_t mcs, uint32_t tx_gain, udp_port_t port, uint32_t payload_size, timestamp_t timestamp);
	queue_operation_status push_to_position_low_priority(Ptr<Packet> data, TDMA_queue::tdma_queue_packet_type packet_type, mcs_t mcs, uint32_t tx_gain, udp_port_t port, uint32_t payload_size, timestamp_t timestamp, uint16_t position);
	queue_operation_status queue_get_data(std::vector<Ptr<Packet>> &data, std::vector<std::pair<mcs_t, uint32_t>> &mcs_gain, std::vector<udp_port_t> &port, std::vector<uint32_t> &payload_size, uint16_t &last_index);
	queue_operation_status queue_get_data_mcs_gain(std::vector<Ptr<Packet>> &data, std::pair<mcs_t, uint32_t> &mcs_gain, std::vector<udp_port_t> &port, std::vector<uint32_t> &payload_size, uint16_t &packet_count, uint16_t &reduce_value);
	uint16_t queue_check_timeout(timestamp_t timestamp);

	queue_operation_status test_data();
	void clear_data();
	void print_queue_info();

private:

	tdma_queue_type queue_type;
	mac_address_t own_mac_address;
	mac_address_t remote_mac_address;
	uint64_t queue_timeout;
	uint16_t queue_first_index;
	uint16_t queue_last_index;

	std::vector<queue_container> queue_data;

	std::mutex tdma_queue_mutex;
};

}

#endif /* TDMA_QUEUE_H_ */
