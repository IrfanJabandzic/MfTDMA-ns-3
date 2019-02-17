/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef MFTDMA_MAC_H
#define MFTDMA_MAC_H

#include "ns3/object.h"
#include "ns3/mac48-address.h"
#include "mftdma-header.h"
#include "tdma_table.h"
#include "acknowledgment.h"
#include "slot_manage_protocol.h"
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/output-stream-wrapper.h"

namespace ns3 {

class DarpaWirelessChannel;
class MFTDMAController;
class MFTDMAMacNetDevice;
class Packet;

struct mac_statistics {

    //Packet counting related stats
    uint32_t packets_sent_from_application=0;
    uint32_t packets_success_phy_sent=0;
    uint32_t packets_phy_received=0;
    uint32_t packets_sent_to_app=0;
    uint32_t packets_lost_error_phy_sent=0;
    uint32_t packets_lost_not_acked=0;
    uint32_t packets_received_crc_error=0;
    uint32_t packets_received_wrong_MAC_header=0;
    uint32_t packets_discarded_delayed=0;
    uint32_t packets_discarded_no_space=0;
    //delay related stats
    uint32_t queue_high_average_delay=0;
    uint32_t queue_low_average_delay=0;
    uint32_t mac_average_packet_delay=0;

    //queue other stats
    uint32_t queue_high_commands_remaining=0;
    uint32_t queue_low_packets_remaining=0;
};

typedef struct {
	SLOT_ATTRIBUTE set_type;
	SLOT_MODE mode;
	//uint64_t transaction_id;
	Internal message_p;
}set_process;

class MFTDMAMac : public Object
{
public:
	typedef Callback< void, Ptr<Packet>, uint16_t, Mac48Address, Mac48Address > ReceiveCallback;

	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);
	MFTDMAMac();
	//~MFTDMAMac();

	void MFTDMAMacInit(uint32_t &max_band, uint32_t &channel_step, uint32_t &init_tx_gain, uint32_t &slots_count, uint8_t &cb_slots_count, uint64_t own_mac_address, Ptr<OutputStreamWrapper> log);

  void Send (Ptr<Packet> p, uint16_t protocol, Mac48Address to, Mac48Address from);

	void Receive (Ptr<Packet> packet, uint16_t protocol, Mac48Address to, Mac48Address from);

	void SetForwardUpCallback (Callback< void, Ptr<Packet>, uint16_t, Mac48Address, Mac48Address > upCallback);

  Ptr<TDMA_table> GetTable (void) const;
  void SetTable (Ptr<TDMA_table> table);

	Ptr<MFTDMAMacNetDevice> GetDevice (void) const;
	void SetDevice (Ptr<MFTDMAMacNetDevice> device);
	Ptr<DarpaWirelessChannel> GetChannel (void) const;
	void SetChannel (Ptr<DarpaWirelessChannel> channel);
	Ptr<MFTDMAController> GetController (void) const;
	void SetController (Ptr<MFTDMAController> controller);

	void SetRxChannel(channel_t rx_channel);
	channel_t GetRxChannel(void);

	void TDMA_MAC_send(Ptr<Packet> packet_data, type_of_message message_type, uint16_t protocol, Mac48Address to, Mac48Address from, bool need_ack);

	bool TDMA_prepare_and_send_last_ack(mac_address_t destination_address, std::vector<Ptr<Packet>> &data, std::pair<mcs_t, uint32_t> &mcs_gain, std::pair<channel_t, slot_number_t> slot_info);

  uint16_t get_previous_packets_per_second() const { return previous_packets_per_second; }

  void reduce_previous_packets_per_second(uint16_t reduce_value) { if(reduce_value > previous_packets_per_second) previous_packets_per_second = 0; else previous_packets_per_second -= reduce_value; }

  void increase_previous_packets_per_second(uint16_t increase_value) { previous_packets_per_second += increase_value; }

  void process_other_MAC(void);
  void process_AI_message(Internal &message);
  uint16_t tx_MAC();

	TDMA_table::table_operation_status GetRxMessage(TDMA_table::rx_timer_container &rx_container, channel_t &rx_channel);

	void block_low_queue();
	void unblock_low_queue();

	bool control_ack_in_list(mac_address_t source_address, mac_index_t mac_packet_index);
	void remove_control_ack_in_list(mac_address_t source_address, mac_index_t mac_packet_index);
	bool check_received_control_ack(mac_address_t source_address, mac_index_t mac_packet_index);
	void add_received_control_ack(mac_address_t source_address, mac_index_t mac_packet_index);

	bool table_get_active_slots_more(std::vector<std::pair<std::pair<channel_t, slot_number_t>, std::pair<TDMA_slot::tdma_slot_type, uint16_t>>> &slots);

	Mac48Address GetAddress (void);

private:
	Ptr<MFTDMAMacNetDevice> m_device;
	Ptr<DarpaWirelessChannel> m_channel;
	Ptr<MFTDMAController> m_controller;

	ReceiveCallback m_rxCallback;

	mac_index_t mac_index;
	bool update_timer;
	uint16_t packets_sent;
	uint16_t next_tx_wkup;
	uint16_t test_variable;
	channel_t rx_channel;

	uint64_t last_add_slot_timestamp;
#ifndef STATIC_TABLE_TYPE
	uint64_t last_table_usage_broadcast_timestamp;
#endif
	uint64_t last_queues_check_timeout;
#ifdef USE_SLOTTED_ALOHA
#ifndef STATIC_TABLE_TYPE
	uint64_t last_neighbour_table_update;
#endif
#endif

  uint32_t max_bandwidth;
  uint32_t channel_step;
  uint32_t init_tx_gain;
  uint8_t max_channel;
  uint8_t min_mcs;
  mac_address_t own_mac_address;
  Mac48Address ns3_own_mac_address;
  uint8_t supeframe_num_slots;
  uint16_t packets_per_second;
  uint16_t previous_packets_per_second;
  uint16_t max_packets_per_second;

  std::vector<std::pair<mac_address_t, mac_index_t>> received_control_acks;
  std::vector<std::pair<mac_address_t, mac_index_t>> control_acks;
  std::vector<Ptr<Packet>> packet_data_vector;
  std::string *ack_data;

  std::mutex ack_send_mutex;

  Ptr<TDMA_table> superframe;

  //Flag to guard not starting another slot allocation procedure while running the last stage of the SET-SET_R of the previous one which might results allocating more than maximum allowed in 6MHz scenarios
  bool final_slot_allocation_in_progress;
  bool m_started;
	bool queue_is_blocked;

	bool slot_delay_calculated, br_delay_calculated;
	uint64_t br_delay, slot_delay;

	std::map<uint64_t,set_process*> procedures_table;

	//Stats
	TracedValue<uint32_t> send_control_count;
	TracedValue<uint32_t> send_control_no_ack_count;
	TracedValue<uint32_t> send_control_no_ack_advertise_count;
	TracedValue<uint32_t> send_data_count;
	TracedValue<uint32_t> ack_control_count;
	TracedValue<uint32_t> ack_data_count;
	TracedValue<uint32_t> drop_count;
	TracedValue<uint32_t> entered_count;
	TracedValue<uint32_t> retr_control_count;
	TracedValue<uint32_t> retr_data_count;
	struct mac_statistics mac_stats;
	uint8_t rx_tx_buffer[256] = {0};

  BW_INDEX get_active_channel_bandwitdh_index();

  Mac48Address get_ns3_mac(mac_address_t mac);

  //boost::circular_buffer<std::pair<mac_address_t, mac_index_t>> received_app_packets(RECEIVED_APP_PACKETS_SIZE);
  //bool received_app_packet_in_list(mac_address_t source_address, mac_index_t mac_packet_index);

  void Packet_pad(Ptr<Packet> data, uint16_t slot_size, mac_header header);
  uint8_t Packet_unpad(Ptr<Packet> data);

#ifdef TDMA_USE_ACK
    Acknowledgment ack;

    void TDMA_prepare_and_send_ack(mac_address_t destination_address);
    void TDMA_prepare_and_send_control_ack(mac_address_t destination_address, mac_index_t mac_index,
    									   fragment_index_t fragment_index, cqi_t cqi, rssi_t rssi);
    void TDMA_aggregate_ack_into_buffer(mac_address_t destination_address, mac_index_t mac_index, fragment_index_t fragment_index, fragment_index_max_t fragment_index_max, cqi_t cqi, rssi_t rssi, timestamp_t current_timestamp);
    void TDMA_handle_received_ack(Ptr<Packet> app_data);
    void TDMA_handle_received_control_ack(Ptr<Packet> app_data);
    void TDMA_send_ack_stats_to_AI(Acknowledgment::ack_statistics ack_stats);
    void TDMA_send_mac_send_stats_to_AI(timestamp_t timestamp, uint16_t packet_count, channel_t channel);
    void TDMA_send_receiver_slot_stats_to_AI(std::pair<channel_t, slot_number_t> slot_info, mac_address_t sender_address);

    void TDMA_get_control_ack_info(Ptr<Packet> app_data, mac_index_t &mac_index);
#endif

    void add_procedure(SLOT_ATTRIBUTE set_type, SLOT_MODE mode, uint64_t transaction_id, Internal mes);
    set_process* procedure_exists(uint64_t trans_id);
    void delete_procedure(uint64_t trans_id);

    /**
     * Slot allocation protocol functions
     */
    void TDMA_initiate_allocate_new_slot(mac_address_t target);
    void TDMA_send_GET_selected_slot_to_AI(mac_address_t initiator, uint8_t number__of_proposals, slot_reference slot_table[]);
    void TDMA_send_GET_selected_move_slot_to_AI(mac_address_t initiator, uint8_t number_of_proposals, slot_reference old_slot, slot_reference slot_table[]);
    void TDMA_send_SET_slot_allocation_to_AI(mac_address_t remote_node, SLOT_MODE mode, uint8_t channel, uint8_t timeslot );
    void TDMA_send_SET_slot_move_to_AI(mac_address_t remote_node, SLOT_MODE mode, uint8_t old_channel, uint8_t old_timeslot, uint8_t new_channel, uint8_t new_timeslot);
    void TDMA_send_SET_external_slot_allocation_to_AI( mac_address_t sender,mac_address_t receiver, uint8_t channel, uint8_t timeslot );
    void TDMA_send_SET_BC_slot_allocation_to_AI(std::pair<uint8_t, slot_reference*> br_slots );

    void TDMA_send_proposed_slots_packet(mac_address_t destination, uint8_t number__of_proposals, slot_reference slot_table[]);
    void TDMA_send_selected_slot_packet(mac_address_t initiator, slot_action action, uint8_t channel, uint8_t slot, uint8_t new_channel = 0, uint8_t new_slot = 0);


    /**
     * Slot Delete protocol functions
    */
    //void TDMA_initiate_delete_slot (channel_t channel, slot_number_t slot_number);

    void TDMA_send_SET_Slot_removal_to_AI(mac_address_t initiator, uint8_t channel, uint8_t slot);
    void TDMA_send_SET_external_slot_removal_to_AI( mac_address_t sender,mac_address_t receiver, uint8_t channel, uint8_t timeslot );
    void TDMA_send_SET_external_slot_move_to_AI( mac_address_t sender,mac_address_t receiver, uint8_t channel_old, uint8_t timeslot_old, uint8_t channel_new, uint8_t timeslot_new );
    void TDMA_send_proposed_move_slots_packet(mac_address_t remote_node, uint8_t number_of_proposals, slot_reference old_slot, slot_reference slot_table[]);

    void TDMA_send_remove_slot_packet(mac_address_t remote_node, uint8_t channel_old, uint8_t slot);
    //void TDMA_handle_incoming_slot_management_packet(::communicator::Application_data* packet_data);

    void TDMA_send_transaction_ack_packet(mac_address_t remote_node, slot_action action,slot_reference slot, slot_reference slot_new = {0, 0});
    void TDMA_send_slot_table_usage_packet();
};

}

#endif /* MFTDMA_MAC_H */

