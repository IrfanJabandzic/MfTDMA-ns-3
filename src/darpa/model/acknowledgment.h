/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef ACKNOWLEDGMENT_H
#define ACKNOWLEDGMENT_H

#include <cstdint>
#include <map>
#include <list>
#include "ack_moving_sample.h"
#include "mac_types.h"
#include <mutex>
#include "ns3/object.h"

namespace ns3 {

//Retnrasmissions control knob
#define RETRANSMISSIONS //undef to disable retransmissions
#define TDMA_USE_ACK
#define TDMA_RETRANSMISSIONS

//#ifdef RETRANSMISSIONS
#define RETRY_COUNTER_MAX 3
//#endif

#define MOVING_SAMPLES_BUFFER_SIZE 100
#define ACK_MOVING_SAMPLES_TIMEOUT 10000000000ul //(10s)

#define ACK_GUARD_TIME 50000000UL // 50msec
#define TDMA_ACK_GUARD_TIME 3000000000UL // 3sec

#define ACK_MAX_TIMEOUT 50*1000000UL + ACK_BUFFER_TIMEOUT + ACK_GUARD_TIME // 50msec + ACK_BUFFER_TIMEOUT (55msec) + ACK_GUARD_TIME (50msec) = 155 msec
#define ACK_CONTROL_MAX_TIMEOUT 25*1000000UL // 50msec
#define ACK_BUFFER_SIZE 11 //At most 11 ACK in the buffer per destination
#define ACK_BUFFER_TIMEOUT 5* 1000000UL * ACK_BUFFER_SIZE // 5ms * ACK_BUFFER_SIZE (11) = 55msec

#define ACK_SLOT_GUARD_SPACE 10

class MFTDMAMac;
class Packet;

class Acknowledgment : public Object
{
public:

    //ACK buffer data structure
    struct ack_buffer_data{
        //Header info
        mac_address_t destination_address;
        mac_index_t mac_packet_index;
        fragment_index_t fragment_index;
        fragment_index_max_t fragment_index_max;
        cqi_t cqi;
        rssi_t rssi;

        timestamp_t timestamp;
    };

    //ACK Aggregated structure
    struct __attribute__ ((__packed__)) ack_preamble {
        uint8_t number_of_acks;
        //std::string* acks_data;
    };

    //ACK payload structure
    struct __attribute__ ((__packed__)) ack_payload{
        mac_index_t mac_index;
        fragment_index_t fragment_index;
        cqi_t cqi;
        rssi_t rssi;
    };

    enum ack_status:uint8_t {
        UNKNOWN = 0,
        SUCCESS = 1,
        FAILED = 2
    };

    //Individual ACKs stats structure
    struct ack_statistics {
        mac_address_t destination_address;       //Destination address from where an ACK is received (Destination of the APP packet)
        ack_status status;                       //Success or Failure (ACK received or timeout)
        mcs_t mcs;                               //MCS, [0, 28]
        gain_t tx_gain;                          //0 - 89 dB range
        udp_port_t port;
        uint32_t payload_size;
        channel_t tx_channel;                    //Tx channel
#ifdef TDMA_USE_ACK
        slot_number_t slot_index;                //Slot index
#endif
        uint8_t max_retry_counter;               //Max number or retransmissions
        uint8_t retry_counter;                   //Number or retransmissions
        byte_counter_t bytes_transmitted;        //Overall bytes of successfully transmitted ACKs
    };

    //Aggregated ACKs stats structure
    struct ack_aggregated_statistics {
        //Aggregated Counters
        packet_counter_t number_of_sent_packets;
        packet_counter_t number_of_received_acks;
        packet_counter_t ack_timeout_counter;
        packet_counter_t bytes_transmitted;
        packet_counter_t last_updated_timestamp;

        //Moving samples
        ACKMovingSample<MOVING_SAMPLES_BUFFER_SIZE> acks_moving_samples;
    };

    //Structure to comprise the destination_mac_address and the timestamp
    struct ack_needed_info {
        mac_address_t destination_mac_address;
        mcs_t mcs;
        gain_t tx_gain;
        udp_port_t port;
        uint32_t payload_size;
        channel_t tx_channel;
#ifdef TDMA_USE_ACK
        slot_number_t slot_index;
        uint8_t slot_type;
        uint8_t priority;
        bool send_ack_stats;
#endif
        timestamp_t timestamp;
#ifdef RETRANSMISSIONS
        uint8_t max_retry_counter;
        uint8_t retry_counter;
        Ptr<Packet> packet;
#endif
    };

    //Structure for retransmission packets info
    struct retransmission_info {
        mac_index_t mac_index;
        fragment_index_t fragment_index;
        mac_address_t destination_mac_address;
        mcs_t mcs;
        gain_t tx_gain;
        udp_port_t port;
        uint32_t payload_size;
        uint8_t slot_type;
        uint8_t priority;
        Ptr<Packet> packet;
    };

		/**
		 * \brief Get the type ID.
		 * \return the object TypeId
		 */
		static TypeId GetTypeId (void);
		Acknowledgment();
		//~Acknowledgment();

		Ptr<MFTDMAMac> GetMac (void) const;
		void SetMac (Ptr<MFTDMAMac> node);

/**
 * Statistics Setters
 */
    void increment_number_of_received_acks(mac_address_t addr, packet_counter_t val, timestamp_t current_timestamp);

    void increment_number_of_sent_packets(mac_address_t addr, packet_counter_t val, timestamp_t current_timestamp);

    void increment_ack_timeout_counter(mac_address_t addr, packet_counter_t val, timestamp_t current_timestamp);

    void clear_ack_stats_list();

/**
 * Getters
 */
    bool get_ack_needed(std::string *data);

    bool check_exceeded_ack_buffer_data(mac_address_t destination_mac_address);

    std::list<ack_buffer_data>* get_ack_buffer_data(mac_address_t destination_mac_address);

    bool get_ack_needed_info(mac_index_t mac_index, fragment_index_t fragment_index, Acknowledgment::ack_needed_info &);

    std::list<ack_statistics>* get_ack_stats_list();

/**
 * CRUD Operations
 */
    bool remove_ack_needed_from_map(mac_index_t mac_index,
                                    fragment_index_t fragment_index);
    bool add_ack_needed_in_map(mac_index_t mac_index,
                               fragment_index_t fragment_index,
                               mac_address_t destination_mac_address,
                               mcs_t mcs,
                               gain_t tx_gain,
															 udp_port_t port,
															 uint32_t payload_size,
                               channel_t tx_channel,
															 slot_number_t slot_index,
															 uint8_t slot_type,
															 uint8_t priority,
                               timestamp_t current_timestamp,
															 uint8_t retry_counter_max,
															 Ptr<Packet> packet);

    bool update_ack_needed_in_map(mac_index_t mac_index,
                                  fragment_index_t fragment_index,
																	bool update_status,
																	timestamp_t current_timestamp,
                                  channel_t tx_channel = 0,
																	slot_number_t slot_index = 0);

    void add_ack_moving_sample(mac_address_t addr,
                               packet_counter_t number_of_sent_packets,
                               packet_counter_t number_of_received_acks,
                               packet_counter_t ack_timeout_counters,
                               timestamp_t current_timestamp);

    bool add_ack_data_into_buffer(mac_address_t destination_mac_address,
                                  mac_index_t mac_packet_index,
                                  fragment_index_t fragment_index,
                                  fragment_index_max_t fragment_index_max,
                                  cqi_t cqi,
                                  rssi_t rssi,
                                  timestamp_t current_timestamp);

    void update_ack_moving_sample(timestamp_t current_timestamp);

#ifdef TDMA_USE_ACK
	uint64_t get_ack_max_timeout() const { return ack_max_timeout; }
	void set_ack_max_timeout(uint64_t ack_max_timeout) { this->ack_max_timeout = ack_max_timeout * 1000000UL; }
	uint64_t get_control_ack_max_timeout() const { return ack_control_max_timeout; }
	void set_control_ack_max_timeout(uint64_t ack_control_max_timeout) { this->ack_control_max_timeout = ack_control_max_timeout; }
	//void set_active_bw_index( communicator::BW_INDEX bw_index  ) { this->active_bw_index = bw_index; }
	void set_active_bw_index( uint8_t bw_index  ) { this->active_bw_index = bw_index; }

#endif

/**
 * Timeout Functions
 */
#ifndef TDMA_USE_ACK
    std::list<communicator::Message>* check_ack_timeout(timestamp_t current_timestamp);
#else
    std::list<retransmission_info>* check_ack_timeout(timestamp_t current_timestamp);
#endif
    std::list<mac_address_t>* get_ack_buffer_destinations_timeout_list(timestamp_t current_timestamp);
    void check_ack_moving_samples_timeout(timestamp_t current_timestamp);

/**
* Print Statistics
*/
    void print_ack_aggregated_statistics();

private:
  	Ptr<MFTDMAMac> m_mac;

    //Map to store ACK stats per destination_mac_address
    std::map<mac_address_t, Acknowledgment::ack_aggregated_statistics> aggregated_ack_stats_map;

    //ACK needed map with key = mac_index, fragment_index and value = destination_mac_address and timestamp
    std::map<std::pair<mac_index_t, fragment_index_t>, Acknowledgment::ack_needed_info> ack_needed_map;

    //Map to group the ACKs to be sent per destination_mac_address
    std::map<mac_address_t, std::list<Acknowledgment::ack_buffer_data>> ack_buffer_map;

    //List of ack_stats for those ACKs that have time out
    std::list<Acknowledgment::ack_statistics> ack_stats_list;

    std::mutex ack_needed_mutex;
    std::mutex aggregated_ack_stats_mutex;

#ifdef TDMA_USE_ACK
    uint64_t ack_max_timeout;
    uint64_t ack_control_max_timeout;
    //communicator::BW_INDEX active_bw_index; //TODO fix this
    uint8_t active_bw_index;
#endif

};

}

#endif /* ACKNOWLEDGMENT_H */
