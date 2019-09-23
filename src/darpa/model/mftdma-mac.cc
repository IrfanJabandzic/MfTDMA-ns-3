/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "mftdma-mac.h"
#include "darpa-wireless-channel.h"
#include "mftdma-controller.h"
#include "mftdma-mac-net-device.h"
#include "ns3/packet.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/ethernet-header.h"
#include <net/ethernet.h>
#include "helpers.h"
#include "ns3/rng-seed-manager.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MFTDMAMac");

NS_OBJECT_ENSURE_REGISTERED (MFTDMAMac);

TypeId
MFTDMAMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MFTDMAMac")
    .SetParent<Object> ()
    .SetGroupName("Darpa")
    .AddConstructor<MFTDMAMac> ()
    .AddAttribute ("Table", "The TDMA table attached to this MAC.",
                   PointerValue (),
                   MakePointerAccessor (&MFTDMAMac::GetTable,
                                        &MFTDMAMac::SetTable),
                   MakePointerChecker<TDMA_table> ())
    .AddTraceSource ("SendControlCount",
                     "Send control count.",
                     MakeTraceSourceAccessor (&MFTDMAMac::send_control_count),
                     "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("SendControlNoAckCount",
										 "Send control no ack count.",
										 MakeTraceSourceAccessor (&MFTDMAMac::send_control_no_ack_count),
										 "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("SendControlNoAckAdvertiseCount",
										 "Send control no ack advertise count.",
										 MakeTraceSourceAccessor (&MFTDMAMac::send_control_no_ack_advertise_count),
										 "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("SendDataCount",
										 "Send data count.",
										 MakeTraceSourceAccessor (&MFTDMAMac::send_data_count),
										 "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("AckControlCount",
										 "Ack control count.",
										 MakeTraceSourceAccessor (&MFTDMAMac::ack_control_count),
										 "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("AckDataCount",
										 "Ack data count.",
										 MakeTraceSourceAccessor (&MFTDMAMac::ack_data_count),
										 "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("DropCount",
										 "Drop count.",
										 MakeTraceSourceAccessor (&MFTDMAMac::drop_count),
										 "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("EnteredCount",
										 "Entered count.",
										 MakeTraceSourceAccessor (&MFTDMAMac::entered_count),
										 "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("RetrControlCount",
										 "Retr. control packets count.",
										 MakeTraceSourceAccessor (&MFTDMAMac::retr_control_count),
										 "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("RetrDataCount",
										 "Retr. data packets count.",
										 MakeTraceSourceAccessor (&MFTDMAMac::retr_data_count),
										 "ns3::TracedValueCallback::Uint32")
  ;
  return tid;
}

MFTDMAMac::MFTDMAMac() :
	m_device (0),
	m_channel (0),
	m_controller(0),
	m_started(false),
	queue_is_blocked(false),
	packets_per_second(0),
	previous_packets_per_second(0),
	own_mac_address(0),
	min_mcs(0),
	final_slot_allocation_in_progress(false),
	mac_index(0),
	update_timer(false),
	packets_sent(0),
	test_variable(0),
	rx_channel(0xFF),
	packet_data_vector(1),
	slot_delay_calculated(false),
	br_delay_calculated(false),
	br_delay(0),
	slot_delay(0)
{
	last_add_slot_timestamp = 0;
#ifndef STATIC_TABLE_TYPE
	last_table_usage_broadcast_timestamp = 0;
#endif
	last_queues_check_timeout = 0;
#ifdef USE_SLOTTED_ALOHA
#ifndef STATIC_TABLE_TYPE
	last_neighbour_table_update = 0;
#endif
#endif

	//Stats
	send_control_count = 0;
	send_control_no_ack_count = 0;
	send_control_no_ack_advertise_count = 0;
	send_data_count = 0;
	ack_control_count = 0;
	ack_data_count = 0;
	drop_count = 0;
	entered_count = 0;
	retr_control_count = 0;
	retr_data_count = 0;

	received_control_acks.reserve(16);
	control_acks.reserve(16);
	ack_data = new std::string();
	ack_data->reserve(100);

	NS_LOG_FUNCTION (this);
}

void MFTDMAMac::MFTDMAMacInit(uint32_t &max_band, uint32_t &channel_step, uint32_t &init_tx_gain, uint32_t &slots_count, uint8_t &cb_slots_count, uint64_t own_mac_address, Ptr<OutputStreamWrapper> log)
{
	NS_LOG_FUNCTION (this);

  this->max_bandwidth = max_band;
  this->channel_step = channel_step;
  this->init_tx_gain = init_tx_gain;
//Calculate the max channels supported based on MAX bandwidth and Channel step
  this->max_channel = (this->max_bandwidth/this->channel_step) - 1;
  this->supeframe_num_slots = (uint8_t) slots_count;
  this->own_mac_address = (mac_address_t) own_mac_address;
  this->ns3_own_mac_address = get_ns3_mac(this->own_mac_address);
  this->max_packets_per_second = SUPERFRAME_DURATION / this->supeframe_num_slots;

  NS_LOG_DEBUG("I am MAC");

  //Initialize the random generator for this node
  //srand(clock_get_time_ns());
  srand(clock_get_realtime_ns() + (this->own_mac_address & 0xFF)); //TODO clock time might be the same for all the nodes, own_mac_address is unique

  NS_LOG_DEBUG("Our own MAC must be " << this->own_mac_address);

  //Calculate min MCS based on the channel width setting passed during initialization
	switch(this->channel_step){

	case 5000000:
		this->min_mcs=0;
		break;
	case 3000000:
		this->min_mcs=3;
		break;
	case 1400000:
		this->min_mcs=7;
		break;
	default:
		break;
	}

	this->min_mcs=20; //TODO hardcoded here

  NS_LOG_DEBUG("Set min MCS to " << (uint16_t)this->min_mcs << " based on channel width of " << this->channel_step << ".");

  superframe = CreateObject<TDMA_table> ();
  superframe->TDMA_table_init(this->own_mac_address, this->max_channel+1, this->supeframe_num_slots, SUPERFRAME_DURATION, this->min_mcs);
	superframe->SetMac(this);

  ack.set_ack_max_timeout(SUPERFRAME_DURATION/supeframe_num_slots + ACK_SLOT_GUARD_SPACE);
  //ack.set_control_ack_max_timeout(ACK_CONTROL_MAX_TIMEOUT);
  ack.set_control_ack_max_timeout((SUPERFRAME_DURATION*1000000UL)/cb_slots_count + ack.get_ack_max_timeout());
  ack.set_active_bw_index(get_active_channel_bandwitdh_index());
  //set_process_high_priority();

  mac_address_t test_remote;
  if(this->own_mac_address == TEST_OWN_MAC_ADDRESS)
  	test_remote = TEST_REMOTE_MAC_ADDRESS;
  else
		test_remote = TEST_OWN_MAC_ADDRESS;

  NS_LOG_DEBUG("Test remote mac address is: " << test_remote);

  //TODO init table with proper remote mac addresses, so the connection can be established
	if(superframe->table_create_static(this->own_mac_address, test_remote, cb_slots_count) != TDMA_table::SUCCESS) {
		NS_LOG_ERROR("Something went wrong during creating static superframe.");
	}
	else {
		//Table was created ok, inform now AI about the Broadcast slots set
		TDMA_send_SET_BC_slot_allocation_to_AI(superframe->get_broadcast_slots());
	}


  superframe->table_align();
  m_controller->MFTDMAControllerInit(5, 10, log); //in ms for now0
  m_controller->MFTDMAControllerStart(1, 1, 1, 1); //in ms for now
  m_controller->MFTDMAControllerSetNeighbors(m_channel->GetNeighbors()); //in ms for now0

  //Populate the MCS-TX gain table for broadcast packets
  initialize_dest_mcs_tupple(BROADCAST_MAC_ADDRESS, BROADCAST_DEFAULT_TX_GAIN , this->min_mcs);

  NS_LOG_DEBUG("MAC init done with TX gain " << this->init_tx_gain << "dB");
}

void
MFTDMAMac::Send (Ptr<Packet> p, uint16_t protocol, Mac48Address to, Mac48Address from)
{
  NS_LOG_FUNCTION (this << p << protocol << to << from);

	type_of_message type = APP;

	EthernetHeader header;
  (void)p->PeekHeader(header);

	if(protocol == ETHERTYPE_ARP || protocol == ETHERTYPE_REVARP)
  {
		NS_LOG_UNCOND("ARP request or reply!");
		type = APP_ARP;
  }

	/*if(header.GetLengthType() == ETHERTYPE_ARP || header.GetLengthType() == ETHERTYPE_REVARP)
	{
		NS_LOG_UNCOND("ARP request or reply!");
		type = APP_ARP;
	}*/

	//Discard packets that are pure APP and if queue is full for this slot
	if((type == APP) && queue_is_blocked){
		mac_stats.packets_discarded_delayed++;
		NS_LOG_LOGIC("Dropped packet: queue blocked.");
		this->drop_count++;
		this->entered_count++;
		return;
	}
	else if(type == APP)
		this->entered_count++;

#ifdef TDMA_USE_ACK
				TDMA_MAC_send(p, type, protocol, to, from, true);
#else
				TDMA_MAC_send(p, type, protocol, to, from, false);
#endif
}

void
MFTDMAMac::Receive (Ptr<Packet> packet, uint16_t protocol,
                          Mac48Address to, Mac48Address from)
{
  NS_LOG_FUNCTION (this << packet << protocol << to << from);

	mac_stats.packets_phy_received++;
	NS_LOG_DEBUG("Count of data packets received: " << mac_stats.packets_phy_received);

	MFTDMAHeader mftdma_header;
  (void)packet->PeekHeader(mftdma_header);
	mac_header header = mftdma_header.MFTDMAGetHeader();

	//fragment_index_t fragment_index(header.fragment_index);
	//fragment_index_max_t fragment_index_max(header.fragment_index_max);
	mac_index_t mac_packet_index(header.mac_index);
	mac_address_t source_address(header.source_mac_address), dest_address(header.destination_mac_address);
	type_of_message mes_type(header.mes_type);

#ifdef TDMA_USE_ACK
		bool need_ack(header.need_ack);
#endif //#TDMA_USE_ACK

		//Unpad and check CRC of the app packet
		uint8_t unpad_result = Packet_unpad(packet);
		if(unpad_result==2){
			mac_stats.packets_received_crc_error++;
			NS_LOG_ERROR("Dropped packet: crc.");
			//Discard the packet , do not forward data to app
			return;
		}
		if(unpad_result==1){
			mac_stats.packets_received_wrong_MAC_header++;
			NS_LOG_ERROR("Dropped packet: wrong mac header.");
			//Discard the packet , do not forward data to app
			return;
		}

		if(own_mac_address!=0 && !(dest_address==own_mac_address || ((dest_address == BROADCAST_MAC_ADDRESS || (dest_address >= MULTICAST_MAC_START && dest_address <= MULTICAST_MAC_END)) && source_address!=own_mac_address))){
			//discard the packet
			NS_LOG_LOGIC("Dropped packet: not for us.");
			return;
		}

#ifdef TDMA_USE_ACK
		//Checking ACK or not
		if (mes_type == MAC_ACK){
			NS_LOG_DEBUG("ACK received with -> Source: " << source_address << ", OWN: " << own_mac_address << ", Destination: " << dest_address);
			//Handling ACKs received
			TDMA_handle_received_ack(packet);
			return; //Because we do not want to send an ACK for an ACK!
		}

		//Checking control ACK or not
		if (mes_type == MAC_CONTROL_ACK){
			NS_LOG_DEBUG("Control ACK received with -> Source: " << source_address << ", OWN: " << own_mac_address << ", Destination: " << dest_address);
			//Handling control ACKs received
			TDMA_handle_received_control_ack(packet);
			return; //Because we do not want to send an ACK for an ACK!
		}

#endif

#ifdef TDMA_USE_ACK
		//If ACK is needed
		if (need_ack){
			if(mes_type == APP)
			{
				//Aggregate ACK into buffer per destination_address
				TDMA_aggregate_ack_into_buffer(source_address, //Because is the APP packet sender
										  mac_packet_index,
										  1,
										  1,
										  0,
										  0,
										  clock_get_time_ns());
			}
			else
			{
				TDMA_prepare_and_send_control_ack(source_address, mac_packet_index, 1, 0, 0);
			}
		}
#endif

	uint32_t data_size = packet->GetSize ();
	packet->CopyData(rx_tx_buffer, data_size);

	switch(mes_type)
	{
	case MAC_FDMA_CHANNEL_BROADCAST:
		break;

	case MAC_TDMA_ADD_SLOT:
		{
    	//(void)packet->CopyData(rx_tx_buffer, 2000);
			add_slot_payload* payload_p = (add_slot_payload*) rx_tx_buffer;
			NS_LOG_INFO("Got Command to establish new RX slot with node " << payload_p->initiator << ", number of suggested slots is " << (uint16_t)payload_p->number_of_suggested_slots );
			TDMA_send_GET_selected_slot_to_AI(payload_p->initiator,payload_p->number_of_suggested_slots, payload_p->slot_table);
		}
		break;

	case MAC_TDMA_MOVE_SLOT:
		{
  		//(void)packet->CopyData(rx_tx_buffer, 2000);
			slot_move_payload* payload_p = (slot_move_payload*) rx_tx_buffer;
			NS_LOG_INFO("Got Command to move RX slot from node " << payload_p->initiator << ", number of suggested slots is " << (uint16_t)payload_p->number_of_suggested_slots );
			TDMA_send_GET_selected_move_slot_to_AI(payload_p->initiator, payload_p->number_of_suggested_slots, payload_p->old_slot, payload_p->slot_table);
		}
		break;

	case MAC_TDMA_DELETE_SLOT:
		{
  		//(void)packet->CopyData(rx_tx_buffer, 2000);
			del_slot_payload* payload_p = (del_slot_payload*) rx_tx_buffer;
			NS_LOG_INFO("Got Command to delete slot with node " << payload_p->initiator << ", slot is " << (uint16_t)payload_p->del_slot.channel << ", " << (uint16_t)payload_p->del_slot.timeslot);

			TDMA_send_SET_Slot_removal_to_AI(payload_p->initiator, payload_p->del_slot.channel, payload_p->del_slot.timeslot);
		}
		break;
	case MAC_TDMA_ADVERTISE_SLOT_TABLE:
		{
			//(void)packet->CopyData(rx_tx_buffer, 2000);
			slot_table_advertisement_payload* payload_p = (slot_table_advertisement_payload*) rx_tx_buffer;
			NS_LOG_INFO("Got Advertise Used Slots from node " << source_address << ", slot count is " << (uint16_t)payload_p->number_of_used_slots);

#ifdef USE_SLOTTED_ALOHA
			superframe->ntable->neighbour_table_manual_update(payload_p->table_owner);
#endif

			for(uint8_t i=0; i<payload_p->number_of_used_slots; i++)
#ifndef SLOT_ALLOCATION_EXPOSED_NODE
				TDMA_send_SET_external_slot_allocation_to_AI(  payload_p->table_owner, 0, payload_p->list_slots[i].channel , payload_p->list_slots[i].timeslot);
#else
				TDMA_send_SET_external_slot_allocation_to_AI(  payload_p->table_owner, 0, payload_p->list_slots[i].channel , payload_p->list_slots[i].timeslot, payload_p->list_slots_mode[i]);
#endif
		}
		break;

	case MAC_TDMA_SLOT_TRANSACTION_RESULT:
		{
			//(void)packet->CopyData(rx_tx_buffer, 2000);
			slot_transaction_result_payload* payload_p = (slot_transaction_result_payload*) rx_tx_buffer;
			if(payload_p->result==SUCCESS){
				NS_LOG_INFO("Slot protocol transaction result: SUCCESS received from " << source_address << ", transaction " << payload_p->action << ", relavent slot channel " << (uint16_t)payload_p->relevant_slot.channel << ", timeslot " << (uint16_t)payload_p->relevant_slot.timeslot);
				if(payload_p->action==add_slot){
					TDMA_send_SET_slot_allocation_to_AI(source_address,SLOT_MODE::TX,payload_p->relevant_slot.channel,payload_p->relevant_slot.timeslot);
				}
				else if(payload_p->action==delete_slot){
					NS_LOG_INFO("Slot protocol transaction result: SUCCESS received from " << source_address << ", transaction " << payload_p->action << ", relavent slot channel  " << (uint16_t)payload_p->relevant_slot.channel << ", timeslot " << (uint16_t)payload_p->relevant_slot.timeslot);//TDMA_send_SET_delete_slot

					//And we send the protocol ACK packet immediately here
					slot_reference temp_slot;
					temp_slot.channel=payload_p->relevant_slot.channel;
					temp_slot.timeslot=payload_p->relevant_slot.timeslot;

					//After every set, all nodes should send a broadcast protocol ACK packet to announce the new slot to third party listeners
#ifndef SLOT_ALLOCATION_EXPOSED_NODE
					TDMA_send_transaction_ack_packet(source_address, delete_slot,temp_slot);
#else
					TDMA_send_transaction_ack_packet(source_address, delete_slot,temp_slot, SLOT_MODE::SLOT_MODE_UNKNOWN);
#endif
				}
				else if(payload_p->action==move_slot)
					TDMA_send_SET_slot_move_to_AI(source_address,SLOT_MODE::TX, payload_p->relevant_slot.channel, payload_p->relevant_slot.timeslot,
													payload_p->new_slot.channel, payload_p->new_slot.timeslot);
			}
			//FAILE TRANSACTION, TODO need to define here what to do for failed responses
			else
			{
				NS_LOG_INFO("New slot channel  " << (uint16_t)payload_p->new_slot.channel << ", timeslot " << (uint16_t)payload_p->new_slot.timeslot);
				NS_LOG_INFO("Slot protocol transaction result: FAIL received from " << source_address << ", transaction " << payload_p->action << ", relevant slot channel " << (uint16_t)payload_p->relevant_slot.channel << ", timeslot " << (uint16_t)payload_p->relevant_slot.timeslot);
			}
		}
		break;

	case MAC_TDMA_SLOT_TRANSACTION_ACK:
		{
			//(void)packet->CopyData(rx_tx_buffer, 2000);

			//Received a final Protocol ACK for a slot allocation transactions, parse it and send it to AI, WITH RX as mode
			slot_transaction_ack_payload* payload_p = (slot_transaction_ack_payload*) rx_tx_buffer;

			NS_LOG_INFO("Slot protocol ACK received from " << source_address << ", remote relevant node is " << payload_p->remote << ", transaction " << payload_p->action << ", relevant slot channel " << (uint16_t)payload_p->relevant_slot.channel << ", timeslot " << (uint16_t)payload_p->relevant_slot.timeslot);

#ifdef USE_SLOTTED_ALOHA
#ifdef ENABLE_IMPROVED_NEIGHBOR_REPORT
			superframe->ntable->neighbour_table_manual_update(payload_p->initiator);
#endif
#endif

			//See if this is for us, if not send it to AI to be added as a used slot
			if(payload_p->initiator!=this->own_mac_address && payload_p->remote!=this->own_mac_address){

				NS_LOG_INFO("Slot in ACK doesn't include us, send it to AI to add it in our table as used");

				if(payload_p->action == slot_action::add_slot)
#ifndef SLOT_ALLOCATION_EXPOSED_NODE
					TDMA_send_SET_external_slot_allocation_to_AI( payload_p->initiator, payload_p->remote , payload_p->relevant_slot.channel, payload_p->relevant_slot.timeslot);
#else
					TDMA_send_SET_external_slot_allocation_to_AI( payload_p->initiator, payload_p->remote , payload_p->relevant_slot.channel, payload_p->relevant_slot.timeslot, payload_p->mode);
#endif
				else if(payload_p->action == delete_slot)
#ifndef SLOT_ALLOCATION_EXPOSED_NODE
					TDMA_send_SET_external_slot_removal_to_AI(payload_p->initiator,payload_p->remote, payload_p->relevant_slot.channel, payload_p->relevant_slot.timeslot);
#else
					TDMA_send_SET_external_slot_removal_to_AI(payload_p->initiator,payload_p->remote, payload_p->relevant_slot.channel, payload_p->relevant_slot.timeslot, payload_p->mode);
#endif
				else
				{
					NS_LOG_INFO("New slot channel " << (uint16_t)payload_p->new_slot.channel << ", timeslot " << (uint16_t)payload_p->new_slot.timeslot);
					TDMA_send_SET_external_slot_move_to_AI( payload_p->initiator,payload_p->remote, payload_p->relevant_slot.channel, payload_p->relevant_slot.timeslot,
															payload_p->new_slot.channel, payload_p->new_slot.timeslot);
				}
			}
		}
		break;

	case APP_ARP:
	case AI_MESS:
	case APP:
		{
			//Check if the same packet is already received. Drop if it is or add to circular buffer if not.
			/*if(received_app_packet_in_list(source_address, mac_packet_index))
			{
				NS_LOG_ERROR("DUPLICATE PACKET: mac_address " << source_address << ", mac_index " << mac_packet_index);
				break;
			}*/

		  m_rxCallback(packet, protocol, to, from);

			mac_stats.packets_sent_to_app++;
			NS_LOG_DEBUG("Count of data packets sent to APP: " << mac_stats.packets_sent_to_app);
			break;
		}
		default:
			break;
	}

}

void
MFTDMAMac::SetForwardUpCallback (Callback< void, Ptr<Packet>, uint16_t, Mac48Address, Mac48Address > upCallback)
{
	NS_LOG_FUNCTION (this);
	m_rxCallback = upCallback;
}

Ptr<TDMA_table>
MFTDMAMac::GetTable (void) const
{
  NS_LOG_FUNCTION (this);
  return superframe;
}

void
MFTDMAMac::SetTable (Ptr<TDMA_table> table)
{
  NS_LOG_FUNCTION (this << table);
	superframe = table;
}

Ptr<MFTDMAMacNetDevice>
MFTDMAMac::GetDevice (void) const
{
  NS_LOG_FUNCTION (this);
  return m_device;
}

void
MFTDMAMac::SetDevice (Ptr<MFTDMAMacNetDevice> device)
{
  NS_LOG_FUNCTION (this << device);
  m_device = device;
}

Ptr<DarpaWirelessChannel>
MFTDMAMac::GetChannel (void) const
{
	NS_LOG_FUNCTION (this);
	return m_channel;
}

void
MFTDMAMac::SetChannel (Ptr<DarpaWirelessChannel> channel)
{
	NS_LOG_FUNCTION (this << channel);
	m_channel = channel;
}

Ptr<MFTDMAController>
MFTDMAMac::GetController (void) const
{
	NS_LOG_FUNCTION (this);
	return m_controller;
}

void
MFTDMAMac::SetController (Ptr<MFTDMAController> controller)
{
	NS_LOG_FUNCTION (this << controller);
	m_controller = controller;
}

void
MFTDMAMac::SetRxChannel(channel_t rx_channel)
{
	//NS_LOG_FUNCTION (this << (uint16_t)rx_channel);
	this->rx_channel = rx_channel;
}

channel_t
MFTDMAMac::GetRxChannel(void)
{
	//NS_LOG_FUNCTION (this);
	return rx_channel;
}

void
MFTDMAMac::TDMA_MAC_send(Ptr<Packet> packet_data, type_of_message message_type, uint16_t protocol, Mac48Address to, Mac48Address from, bool need_ack)
{
  mac_header header;
  header.mac_index = mac_index++;
  header.mes_type = message_type;
  header.need_ack = need_ack;
  header.mac_crc = 0;
  //to.CopyTo ((uint8_t*)&header.destination_mac_address);
  //from.CopyTo ((uint8_t*)&header.source_mac_address);

  uint8_t temp[6];
  to.CopyTo(temp);
  header.destination_mac_address = get_mac_address_from_array(temp);
  from.CopyTo(temp);
  header.source_mac_address = get_mac_address_from_array(temp);

  NS_LOG_DEBUG("destination_mac_address " << header.destination_mac_address << " source_mac_address " << header.source_mac_address);

  udp_port_t port = protocol;
  std::vector<uint32_t> payload_size(1);

#ifdef USE_QUEUES
  TDMA_queue::tdma_queue_packet_type packet_type;
#endif
  switch(message_type){
  	case APP:

  	if(header.destination_mac_address == BROADCAST_MAC_ADDRESS)
		{
  		    header.need_ack = false;
#ifdef USE_QUEUES
			packet_type = TDMA_queue::BROADCAST;
#endif
		}
		else
		{
#ifdef USE_QUEUES
			packet_type = TDMA_queue::APP_LOW_PRIORITY;
			//get_dest_udp_port(packet_data, port);
#endif
		}
  		break;

  	case APP_ARP:
	    header.need_ack = false;
#ifdef USE_QUEUES
			packet_type = TDMA_queue::BROADCAST;
#endif
  		break;
  	case AI_MESS:
	    header.need_ack = false;
#ifdef USE_QUEUES
			packet_type = TDMA_queue::AI;
#endif
  		break;
  	default:
#ifdef USE_QUEUES
		packet_type = TDMA_queue::CONTROL;
#endif
  		break;
  }

#ifdef USE_QUEUES
if(packet_type == TDMA_queue::APP_LOW_PRIORITY)
{
	previous_packets_per_second = packets_per_second;
	packets_per_second += 1;
	if(packets_per_second >= max_packets_per_second)
	{
		packets_per_second = 0;
		block_low_queue();
 	}
}
#endif

	header.fragment_index = 1;
	header.fragment_index_max = 1;
	//header.source_mac_address = own_mac_address;


	/*uint32_t mcs_val = get_mcs_based_on_destination(header.destination_mac_address, this->min_mcs);

	uint16_t slot_size = get_bytes_in_slot(get_active_channel_bandwitdh_index(), mcs_val);
	payload_size.push_back(slot_size);

	//Read the destination MAC and add it to the Dest-MCS map for now with DEFAULT_MCS
	initialize_dest_mcs_tupple(header.destination_mac_address, this->init_tx_gain, this->min_mcs);

	uint32_t tx_gain = get_txgain_based_on_destination(header.destination_mac_address, this->init_tx_gain);*/

	uint32_t mcs_val = 0;
	uint16_t slot_size = 0;
	payload_size[0] = slot_size;
	uint32_t tx_gain = 0;

	Packet_pad(packet_data, slot_size, header);

	/*ack.add_ack_needed_in_map(header.mac_index,
							  header.fragment_index,
							  header.destination_mac_address,
								mcs_val,
								tx_gain,
								port,
								payload_size[0],
							  0,
							  0,
							  (uint8_t)TDMA_slot::UNICAST_SEND,
								TDMA_table::LOW_PRIORITY,
								clock_get_time_ns(),
							  RETRY_COUNTER_MAX,
								packet_data);

	//Increment ACK statistics
	ack.increment_number_of_sent_packets(header.destination_mac_address,
										 1,
										 clock_get_time_ns()); //header.destination_mac_address because it's the destination of the APP packet

  m_channel->Send(packet_data, protocol, to, from, this);*/

#ifdef USE_QUEUES
  //std::vector<Ptr<Packet>> packet_data_vector(1);
  packet_data_vector[0] = packet_data;
  //packet_data_vector.push_back(packet_data);
  TDMA_table::table_operation_status status = superframe->table_add_packets(packet_data_vector, mcs_val, tx_gain, port, payload_size, packet_type, header.destination_mac_address);
#endif

  if(status == TDMA_table::SUCCESS)
  {
      //Update statistics about App packets sent to PHY
      mac_stats.packets_sent_from_application++;
      NS_LOG_INFO("Data packets sent to PHY: " << mac_stats.packets_sent_from_application);
  }
  else
  {
  	mac_stats.packets_discarded_no_space++;
  	NS_LOG_INFO("Packet discarded, total packets discarded because of lack of space in superframe " << mac_stats.packets_discarded_no_space << ", total packets sent " << mac_stats.packets_sent_from_application);
  }

  //Check if slot to sent to target exists, else initiate slot allocation procedure
#ifdef USE_QUEUES
  if(packet_type!=TDMA_queue::AI && packet_type!= TDMA_queue::BROADCAST && packet_type!= TDMA_queue::CONTROL && !superframe->slot_exists_to(header.destination_mac_address)){

  	if(this->final_slot_allocation_in_progress==false){
#ifndef STATIC_TABLE_TYPE
  			NS_LOG_INFO("We are NOT locked , flag is " << this->final_slot_allocation_in_progress << ", going to allocate new slot");
      	TDMA_initiate_allocate_new_slot(header.destination_mac_address);
#endif
     	}else
     		NS_LOG_INFO("We are LOCKED , flag is " << this->final_slot_allocation_in_progress << ", NOT going to allocate new slot");

  }
#endif
}

void MFTDMAMac::process_other_MAC(void)
{
    //Getting current timestamp
    timestamp_t current_timestamp = clock_get_time_ns();

#ifdef TDMA_USE_ACK
    //Checking if the ACKs Buffers has timed out
    /*std::list<mac_address_t>* acks_destinations_timeout_p = ack.get_ack_buffer_destinations_timeout_list(current_timestamp);
    if (acks_destinations_timeout_p->size()){
        for (auto it : *acks_destinations_timeout_p) {
        		NS_LOG_DEBUG("ACK Buffer timeout for destination " << it);
            //Send all ACKs for a given destination address
            TDMA_prepare_and_send_ack(it);
        }
    }
    //Clearing the list
	delete acks_destinations_timeout_p;*/

#ifdef TDMA_RETRANSMISSIONS
    //Check if the ACKs has timed out and update the statistics
    std::list<Acknowledgment::retransmission_info>* retr_list_p = ack.check_ack_timeout(current_timestamp);

    if (retr_list_p->size())
    {
    		TDMA_table::table_operation_status status;
    		NS_LOG_DEBUG("Start of packets retransmission, size " << retr_list_p->size());
        for(auto it : *retr_list_p){
#ifdef USE_QUEUES
        	if(it.slot_type != TDMA_slot::CONTROL_BROADCAST)
        	{
        		status = superframe->table_add_packet(it.packet, it.mcs, it.tx_gain, it.port, it.payload_size, TDMA_queue::RETRANSMISSION, it.destination_mac_address);
        		retr_data_count++;
        	}
        	else
        	{
        		status = superframe->table_add_packet(it.packet, it.mcs, it.tx_gain, it.port, it.payload_size, TDMA_queue::CONTROL, it.destination_mac_address);
        		retr_control_count++;
        	}
#endif
        	if(status != TDMA_table::SUCCESS)
            {
                ack.update_ack_needed_in_map(it.mac_index, it.fragment_index, false, current_timestamp);
                NS_LOG_DEBUG("Retransmission packet dropped, no space in superframe for destination with mac address " << it.destination_mac_address);
            }
        }
    }
#endif

    retr_list_p->clear();
    delete retr_list_p;

    //Send ACK Stats to the AI
    std::list<Acknowledgment::ack_statistics>* ack_stats_list_p = ack.get_ack_stats_list(); //SHOULD NOT BE DELETED, NOT DYNAMICALLY ALLOCATED
    if(!ack_stats_list_p->empty()){
    	for (auto it : *ack_stats_list_p) {
    		TDMA_send_ack_stats_to_AI(it);
    	}
    	ack_stats_list_p->clear();
    }

    //Iterating over the moving averages and removing whose are expired by a defined threshold!
    ack.check_ack_moving_samples_timeout(current_timestamp);
#endif

#ifdef USE_QUEUES
    if(last_queues_check_timeout + QUEUES_CHECK_TIMEOUT_PERIOD <= current_timestamp)
    {
    	last_queues_check_timeout = current_timestamp;

		std::map<mac_address_t,uint16_t>* delayed_mac_address_set = superframe->table_queue_check_timeout(current_timestamp);
		//std::map<mac_address_t,uint16_t>* delayed_mac_address_set = new std::map<mac_address_t,uint16_t>;
		//Allocate new slots if we lose packets for a specific target (loadbalancing so we don't allocate more than 4 per node, when in 5MHz Channel width)

		if(!delayed_mac_address_set->empty()){

			//Try to delay adding new slots after a slot procedure has started. Keep a timestamp here and use it as a timer
			if(slot_delay_calculated == false)
			{
				slot_delay = PSEUDO_RANDOM_SLOT_ALLOCATION_DELAY;
#ifndef SLOT_ALLOCATION_UPGRADE
				slot_delay_calculated = false;
#else
				slot_delay_calculated = true;
#endif
			}

			if(last_add_slot_timestamp + slot_delay <= current_timestamp){
				slot_delay_calculated = false;
				NS_LOG_INFO(" PASSED THE FILTER of DELAY, time now is " << current_timestamp << ", last timestamp " << last_add_slot_timestamp << ", last_timestamp + random delay is " << slot_delay);
				last_add_slot_timestamp = current_timestamp;

				//Now we can release the pending new slot allocation requests to different MAC destinations
				for (auto itera : *delayed_mac_address_set) {
					if(!(this->max_bandwidth==6000000 && superframe->get_active_send_slots_to(itera.first)>get_limit_of_active_slots(this->channel_step))){
#ifndef STATIC_TABLE_TYPE
							NS_LOG_DEBUG("STARTING NEW SLOT cause of dropped packets, mac address " << itera.first << ", packets deleted " << itera.second << ", active slots " << (uint16_t)superframe->get_active_send_slots());
							TDMA_initiate_allocate_new_slot(itera.first);
#endif
					}
				}
			}
		}
		delete delayed_mac_address_set;
    }

#ifndef STATIC_TABLE_TYPE
    //Checking if i need to broadcast my slot table usage for TX and RX
    if(br_delay_calculated == false)
  	{
    	br_delay = PSEUDO_RANDOM_TABLE_BROADCAST_DELAY;
#ifndef SLOT_ALLOCATION_UPGRADE
    	br_delay_calculated = false;
#else
    	br_delay_calculated = true;
#endif
  	}
    if(last_table_usage_broadcast_timestamp + br_delay <= current_timestamp){
    			br_delay_calculated = false;
    	    TDMA_send_slot_table_usage_packet();
    	    last_table_usage_broadcast_timestamp = current_timestamp;
    	    NS_LOG_INFO(" Sending out a FULL SLOT USAGE message , node " << this->own_mac_address);
    }
#endif

#endif

#ifdef USE_SLOTTED_ALOHA
#ifndef STATIC_TABLE_TYPE
    if(last_neighbour_table_update + NEIGHBOUR_TABLE_UPDATE_TIMEOUT <= current_timestamp)
    {
    	superframe->ntable->neighbour_table_timeout_update();
    	last_neighbour_table_update = current_timestamp;
    }
#endif
#endif

    return;
	//TODO Modify to use timer or another thread instead of this process
}

void MFTDMAMac::process_AI_message(Internal &message)
{
	MESSAGE_ATTRIBUTE type = message.attribute;

	switch(type)
	{
		case MESSAGE_GETR:
			NS_LOG_LOGIC("Get_r response");

			//SWITCH BETWEEN PHY STAT AND MAC STAT
			switch(message.getr.attribute)
			{
				case PROPOSED_SLOTS:
					{
					//NS_LOG_INFO("We have proposed slots " << imes->getr().proposed_slots().DebugString());

					Proposed_Slots slot_prop_p = message.getr.proposed_slots;

					slot_reference slots[MAX_NUMBER_PROPOSED_SLOTS];

					if(slot_prop_p.slots.size()>MAX_NUMBER_PROPOSED_SLOTS){
						NS_LOG_ERROR("Returned proposed slots more than MAX defined, ERROR");
						break;
					}
					if(slot_prop_p.slots.size()==0){
						NS_LOG_INFO("AI didn't find any slot suitable for the remote MAC " << slot_prop_p.mac << ", terminating slot allocation procedure");
						break;
					}

					for (int i = 0; i < slot_prop_p.slots.size(); i++) {
						slots[i].channel =(uint8_t) slot_prop_p.slots[i].frequency_num;
						slots[i].timeslot = (uint8_t) slot_prop_p.slots[i].timeslot_num;
					}

					int8_t number_proposed_slots = slot_prop_p.slots.size();

					TDMA_send_proposed_slots_packet(slot_prop_p.mac,number_proposed_slots, slots);
					}
					break;
				case SELECTED_SLOT:
				{
					//NS_LOG_INFO("We have selected slot " << imes->getr().selected_slot().DebugString());

					Selected_Slot selected_slot_message =  message.getr.selected_slot;

					MFTDMA_Slot select_slot_p = message.getr.selected_slot.slot;

					//Check if a free valid slot was selected, and act accordingly
					if(select_slot_p.frequency_num==255 || select_slot_p.timeslot_num==255 ){
						//Slot doesn't exist, inform the other party and cancel this procedure
						NS_LOG_INFO("Got slot 255-255 as answer, no free slot exist, terminating slot allocation procedure");
						//Inform the other party about the selected slot
						// NOT FOR NOW TDMA_send_selected_slot_packet((uint64_t)selected_slot_message->mac(),(uint32_t) select_slot_p->frequency_num(),(uint32_t)select_slot_p->timeslot_num() );
						break;
					}
					//Inform the other party about the selected slot
					TDMA_send_selected_slot_packet((uint64_t)selected_slot_message.mac, add_slot, (uint32_t) select_slot_p.frequency_num,(uint32_t)select_slot_p.timeslot_num);

					//Inform AI to insert this final slot in the TDMA table
					TDMA_send_SET_slot_allocation_to_AI((uint64_t)selected_slot_message.mac,SLOT_MODE::RX,(uint32_t) select_slot_p.frequency_num,(uint32_t)select_slot_p.timeslot_num);
					}
					break;

				case SELECTED_MOVE_SLOT:
				{
					//NS_LOG_INFO("We have selected move slot " << imes->getr().selected_move_slot().DebugString());

					Selected_Move_Slot selected_move_slot_message =  message.getr.selected_move_slot;

					MFTDMA_Slot select_old_slot_p = selected_move_slot_message.old_slot;
					MFTDMA_Slot select_new_slot_p = selected_move_slot_message.new_slot;

					//Check if a free valid slot was selected, and act accordingly
					if(select_new_slot_p.frequency_num==255 || select_new_slot_p.timeslot_num==255 ){
						//Slot doesn't exist, inform the other party and cancel this procedure
						NS_LOG_INFO("Got slot 255-255 as answer, no free slot exist, terminating slot move procedure");
						//Inform the other party about the selected slot
						// NOT FOR NOW TDMA_send_selected_slot_packet((uint64_t)selected_slot_message->mac(),(uint32_t) select_slot_p->frequency_num(),(uint32_t)select_slot_p->timeslot_num() );
						break;
					}
					//Inform the other party about the selected move slot
					TDMA_send_selected_slot_packet((uint64_t)selected_move_slot_message.mac, move_slot, (uint32_t) select_old_slot_p.frequency_num,(uint32_t)select_old_slot_p.timeslot_num,
												   (uint32_t) select_new_slot_p.frequency_num,(uint32_t)select_new_slot_p.timeslot_num);
					//Inform AI to move slot in the TDMA table
					TDMA_send_SET_slot_move_to_AI((uint64_t)selected_move_slot_message.mac,SLOT_MODE::RX, (uint32_t) select_old_slot_p.frequency_num,(uint32_t)select_old_slot_p.timeslot_num,
							   	   	   	   	   	   	   	   	   	   	  (uint32_t) select_new_slot_p.frequency_num,(uint32_t)select_new_slot_p.timeslot_num);
				}
				break;

				default:
					NS_LOG_WARN("There was no predefined payload");
			}
		break;

		case MESSAGE_SET:
      NS_LOG_LOGIC("Set command");

      switch(message.set.attribute)
      {
          case SLOT_REMOVAL:
					{
						//NS_LOG_INFO("We have slot removal message from AI " << imes->set().slot_removal().DebugString());

						//Get access to the slot info and the relevant remote mac address
						Slot_Removal slot_rem_p = message.set.slot_removal;
						MFTDMA_Slot slot_p = slot_rem_p.slot;

						//We send a remove packet to the remote station to remove it as well
						TDMA_send_remove_slot_packet((mac_address_t) slot_rem_p.mac, (uint8_t) slot_p.frequency_num, (uint8_t)slot_p.timeslot_num);

						//TODO check what we need to check if the slot response is ok.
						TRANSACTION_RESULT trans_result;
#ifdef SLOT_ALLOCATION_EXPOSED_NODE
						SLOT_MODE slot_mode = SLOT_MODE::SLOT_MODE_UNKNOWN;
#endif
						//Delete the slot if its in the table
						if(superframe->check_specific_slot_exists_to((mac_address_t) slot_rem_p.mac, (uint8_t) slot_p.frequency_num, (uint8_t)slot_p.timeslot_num))
						{
							superframe->remove_slot((uint8_t) slot_p.frequency_num, (uint8_t)slot_p.timeslot_num);
#ifdef SLOT_ALLOCATION_EXPOSED_NODE
							slot_mode = SLOT_MODE::TX;
#endif
						}
						else if(superframe->check_specific_slot_exists_from((mac_address_t) slot_rem_p.mac, (uint8_t) slot_p.frequency_num, (uint8_t)slot_p.timeslot_num))
						{
							superframe->remove_slot((uint8_t) slot_p.frequency_num, (uint8_t)slot_p.timeslot_num);
#ifdef SLOT_ALLOCATION_EXPOSED_NODE
							slot_mode = SLOT_MODE::RX;
#endif
						}
						else
							NS_LOG_ERROR("ERROR: Slot to be removed, not allocated in our table");
							//NS_LOG_ERROR("ERROR: Slot to be removed, not allocated in our table, " << imes->set().slot_removal().DebugString());

						//Anyway the slot is now not present in the table
						trans_result = TRANSACTION_RESULT::OK;

						//We send the protocol ACK broadcast packet as well to announce to our neighbors about the deletion
						slot_reference temp_slot;
						temp_slot.channel=(uint8_t) slot_p.frequency_num;
						temp_slot.timeslot=(uint8_t)slot_p.timeslot_num;

						//After every set slot removal, the node should send a broadcast protocol ACK packet to announce the deleted slot to third party listener
#ifndef SLOT_ALLOCATION_EXPOSED_NODE
						TDMA_send_transaction_ack_packet((mac_address_t) slot_rem_p.mac, delete_slot,temp_slot);
#else
						TDMA_send_transaction_ack_packet((mac_address_t) slot_rem_p.mac, delete_slot,temp_slot, slot_mode);
#endif

						//Create a set_r message to respond back to AI
						Internal setr_mes;
						setr_mes.module = MODULE_AI;
						setr_mes.attribute = MESSAGE_SETR;
						setr_mes.setr.result = trans_result;

						//Send message back to Sender AI
						setr_mes.transaction_index = m_device->MacAIMessageIndexUpdate(message.transaction_index);
						m_device->SendMacAIMessage(setr_mes);
					}
					break;

          case SELECTED_MOVE_SLOT:
          {
						//NS_LOG_INFO("We have slot move message from AI " << imes->set().proposed_move_slots().DebugString());

						Proposed_Move_Slots slot_move_prop_p = message.set.proposed_move_slots;

						MFTDMA_Slot old_slot_p = slot_move_prop_p.old_slot;

						slot_reference old_slot;
						slot_reference slots[MAX_NUMBER_PROPOSED_SLOTS];

						if(slot_move_prop_p.slots.size()>MAX_NUMBER_PROPOSED_SLOTS){
							NS_LOG_ERROR("Returned proposed slots more than MAX defined, ERROR");
							break;
						}
						if(slot_move_prop_p.slots.size()==0){
							NS_LOG_INFO("AI didn't find any slot suitable for the remote MAC " << slot_move_prop_p.mac << ", terminating slot move procedure");
							break;
						}

						old_slot.channel = old_slot_p.frequency_num;
						old_slot.timeslot = old_slot_p.timeslot_num;

						for (int i = 0; i < slot_move_prop_p.slots.size(); i++) {
							slots[i].channel =(uint8_t) slot_move_prop_p.slots[i].frequency_num;
							slots[i].timeslot = (uint8_t) slot_move_prop_p.slots[i].timeslot_num;
						}

						int8_t number_proposed_slots = slot_move_prop_p.slots.size();

						TDMA_send_proposed_move_slots_packet(slot_move_prop_p.mac, number_proposed_slots, old_slot, slots);
          }
          break;

          default:
          	NS_LOG_WARN("Unknown Set command");
          break;
      }
		break;

		case MESSAGE_SETR:
    {
        NS_LOG_LOGIC("Set_r response");
        NS_LOG_DEBUG("The transaction result of the set_r " << message.transaction_index << " command is: " << message.setr.result);

        //Todo add a way in the response to know what kind of SET this SET_R responds to.
        set_process* procedure_p = procedure_exists(message.transaction_index);

        if(procedure_p==NULL){
        	NS_LOG_ERROR("ERROR: Got a set_r response with transaction ID " << message.transaction_index << " that wasn't in the table of active procedures, impossible");
        	break;
        }
        else {
        	NS_LOG_INFO(" Treating a SET_R response with transaction ID " << message.transaction_index << " and of SET type " << procedure_p->set_type << " that was found in our tables as active");

        	/** Treat all of the above if needed
        	kMcsTxgainBw = 1,
	    		kCh = 2,
					kExternalSlotAllocation = 3,
					kExternalSlotRemoval = 4,
					kSlotAllocation = 5,
					kSlotRemoval = 6,
					kBcSlotsAnnouncement = 7,
        	 	**/
        	if(procedure_p->set_type == SLOT_ALLOCATION){

        		if(message.setr.result == TRANSACTION_RESULT::OK){
        			Slot_Allocation slot_alloc = procedure_p->message_p.set.slot_allocation;

        			NS_LOG_INFO(" MAKING FINAL SLOT ALLOCATION IN LOCAL TABLE with node " << slot_alloc.mac << ", FR " << slot_alloc.slot.frequency_num << ", TIME " << slot_alloc.slot.timeslot_num << ",  and of (RX,TX - 1,2) slot type " << procedure_p->mode << " that was found in our SET tables as active");

        			//Convert MODE  RX/TX to slot type
        			enum TDMA_slot::tdma_slot_type slot_type=TDMA_slot::UNUSED;
#ifdef SLOT_ALLOCATION_EXPOSED_NODE
        			SLOT_MODE slot_mode = SLOT_MODE::SLOT_MODE_UNKNOWN;
#endif
        			if(procedure_p->mode==SLOT_MODE::TX)
        			{
        				slot_type = TDMA_slot::UNICAST_SEND;
#ifdef SLOT_ALLOCATION_EXPOSED_NODE
        				slot_mode = SLOT_MODE::TX;
#endif
        			}
        			else if(procedure_p->mode==SLOT_MODE::RX)
        			{
        				slot_type = TDMA_slot::RECEIVE;
#ifdef SLOT_ALLOCATION_EXPOSED_NODE
        				slot_mode = SLOT_MODE::RX;
#endif
        			}
        			else
        				NS_LOG_ERROR("Got an add rx/tx slot set command with unknown mode type " << procedure_p->mode);

        			//If results is ok we add the slot to our table
        			if(!superframe->check_specific_slot_exists_from((mac_address_t) procedure_p->message_p.set.slot_allocation.mac, (uint8_t) slot_alloc.slot.frequency_num, (uint8_t)slot_alloc.slot.timeslot_num))
        				superframe->add_slot(slot_alloc.slot.frequency_num,
        									 	 	 	 	 	 slot_alloc.slot.timeslot_num,
																		 slot_type,
																		 	slot_alloc.mac);
        			else
        				NS_LOG_WARN("Tried to add rx/tx slot and it the spot wasn't free: FR " << slot_alloc.slot.frequency_num << " TIME " << slot_alloc.slot.timeslot_num);

        			//And we send the protocol ACK packet immediately here
				slot_reference temp_slot;
				temp_slot.channel=slot_alloc.slot.frequency_num;
				temp_slot.timeslot=slot_alloc.slot.timeslot_num;

				//After every set, all nodes should send a broadcast protocol ACK packet to announce the new slot to third party listeners
#ifndef SLOT_ALLOCATION_EXPOSED_NODE
				TDMA_send_transaction_ack_packet(slot_alloc.mac, add_slot,temp_slot);
#else
				TDMA_send_transaction_ack_packet(slot_alloc.mac, add_slot,temp_slot, slot_mode);
#endif
	        }
        	    else{
        			//AI said it cannot add the slot, so IF this was a TX slot, send back to the other side that we couldn't allocate the slot and that he has to remove it as well
        	    	//If it is an RX mode it is impossible as handled by AI to be altered before the transaction is done, as it is in Proposed state
        	    	Slot_Allocation slot_alloc = procedure_p->message_p.set.slot_allocation;
        	    	if(procedure_p->mode==SLOT_MODE::TX)
        	    		TDMA_send_remove_slot_packet((mac_address_t) procedure_p->message_p.set.slot_allocation.mac, (uint8_t) slot_alloc.slot.frequency_num, (uint8_t)slot_alloc.slot.timeslot_num);
        		}
        		//Release the SET-SET_R lock
        		this->final_slot_allocation_in_progress=false;
        	}
        	else if (procedure_p->set_type == SLOT_REMOVAL){

        		if(message.setr.result == TRANSACTION_RESULT::OK){
        			Slot_Removal slot_removal = procedure_p->message_p.set.slot_removal;

        			NS_LOG_INFO(" MAKING FINAL SLOT DELETION IN LOCAL TABLE with node " << slot_removal.mac << ", FR " << slot_removal.slot.frequency_num << ", TIME " << slot_removal.slot.timeslot_num << ",  and of rx/tx slot type " << procedure_p->mode << " that was found in our tables as active");

#ifdef SLOT_ALLOCATION_EXPOSED_NODE
        			SLOT_MODE slot_mode = superframe->get_slot_mode((uint8_t) slot_removal.slot.frequency_num, (uint8_t) slot_removal.slot.timeslot_num);
#endif

        			//Delete here the slot, ai is OK
				superframe->remove_slot( (uint8_t) slot_removal.slot.frequency_num, (uint8_t) slot_removal.slot.timeslot_num);

				//And we send the protocol ACK broadcast packet as well to announce to our neighbors about the deletion
				slot_reference temp_slot;
				temp_slot.channel=slot_removal.slot.frequency_num;
				temp_slot.timeslot=slot_removal.slot.timeslot_num;

				//After every set slot removal, the node should send a broadcast protocol ACK packet to announce the deleted slot to third party listener
#ifndef SLOT_ALLOCATION_EXPOSED_NODE
				TDMA_send_transaction_ack_packet(slot_removal.mac, delete_slot,temp_slot);
#else
				TDMA_send_transaction_ack_packet(slot_removal.mac, delete_slot,temp_slot, slot_mode);
#endif
        		}
        		//Else do nothing
        	}
        	else if (procedure_p->set_type == SLOT_MOVE)
        	{
        		if(message.setr.result == TRANSACTION_RESULT::OK)
        		{
        			Slot_Move slot_move = procedure_p->message_p.set.slot_move;

        			NS_LOG_INFO(" MAKING FINAL SLOT MOVE IN LOCAL TABLE with node " << slot_move.mac << ", old: FR " << slot_move.old_slot.frequency_num << ", TIME " << slot_move.old_slot.timeslot_num << ", new: FR " << slot_move.new_slot.frequency_num << ", TIME " << slot_move.new_slot.timeslot_num << " and of (RX,TX - 1,2) slot type " << procedure_p->mode);

        			//Delete here the slot, ai is OK
				superframe->remove_slot( (uint8_t) slot_move.old_slot.frequency_num, (uint8_t) slot_move.old_slot.timeslot_num);

        			//Convert MODE  RX/TX to slot type
        			enum TDMA_slot::tdma_slot_type slot_type=TDMA_slot::UNUSED;
        			if(procedure_p->mode==SLOT_MODE::TX)
        				slot_type = TDMA_slot::UNICAST_SEND;
        			else if(procedure_p->mode==SLOT_MODE::RX)
        				slot_type = TDMA_slot::RECEIVE;
        			else
        				NS_LOG_ERROR("Got an add rx/tx slot set command with unknown mode type " << procedure_p->mode);

        			//If results is ok we add the slot to our table
        			if(!superframe->check_specific_slot_exists_from((mac_address_t) procedure_p->message_p.set.slot_move.mac, (uint8_t) slot_move.new_slot.frequency_num, (uint8_t)slot_move.new_slot.timeslot_num))
        				superframe->add_slot(slot_move.new_slot.frequency_num,
        									 slot_move.new_slot.timeslot_num,
										 slot_type,
										 slot_move.mac);
        			else
      					NS_LOG_WARN("Tried to add rx/tx slot and it the spot wasn't free");
        				//NS_LOG_WARN("Tried to add rx/tx slot and it the spot wasn't free: " << slot_move.new_slot().DebugString());

				//And we send the protocol ACK broadcast packet as well to announce to our neighbors about the deletion
				slot_reference old_slot, new_slot;
				old_slot.channel=slot_move.old_slot.frequency_num;
				old_slot.timeslot=slot_move.old_slot.timeslot_num;
				new_slot.channel=slot_move.new_slot.frequency_num;
				new_slot.timeslot=slot_move.new_slot.timeslot_num;

				//After every set slot move, the node should send a broadcast protocol ACK packet to announce the move operation to third party listener
#ifndef SLOT_ALLOCATION_EXPOSED_NODE
				TDMA_send_transaction_ack_packet(slot_move.mac, move_slot, old_slot, new_slot);
#else
				TDMA_send_transaction_ack_packet(slot_move.mac, move_slot, old_slot, SLOT_MODE::SLOT_MODE_UNKNOWN, new_slot);
#endif
        		}
        		else
        		{
        			//AI said it cannot move the slot, so IF this was a TX slot, send back to the other side that we couldn't move to the new slot and that he has to remove it as well
				//If it is an RX mode it is impossible as handled by AI to be altered before the transaction is done, as it is in Proposed state
				Slot_Move slot_move = procedure_p->message_p.set.slot_move;
				if(procedure_p->mode==SLOT_MODE::TX)
				{
          			//Delete here the slot, ai is OK
					superframe->remove_slot( (uint8_t) slot_move.old_slot.frequency_num, (uint8_t) slot_move.old_slot.timeslot_num);

					TDMA_send_remove_slot_packet((mac_address_t) procedure_p->message_p.set.slot_allocation.mac, (uint8_t) slot_move.new_slot.frequency_num, (uint8_t)slot_move.new_slot.timeslot_num);
				}
        		}
        	}

		//Done processing of the procedure, delete it from the local tables
		delete_procedure(message.transaction_index);
        }

        break;
    }

		default:
			NS_LOG_ERROR("Unknown type of message!");
		break;
	}
}

uint16_t MFTDMAMac::tx_MAC()
{
	bool process_message = true;

	if(test_variable >= superframe->get_table_sframe_duration()/supeframe_num_slots)
	{
		test_variable = 0;
		previous_packets_per_second = 0;
		packets_per_second = 0;
		unblock_low_queue();
		//Logger::log_info<TDMAMacModule>("Queue unblocked!");
		//printf("DRIFT AFTER SUPEFRAME PASSED: %d.\n", superframe->table_check_drift());
	}

	if(update_timer)
	{
		update_timer = false;
		superframe->update_table_current_position(0);
	}
	mac_container container;
	TDMA_table::table_operation_status status = superframe->table_get_next_tx_message(container, next_tx_wkup);
	test_variable += next_tx_wkup;

	while(process_message)
	{
		process_message = false;
		if(status == TDMA_table::NOTHING_TO_SEND)
		{
			//NS_LOG_LOGIC("Nothing to send!");
			//printf("Nothing to send (no send_unicast slot)!\n");
		}
		else
		{
			NS_LOG_LOGIC("Something to send!");
			timestamp_t current_timestamp = clock_get_time_ns();
			//TODO tighten the error gap
			//TODO do we drop the packets, or send them anyway
			//if((current_timestamp / 1000000 + 1) >= container.ctrl_info.timestamp || (current_timestamp / 1000000 + 4) < container.ctrl_info.timestamp)
			if(false)
			{
				update_timer = true;
				NS_LOG_ERROR("ERROR: current timestamp " << (current_timestamp / 1000000) << ", sent packet at timestamp " << container.ctrl_info.timestamp);
				mac_header header;

				for(uint8_t i = 0; i < container.packets_in_data; i++)
				{
					MFTDMAHeader mftdma_header;
					(void)container.data[i]->PeekHeader(mftdma_header);
					header = mftdma_header.MFTDMAGetHeader();

					//Removing values in the ACKs map!
					if (header.need_ack)
						ack.remove_ack_needed_from_map(header.mac_index, header.fragment_index);
				}
			}
			else
			{
	#ifdef TDMA_USE_ACK
				mac_header header;

				for(uint8_t i = 0; i < container.packets_in_data; i++)
				{
					MFTDMAHeader mftdma_header;
					(void)container.data[i]->PeekHeader(mftdma_header);
					header = mftdma_header.MFTDMAGetHeader();

					if(header.mes_type == MAC_CONTROL_ACK)
					{
						mac_index_t mac_ind;
						TDMA_get_control_ack_info(container.data[i]->Copy(), mac_ind);
						remove_control_ack_in_list(header.destination_mac_address, mac_ind);
					}

					//Check if ACK is needed
					if (header.need_ack){

					if((header.mes_type == MAC_TDMA_ADD_SLOT || header.mes_type == MAC_TDMA_DELETE_SLOT
							|| header.mes_type == MAC_TDMA_MOVE_SLOT || header.mes_type == MAC_TDMA_SLOT_TRANSACTION_RESULT)
							&& check_received_control_ack(header.destination_mac_address, header.mac_index))
						{
								NS_LOG_WARN("Already received control ack from " << header.destination_mac_address << " with mac index " << header.mac_index << " and message type " << header.mes_type);
								status = superframe->table_get_cb_message(container, next_tx_wkup);
								process_message = true;
								continue;

								//return next_tx_wkup;
						}
						//Inserting values in the ACKs map!
						if(!ack.update_ack_needed_in_map(header.mac_index, header.fragment_index, true, current_timestamp, container.slot_info.channel, container.slot_info.slot_number))
						{
							ack.add_ack_needed_in_map(header.mac_index,
														header.fragment_index,
														header.destination_mac_address,
														container.ctrl_info.mcs,
														container.ctrl_info.tx_gain,
														container.port[i],
														container.payload_size[i],
														container.slot_info.channel,
														container.slot_info.slot_number,
														(uint8_t)container.slot_info.slot_type,
														container.slot_info.priority,
														current_timestamp,
														RETRY_COUNTER_MAX,
														container.data[i]);

							//Increment ACK statistics
							ack.increment_number_of_sent_packets(header.destination_mac_address,
																 1,
																 current_timestamp); //header.destination_mac_address because it's the destination of the APP packet

						}
					}
				}

				if(process_message)
					continue;
	#endif

				for(uint8_t i = 0; i < container.packets_in_data; i++)
				{
					Mac48Address to = get_ns3_mac(header.destination_mac_address);
					Mac48Address from = get_ns3_mac(own_mac_address);

					//m_channel->Send(container.data[i], container.port[i], to, from, container.slot_info.channel, this);
					m_channel->AddPacket(container.data[i], container.port[i], to, from, container.slot_info.channel, container.ctrl_info.timestamp, container.slot_info.slot_type, container.slot_info.slot_number, this);
				}
				if(header.need_ack)
				{
					if(container.slot_info.slot_type == TDMA_slot::CONTROL_BROADCAST)
						send_control_count += next_tx_wkup;
					else if(container.slot_info.slot_type == TDMA_slot::UNICAST_SEND)
						send_data_count += next_tx_wkup;
				}
				else
				{
					if(header.mes_type == MAC_TDMA_SLOT_TRANSACTION_ACK || header.mes_type == MAC_CONTROL_ACK)
						send_control_no_ack_count += next_tx_wkup;
					else if(header.mes_type == MAC_TDMA_ADVERTISE_SLOT_TABLE)
						send_control_no_ack_advertise_count += next_tx_wkup;
				}

				packets_sent += next_tx_wkup;

				TDMA_send_mac_send_stats_to_AI(container.ctrl_info.timestamp, container.packets_in_data, container.ctrl_info.tx_channel);
				NS_LOG_LOGIC("Sent packets " << packets_sent << "!");
			}
		}
	}

  return next_tx_wkup;
}

TDMA_table::table_operation_status MFTDMAMac::GetRxMessage(TDMA_table::rx_timer_container &rx_container, channel_t &rx_channel)
{
#if USE_RX == USE_RX_NO1
	return superframe->table_get_next_rx_message_no1(rx_container, rx_channel);
#elif USE_RX == USE_RX_NO2
	return superframe->table_get_next_rx_message_no2(rx_container, rx_channel);
#endif
}

inline void
MFTDMAMac::block_low_queue()
{
	queue_is_blocked=true;
}

inline void
MFTDMAMac::unblock_low_queue()
{
	queue_is_blocked=false;
}

inline bool
MFTDMAMac::control_ack_in_list(mac_address_t source_address, mac_index_t mac_packet_index)
{
    std::pair<mac_address_t, mac_index_t> control_ack = std::make_pair(source_address, mac_packet_index);

    for (auto it : control_acks) {
    	if(it == control_ack)
    		return true;
    }

    control_acks.push_back(control_ack);

    while(control_acks.size() > RECEIVED_CONTROL_ACKS_SIZE)
			control_acks.erase(control_acks.begin());

    return false;
}

inline void
MFTDMAMac::remove_control_ack_in_list(mac_address_t source_address, mac_index_t mac_packet_index)
{
	std::pair<mac_address_t, mac_index_t> control_ack = std::make_pair(source_address, mac_packet_index);

	uint16_t i = 0;

  for (auto it : control_acks) {
  	if(it == control_ack)
  	{
  		control_acks.erase(control_acks.begin() + i);
  		break;
  	}
  	i++;
  }
}

inline bool
MFTDMAMac::check_received_control_ack(mac_address_t source_address, mac_index_t mac_packet_index)
{
  std::pair<mac_address_t, mac_index_t> received_control_ack = std::make_pair(source_address, mac_packet_index);

  for (auto it : received_control_acks) {
  	if(it == received_control_ack)
  		return true;
  }

  return false;
}

inline void
MFTDMAMac::add_received_control_ack(mac_address_t source_address, mac_index_t mac_packet_index)
{
	std::pair<mac_address_t, mac_index_t> received_control_ack = std::make_pair(source_address, mac_packet_index);

	received_control_acks.push_back(received_control_ack);

  while(received_control_acks.size() > RECEIVED_CONTROL_ACKS_SIZE)
  	received_control_acks.erase(received_control_acks.begin());
}

bool
MFTDMAMac::table_get_active_slots_more(std::vector<std::pair<std::pair<channel_t, slot_number_t>, std::pair<TDMA_slot::tdma_slot_type, uint16_t>>> &slots)
{
	return superframe->table_get_active_slots_more(slots);
}

/*void
MFTDMAMac::Packet_pad(Ptr<Packet> data, uint16_t slot_size, mac_header header)
{
	uint32_t data_size = data->GetSize ();

  //uint8_t *buffer = new uint8_t[data_size];
	data->CopyData(rx_tx_buffer, data_size);

	NS_LOG_DEBUG("Send APP message size is: " << data_size);

	header.num_pad_bytes =  slot_size - data_size - sizeof(struct mac_header);
	header.mac_crc = crc16(rx_tx_buffer, data_size);

	NS_LOG_DEBUG("Padded bytes is: "<< header.num_pad_bytes <<", CRC16 is " << header.mac_crc);

	MFTDMAHeader mftdma_header;
	mftdma_header.MFTDMASetHeader(header);
	mftdma_header.Print(std::cout);
  data->AddHeader (mftdma_header);
  data->AddPaddingAtEnd (header.num_pad_bytes);

  //delete[] buffer;
}

uint8_t
MFTDMAMac::Packet_unpad(Ptr<Packet> data)
{
	MFTDMAHeader mftdma_header;
  (void)data->RemoveHeader(mftdma_header);
	mftdma_header.Print(std::cout);
	mac_header header = mftdma_header.MFTDMAGetHeader();

	uint16_t num_padded_bytes = header.num_pad_bytes;
	uint16_t crc_check = header.mac_crc;

	NS_LOG_DEBUG("Padded bytes is: "<< num_padded_bytes <<", CRC16 is " << crc_check);

	if(num_padded_bytes <= data->GetSize()){
  	data->RemoveAtEnd(num_padded_bytes);
	}
	else {
		NS_LOG_ERROR("ERROR: Unable to read correct padded bytes value, discarding the packet");
		return 1;
	}

	uint32_t data_size = data->GetSize ();
  //uint8_t *buffer = new uint8_t[data_size];
  data->CopyData(rx_tx_buffer, data_size);

	NS_LOG_DEBUG("Received APP message size is: " << data_size);

	uint16_t crc_calc = crc16(rx_tx_buffer, data_size);

  //delete[] buffer;

	if(crc_check==crc_calc){
		NS_LOG_DEBUG("CRC CHECK OK");
		return 0;
	}
	else{
		NS_LOG_ERROR("ERROR: CRC check failed");
		return 2;
	}
}*/

inline void
MFTDMAMac::Packet_pad(Ptr<Packet> data, uint16_t slot_size, mac_header header)
{
	header.num_pad_bytes =  0;
	header.mac_crc = 0;

	NS_LOG_DEBUG("Padded bytes is: "<< header.num_pad_bytes <<", CRC16 is " << header.mac_crc);

	MFTDMAHeader mftdma_header;
	mftdma_header.MFTDMASetHeader(header);
  data->AddHeader (mftdma_header);
}

inline uint8_t
MFTDMAMac::Packet_unpad(Ptr<Packet> data)
{
	MFTDMAHeader mftdma_header;
  (void)data->RemoveHeader(mftdma_header);
	mac_header header = mftdma_header.MFTDMAGetHeader();

	uint16_t num_padded_bytes = header.num_pad_bytes;
	uint16_t crc_check = header.mac_crc;

	NS_LOG_DEBUG("Padded bytes is: "<< num_padded_bytes <<", CRC16 is " << crc_check);

	return 0;
}

/**
* This function sends last aggregates ACKs for a given destination address
 * @param destination_address, data, mcs_gain, slot_info
 * @return bool
*/
bool MFTDMAMac::TDMA_prepare_and_send_last_ack(mac_address_t destination_address, std::vector<Ptr<Packet>> &data, std::pair<mcs_t, uint32_t> &mcs_gain, std::pair<channel_t, slot_number_t> slot_info){
		//std::lock_guard<std::mutex> ack_send_mutex_lock{ack_send_mutex};

	//Getting ACK buffer data based on the destination_address
    std::list<Acknowledgment::ack_buffer_data>* acks_buffered_data_list_p = ack.get_ack_buffer_data(destination_address);

    if(acks_buffered_data_list_p->size() != 0)
    {
    		NS_LOG_DEBUG("Sending last in slot " << acks_buffered_data_list_p->size() << " ACKs!");

        //std::string *ack_data = new std::string();

        mac_header ack_header;

        ack_header.mes_type = MAC_ACK;
        ack_header.need_ack = false;
#ifdef USE_AGGREGATION
        ack_header.is_aggregated = false;
#endif
    #ifdef IN_PACKET_CHANNEL_ANNOUNCEMENT
        ack_header.residing_channel = RX_channel;
    #endif
        ack_header.destination_mac_address = destination_address;
        ack_header.source_mac_address = own_mac_address;
        ack_header.mac_index = 0;
        ack_header.fragment_index = 0;
        ack_header.fragment_index_max = 0;

        //Adding the ACK preamble
        Acknowledgment::ack_preamble acks_preamble;
        acks_preamble.number_of_acks = acks_buffered_data_list_p->size();
        const char *ack_preamble_str = (const char *) &acks_preamble;
        ack_data->insert(0, ack_preamble_str, sizeof(struct Acknowledgment::ack_preamble));
        NS_LOG_DEBUG("ACK Preamble added to the data packet! ack_data->size(): " << ack_data->size() << " bytes");

        for (auto it : *acks_buffered_data_list_p) {
        		NS_LOG_DEBUG("Grouping ACK with: dst: " << it.destination_address << ", mac_i: " << it.mac_packet_index << ", frag_i: " << (uint16_t)it.fragment_index << ", frag_i_max: " << (uint16_t)it.fragment_index_max << ", cqi: " << (uint16_t)it.cqi << ", rssi: " << (uint16_t)it.rssi << ", pkt_timestamp: " << it.timestamp);

            Acknowledgment::ack_payload acks_payload;
            acks_payload.mac_index = it.mac_packet_index;
            acks_payload.fragment_index = it.fragment_index;
            acks_payload.cqi = it.cqi;
            acks_payload.rssi = it.rssi;

            const char* ack_payload_str = (const char*)&acks_payload;
            ack_data->insert(ack_data->size(), ack_payload_str, sizeof(struct Acknowledgment::ack_payload));
            NS_LOG_DEBUG("An ACK added to the last in slot aggregated data packet! ack_data->size(): " << ack_data->size() << " bytes");
        }

        Ptr<Packet> ack_data_packet = Create<Packet> (reinterpret_cast<const uint8_t*> (ack_data->c_str()) , ack_data->size());

        //uint16_t slot_size = get_bytes_in_slot(DEFAULT_BW_INDEX, get_mcs_based_on_destination(destination_address));
        uint16_t slot_size = get_bytes_in_slot(get_active_channel_bandwitdh_index(), this->min_mcs);
        Packet_pad(ack_data_packet, slot_size, ack_header);
        mcs_gain.first = this->min_mcs;
        mcs_gain.second = get_txgain_based_on_destination(destination_address, this->init_tx_gain);
        data.push_back(ack_data_packet);
        //print_packet_data(ack_data);

        NS_LOG_DEBUG("All ACKs added to superframe!!");

        TDMA_send_receiver_slot_stats_to_AI(slot_info, destination_address);

        //delete ack_data;
        ack_data->clear();

        //Free ACKs buffer for a given destination address
        acks_buffered_data_list_p->clear();

        //Check if the list was cleared
        if(!acks_buffered_data_list_p->empty())
        	NS_LOG_ERROR("ACKS list not cleared successfully");

        return true;
    }
    else
    	return false;
}

#ifdef TDMA_USE_ACK
/**
* This function sends aggregates ACKs for a given destination address
 * @param destination_address
 * @return void
*/
void MFTDMAMac::TDMA_prepare_and_send_ack(mac_address_t destination_address){

	//Getting ACK buffer data based on the destination_address
    std::list<Acknowledgment::ack_buffer_data>* acks_buffered_data_list_p = ack.get_ack_buffer_data(destination_address);

    if(acks_buffered_data_list_p->size() != 0)
    {
        NS_LOG_DEBUG("Sending " << acks_buffered_data_list_p->size() << " ACKs!");

        //std::string *ack_data = new std::string();

        mac_header ack_header;

        ack_header.mes_type = MAC_ACK;
        ack_header.need_ack = false;
        ack_header.destination_mac_address = destination_address;
        ack_header.source_mac_address = own_mac_address;
        ack_header.mac_index = 0;
        ack_header.fragment_index = 0;
        ack_header.fragment_index_max = 0;

        //Adding the ACK preamble
        Acknowledgment::ack_preamble acks_preamble;
        acks_preamble.number_of_acks = acks_buffered_data_list_p->size();
        const char *ack_preamble_str = (const char *) &acks_preamble;
        ack_data->insert(0, ack_preamble_str, sizeof(struct Acknowledgment::ack_preamble));
        NS_LOG_DEBUG("ACK Preamble added to the data packet! ack_data->size(): " << ack_data->size() << " bytes");

        for (auto it : *acks_buffered_data_list_p) {
        		NS_LOG_DEBUG("Grouping ACK with: dst: " << it.destination_address << ", mac_i: " << it.mac_packet_index << ", frag_i: " << (uint16_t)it.fragment_index << ", frag_i_max: " << (uint16_t)it.fragment_index_max << ", cqi: " << (uint16_t)it.cqi << ", rssi: " << (uint16_t)it.rssi << ", pkt_timestamp: " << it.timestamp);

            Acknowledgment::ack_payload acks_payload;
            acks_payload.mac_index = it.mac_packet_index;
            acks_payload.fragment_index = it.fragment_index;
            acks_payload.cqi = it.cqi;
            acks_payload.rssi = it.rssi;

            const char* ack_payload_str = (const char*)&acks_payload;
            ack_data->insert(ack_data->size(), ack_payload_str, sizeof(struct Acknowledgment::ack_payload));
            NS_LOG_DEBUG("An ACK added to the aggregated data packet! ack_data->size(): " << ack_data->size() << " bytes");
        }

        Ptr<Packet> ack_data_packet = Create<Packet> (reinterpret_cast<const uint8_t*> (ack_data->c_str()) , ack_data->size());

        //uint16_t slot_size = get_bytes_in_slot(DEFAULT_BW_INDEX, get_mcs_based_on_destination(destination_address));
        uint16_t slot_size = get_bytes_in_slot(get_active_channel_bandwitdh_index(), this->min_mcs);
        Packet_pad(ack_data_packet, slot_size, ack_header);

        //print_packet_data(ack_data);

        /*
        Mac48Address to;
				Mac48Address from;

				uint8_t temp_mac[6];
				get_array_from_mac_address(destination_address, temp_mac);
				to.CopyFrom(temp_mac);
				get_array_from_mac_address(own_mac_address, temp_mac);
				from.CopyFrom(temp_mac);

        m_channel->Send(ack_data_packet, 0, to, from, this);*/

        std::pair<channel_t, slot_number_t> slot_info;
        TDMA_table::table_operation_status status = superframe->table_add_ack(ack_data_packet, destination_address, slot_info, get_txgain_based_on_destination(destination_address, this->init_tx_gain), this->min_mcs);

        if(status == TDMA_table::SUCCESS)
        {
        		NS_LOG_DEBUG("All ACKs added to superframe!!");

            TDMA_send_receiver_slot_stats_to_AI(slot_info, destination_address);
        }
        else
        {
        	NS_LOG_ERROR("ACKs dropped. No RECEIVE_tx slots for remote mac address " << destination_address << ".");
        }

        //delete ack_data;
        ack_data->clear();

        //Free ACKs buffer for a given destination address
        acks_buffered_data_list_p->clear();

        //Check if the list was cleared
        if(!acks_buffered_data_list_p->empty())
        	NS_LOG_ERROR("ACKS list not cleared successfully");
    }
}

/**
* This function sends control ACKs for a given destination address
 * @param destination_address
 * @return void
*/
void MFTDMAMac::TDMA_prepare_and_send_control_ack(mac_address_t destination_address,
													  mac_index_t mac_index,
													  fragment_index_t fragment_index,
													  cqi_t cqi,
													  rssi_t rssi){

	if(control_ack_in_list(destination_address, mac_index))
	{
		NS_LOG_WARN("Already sent control ack to " << destination_address << " with mac index " << mac_index);

		return;
	}

	//std::string *ack_data = new std::string();

	mac_header ack_header;

	ack_header.mes_type = MAC_CONTROL_ACK;
	ack_header.need_ack = false;
#ifdef USE_AGGREGATION
    ack_header.is_aggregated = false;
#endif
#ifdef IN_PACKET_CHANNEL_ANNOUNCEMENT
	ack_header.residing_channel = RX_channel;
#endif
	ack_header.destination_mac_address = destination_address;
	ack_header.source_mac_address = own_mac_address;
	ack_header.mac_index = 0;
	ack_header.fragment_index = 0;
	ack_header.fragment_index_max = 0;

	//Adding the ACK preamble
	Acknowledgment::ack_preamble acks_preamble;
	acks_preamble.number_of_acks = 1;
	const char *ack_preamble_str = (const char *) &acks_preamble;
	ack_data->insert(0, ack_preamble_str, sizeof(struct Acknowledgment::ack_preamble));
	NS_LOG_DEBUG("ACK Preamble added to the data packet! ack_data->size(): " << ack_data->size() << " bytes");

	NS_LOG_DEBUG("Adding ACK with: dst: " << destination_address << ", mac_i: " << mac_index << ", frag_i: " << (uint16_t)fragment_index << ", cqi: " << (uint16_t)cqi << ", rssi: " << (uint16_t)rssi);

	Acknowledgment::ack_payload acks_payload;
	acks_payload.mac_index = mac_index;
	acks_payload.fragment_index = fragment_index;
	acks_payload.cqi = cqi;
	acks_payload.rssi = rssi;

	const char* ack_payload_str = (const char*)&acks_payload;
	ack_data->insert(ack_data->size(), ack_payload_str, sizeof(struct Acknowledgment::ack_payload));
	NS_LOG_DEBUG("An control ACK added! ack_data->size(): " << ack_data->size() << " bytes");

  Ptr<Packet> ack_data_packet = Create<Packet> (reinterpret_cast<const uint8_t*> (ack_data->c_str()) , ack_data->size());

	uint16_t slot_size = get_bytes_in_slot(get_active_channel_bandwitdh_index(), this->min_mcs);
	Packet_pad(ack_data_packet, slot_size, ack_header);

	//print_packet_data(ack_data);

	TDMA_table::table_operation_status status = superframe->table_add_control_ack(ack_data_packet, destination_address, get_txgain_based_on_destination(destination_address, this->init_tx_gain), this->min_mcs);

	if(status == TDMA_table::SUCCESS)
	{
		NS_LOG_DEBUG("Control ACK added to superframe!!");
	}
	else
	{
		NS_LOG_DEBUG("Control ACK dropped. No CONTROL_BROADCAST slots for remote mac address " << destination_address << ".");
	}

	//delete ack_data;
  ack_data->clear();
}

void MFTDMAMac::TDMA_aggregate_ack_into_buffer(mac_address_t sender_address,
										  mac_index_t mac_index,
										  fragment_index_t fragment_index,
										  fragment_index_max_t fragment_index_max,
										  cqi_t cqi,
										  rssi_t rssi,
										  timestamp_t current_timestamp) {

	//std::lock_guard<std::mutex> ack_send_mutex_lock{ack_send_mutex};
	//Filtering unidentified source_address
	if (sender_address != 0){
		//Aggregating ACK into buffer
		NS_LOG_DEBUG("Aggregating ACK with -> Destination: " << sender_address << ", mac_packet_index: " << mac_index << ", fragment_index: " << (uint16_t)fragment_index << ", and fragment_index_max " << (uint16_t)fragment_index_max);
		//Adding ACK to the buffer
		if (ack.add_ack_data_into_buffer(sender_address, mac_index, fragment_index, fragment_index_max, cqi, rssi, current_timestamp)){
			//If there are ACKs to be sent
			if (ack.check_exceeded_ack_buffer_data(sender_address)){
				//Send all ACKs for a given destination address
				TDMA_prepare_and_send_ack(sender_address);
			}
		}
	} else {
		ack.remove_ack_needed_from_map(mac_index, fragment_index);
		NS_LOG_ERROR("Not aggregating ACK and removing from the ACK_needed_map because its destination_address = " << sender_address);
	}
}

/**
* This function handles the ACKs received
 * @param app_data
 * @return void
*/
void MFTDMAMac::TDMA_handle_received_ack(Ptr<Packet> app_data){

    //Reading the ACKs Preamble
    NS_LOG_DEBUG("Treating all the ACKS received");
    //(void)app_data->CopyData(rx_tx_buffer, 2000);

  	uint32_t app_data_size = app_data->GetSize ();
    app_data->CopyData(rx_tx_buffer, app_data_size);

    const struct Acknowledgment::ack_preamble* acks_preamble = (struct Acknowledgment::ack_preamble*) rx_tx_buffer;
    uint8_t number_of_acks = acks_preamble->number_of_acks;
    ack_data_count += number_of_acks;
    NS_LOG_DEBUG("acks_preamble->number_of_acks: " << (uint16_t)acks_preamble->number_of_acks << ", aux_number_of_acks: " << (uint16_t)number_of_acks);
    uint32_t buffer_index = sizeof(Acknowledgment::ack_preamble);

    timestamp_t current_time = clock_get_time_ns();
    //Reading the ACKs Payload
    for (int i = 0; i < number_of_acks; i++){
        struct Acknowledgment::ack_payload* acks_payload = (struct Acknowledgment::ack_payload*) (rx_tx_buffer + buffer_index);
        NS_LOG_DEBUG("acks_payload->mac_index: " << acks_payload->mac_index << ", fragment_index: " << (uint16_t)acks_payload->fragment_index);

        //Getting info from ack_needed_map, assuming that we have enough mac_index and fragment_index ranges to avoid inconsistency!
        Acknowledgment::ack_needed_info ack_needed_info_data;

        if(ack.get_ack_needed_info(acks_payload->mac_index, acks_payload->fragment_index, ack_needed_info_data))
        {
            //Creating struct to comprise the ACK stats
            Acknowledgment::ack_statistics ack_stats;

            NS_LOG_DEBUG("ACK received after time: " << (current_time - ack_needed_info_data.timestamp)/1000000);
            //Populating the structure
            ack_stats.destination_address = ack_needed_info_data.destination_mac_address; //Destination address from where an ACK is received (Destination of the APP packet)
            ack_stats.status = Acknowledgment::SUCCESS;
            ack_stats.mcs = ack_needed_info_data.mcs;
            ack_stats.tx_gain = ack_needed_info_data.tx_gain;
            ack_stats.port = ack_needed_info_data.port;
            ack_stats.payload_size = ack_needed_info_data.payload_size;
            ack_stats.tx_channel = ack_needed_info_data.tx_channel;
            ack_stats.slot_index = ack_needed_info_data.slot_index;
            ack_stats.max_retry_counter = ack_needed_info_data.max_retry_counter;
            ack_stats.retry_counter = ack_needed_info_data.max_retry_counter - ack_needed_info_data.retry_counter;
            ack_stats.bytes_transmitted = get_bytes_in_slot(get_active_channel_bandwitdh_index(), ack_needed_info_data.mcs);

			//If mac_index is sucessfully removed from acks_map then
				if (ack.remove_ack_needed_from_map(acks_payload->mac_index, acks_payload->fragment_index)){

									//Increment Aggregated ACK statistics
									ack.increment_number_of_received_acks(ack_stats.destination_address, 1, current_time); //source_address from the receiver perspective or the destination address of the APP packet
									//ack.print_ack_aggregated_statistics();

									//Calling Function to send stats to the AI
									TDMA_send_ack_stats_to_AI(ack_stats);
							}
		}
		else
		{
			NS_LOG_WARN("Duplicate ack with acks_payload->mac_index: " << acks_payload->mac_index << ", fragment_index: " << acks_payload->fragment_index);
		}

    		buffer_index += sizeof(Acknowledgment::ack_payload);
	}
	//Update moving sample buffer with the ACK statistics
	ack.update_ack_moving_sample(current_time);
}

/**
* This function handles the control ACKs received
 * @param app_data
 * @return void
*/
void MFTDMAMac::TDMA_handle_received_control_ack(Ptr<Packet> app_data){

    //Reading the ACKs Preamble
    NS_LOG_DEBUG("Treating control ACK received");
    //(void)app_data->CopyData(rx_tx_buffer, 2000);

  	uint32_t app_data_size = app_data->GetSize ();
    app_data->CopyData(rx_tx_buffer, app_data_size);

    const struct Acknowledgment::ack_preamble* acks_preamble = (struct Acknowledgment::ack_preamble*) rx_tx_buffer;
    uint8_t number_of_acks = acks_preamble->number_of_acks;
    ack_control_count += number_of_acks;
    NS_LOG_DEBUG("acks_preamble->number_of_acks: " << (uint16_t)acks_preamble->number_of_acks << ", aux_number_of_acks: " << (uint16_t)number_of_acks);
    uint32_t buffer_index = sizeof(Acknowledgment::ack_preamble);

    timestamp_t current_time = clock_get_time_ns();
    //Reading the ACKs Payload
    for (int i = 0; i < number_of_acks; i++){
        struct Acknowledgment::ack_payload* acks_payload = (struct Acknowledgment::ack_payload*) (rx_tx_buffer + buffer_index);
        NS_LOG_DEBUG("acks_payload->mac_index: " << acks_payload->mac_index << ", fragment_index: " << (uint16_t)acks_payload->fragment_index);

        //Getting info from ack_needed_map, assuming that we have enough mac_index and fragment_index ranges to avoid inconsistency!
        Acknowledgment::ack_needed_info ack_needed_info_data;

        if(ack.get_ack_needed_info(acks_payload->mac_index, acks_payload->fragment_index, ack_needed_info_data))
        {
        		NS_LOG_DEBUG("ACK received after time: " << (current_time - ack_needed_info_data.timestamp)/1000000);
            //Populating the structure
            mac_address_t destination_address = ack_needed_info_data.destination_mac_address; //Destination address from where an ACK is received (Destination of the APP packet)
            add_received_control_ack(destination_address, acks_payload->mac_index);
			//If mac_index is sucessfully removed from acks_map then
			if (ack.remove_ack_needed_from_map(acks_payload->mac_index, acks_payload->fragment_index)){

                //Increment Aggregated ACK statistics
                ack.increment_number_of_received_acks(destination_address, 1, current_time); //source_address from the receiver perspective or the destination address of the APP packet
                //ack.print_ack_aggregated_statistics();
            }
		}
		else
		{
			NS_LOG_WARN("Duplicate control ack with acks_payload->mac_index: " << acks_payload->mac_index << ", fragment_index: " << (uint16_t)acks_payload->fragment_index);
		}

		buffer_index += sizeof(Acknowledgment::ack_payload);
	}
	//Update moving sample buffer with the ACK statistics
	ack.update_ack_moving_sample(current_time);
}

/**
* This function sends individual ACK stats to the AI
 * @param ack_statistics
 * @return void
*/
void MFTDMAMac::TDMA_send_ack_stats_to_AI(Acknowledgment::ack_statistics ack_stats){
	NS_LOG_DEBUG("**** send_ack_stats_to_AI *****");
	NS_LOG_DEBUG("Dst: " << ack_stats.destination_address << ",  Status: " << ack_stats.status << ", MCS: " << (uint16_t)ack_stats.mcs << ", tx_gain: " << ack_stats.tx_gain << ", tx_channel: " << (uint16_t)ack_stats.tx_channel << ", retries: " << (uint16_t)ack_stats.retry_counter << ", bytes: " << ack_stats.bytes_transmitted);

	//Create a stats message to send to AI, MAC stats are included in the message, see format_message implementation
	Internal stats_mes;
	stats_mes.module = MODULE_AI;
	stats_mes.attribute = MESSAGE_STATS;
	stats_mes.stats.mac_stat.attribute = MAC_STAT_TX;

	stats_mes.stats.mac_address = ack_stats.destination_address;

	//Setting the ACK statistics here
	Mac_tx_stat mac_tx_stats_p;

	Mac_ack_stat mac_ack_stats_p;

	//Setting ACK Counter Statistics
	mac_ack_stats_p.destination_address = ack_stats.destination_address;

	mac_ack_stats_p.status = (ack_stats.status == Acknowledgment::SUCCESS ? Mac_ack_stat::ACKSTATUS_SUCCESS : Mac_ack_stat::ACKSTATUS_FAILED);
	mac_ack_stats_p.mcs = ack_stats.mcs;
	mac_ack_stats_p.tx_gain = ack_stats.tx_gain;
	mac_ack_stats_p.port = ack_stats.port;
	mac_ack_stats_p.payload_size = ack_stats.payload_size;
	mac_ack_stats_p.tx_channel = ack_stats.tx_channel;

	mac_ack_stats_p.slot_index = ack_stats.slot_index;

	mac_ack_stats_p.max_retries = ack_stats.max_retry_counter;
	mac_ack_stats_p.num_retries = ack_stats.retry_counter;
	mac_ack_stats_p.bytes_transmitted = ack_stats.bytes_transmitted;

	stats_mes.stats.mac_stat.tx_stat.ack_stat.push_back(mac_ack_stats_p);

#ifndef STATIC_TABLE_TYPE
	//Send message to AI
	stats_mes.transaction_index = m_device->MacAIMessageIndexUpdate();
	m_device->SendMacAIMessage(stats_mes);
#endif

	NS_LOG_DEBUG("**** ACK stats sent to the AI *****");
}

/**
* This function sends Mac_send_stats to the AI
 * @param timestamp, packet_count, channel
 * @return void
*/
inline void MFTDMAMac::TDMA_send_mac_send_stats_to_AI(timestamp_t timestamp, uint16_t packet_count, channel_t channel)
{
}

/**
* This function sends receive slot stats to the AI
 * @param channel, slot_number, sender_address
 * @return void
*/
void MFTDMAMac::TDMA_send_receiver_slot_stats_to_AI(std::pair<channel_t, slot_number_t> slot_info, mac_address_t sender_address)
{
	NS_LOG_DEBUG("**** send_receiver_slot_stats_to_AI *****");
	NS_LOG_DEBUG("Sender: " << sender_address << ",  Channel: " << (uint16_t)slot_info.first << ", Slot index: " << (uint16_t)slot_info.second);

	Internal stats_mes;
	stats_mes.module = MODULE_AI;
	stats_mes.attribute = MESSAGE_STATS;
	stats_mes.stats.mac_stat.attribute = MAC_STAT_RX;

	//Embed here the MAC stats
	Mac_stat mac_stats_p;
	//Setting the ACK statistics here
	Mac_rx_stat mac_rx_stats_p = {slot_info.first, slot_info.second};

	stats_mes.stats.mac_address = sender_address;
	stats_mes.stats.mac_stat.rx_stat = mac_rx_stats_p;

#ifndef STATIC_TABLE_TYPE
	//Send message to AI
	stats_mes.transaction_index = m_device->MacAIMessageIndexUpdate();
	m_device->SendMacAIMessage(stats_mes);
#endif

	NS_LOG_DEBUG("**** Receive slot stats sent to the AI *****");
}

void MFTDMAMac::TDMA_get_control_ack_info(Ptr<Packet> app_data, mac_index_t &mac_index)
{
	MFTDMAHeader mftdma_header;
  (void)app_data->RemoveHeader(mftdma_header);

	uint32_t data_size = app_data->GetSize ();

  uint8_t *buffer = new uint8_t[data_size];
  app_data->CopyData(buffer, data_size);

  const struct Acknowledgment::ack_preamble* acks_preamble = (struct Acknowledgment::ack_preamble*) buffer;
  uint8_t number_of_acks = acks_preamble->number_of_acks;

  NS_LOG_DEBUG("number_of_acks: " << (uint16_t)number_of_acks);
  uint32_t buffer_index = sizeof(Acknowledgment::ack_preamble);

  struct Acknowledgment::ack_payload* acks_payload = (struct Acknowledgment::ack_payload*) (buffer + buffer_index);
  NS_LOG_DEBUG("acks_payload->mac_index: " << acks_payload->mac_index << ", fragment_index: " << (uint16_t)acks_payload->fragment_index);

  mac_index = acks_payload->mac_index;
}

#endif //TDMA_USE_ACK

//Procedures keeping state relevant functions

inline void MFTDMAMac::add_procedure(SLOT_ATTRIBUTE set_type, SLOT_MODE mode, uint64_t transaction_id, Internal mes)
{
	set_process* set_p = new set_process();
	set_p->message_p = mes;
	set_p->set_type=set_type;
	set_p->mode=mode;

	procedures_table[transaction_id] = set_p;
}

set_process* MFTDMAMac::procedure_exists(uint64_t trans_id)
{
	std::map<uint64_t,set_process*>::iterator it = procedures_table.find(trans_id);

	if(it==procedures_table.end())
		return NULL;

	return it->second;
}

inline void MFTDMAMac::delete_procedure(uint64_t trans_id)
{
	std::map<uint64_t,set_process*>::iterator iter = procedures_table.find(trans_id);

	//Deleting the actual set_procces struct, so no memory leak occurs
	set_process* set_p =  iter->second;
	delete set_p;

	//Deleting the element in the table for the specific transaction ID
	procedures_table.erase(trans_id);
}

BW_INDEX MFTDMAMac::get_active_channel_bandwitdh_index(){

	switch(this->channel_step){

	case 1400000:
		return BW_INDEX::OneDotFour;
	break;
	case 3000000:
			return BW_INDEX::Three;
		break;
	case 5000000:
			return BW_INDEX::Five;
		break;
	case 10000000:
			return BW_INDEX::Ten;
		break;
	case 15000000:
			return BW_INDEX::Fifteen;
		break;
	case 20000000:
			return BW_INDEX::Twenty;
		break;
	default:
		return BW_INDEX::BW_INDEX_UNKNOWN;
		break;
	}
}

inline Mac48Address MFTDMAMac::get_ns3_mac(mac_address_t mac)
{
	Mac48Address ns3_mac;
	uint8_t temp_mac[6];

	get_array_from_mac_address(mac, temp_mac);
	ns3_mac.CopyFrom(temp_mac);

	return ns3_mac;
}

Mac48Address MFTDMAMac::GetAddress (void)
{
	return ns3_own_mac_address;
}

/**
 * These are the ALLOCATE slot procedure functions following the series of events
 */

//EVENT ONE, TDMA wants to allocate a new slot for a mac address
void MFTDMAMac::TDMA_initiate_allocate_new_slot(mac_address_t target){

	NS_LOG_INFO("Initiate slot allocation procedure with node : " << target);

	//Create and send a ZMQ message to AI to ask for proposed slots to node mac_addr_v
	Internal get_mes;
	get_mes.module = MODULE_AI;
	get_mes.attribute = MESSAGE_GET;
	get_mes.get.attribute = PROPOSED_SLOTS;

	//Create the proposal slot request structure
	Proposed_Slots_Request proposed_slots_structure_p;

	//Populate the fields of proposal slot request
	proposed_slots_structure_p.mac = target;
	proposed_slots_structure_p.num_slots = MAX_NUMBER_PROPOSED_SLOTS;

	get_mes.get.slot_request = proposed_slots_structure_p;

	//Send message to AI
	get_mes.transaction_index = m_device->MacAIMessageIndexUpdate();
	m_device->SendMacAIMessage(get_mes);

	NS_LOG_INFO("**** Get proposed slots command sent to the AI *****");
}

//Event two, received a response from AI and sending the proposed slots to the receiver side
void MFTDMAMac::TDMA_send_proposed_slots_packet(mac_address_t remote_node, uint8_t number__of_proposals, slot_reference slot_table[] ){

	//Create the payload container
	std::string* slot_allocation_payload = new std::string();

	//Find suggested slots and add them to the payload

	//Create packet header
	add_slot_payload payload;
	payload.initiator=this->own_mac_address;
	payload.number_of_suggested_slots=number__of_proposals;
	memset(payload.slot_table, 0, sizeof(payload.slot_table));

	//Add slots payload
	for(uint8_t i=0; i<number__of_proposals; i++){
		payload.slot_table[i].channel=	slot_table[i].channel;
		payload.slot_table[i].timeslot=	slot_table[i].timeslot;
	}

	//Copy payload into std::string buffer
	const char* payload_str = (const char*)&payload;
	slot_allocation_payload->insert(slot_allocation_payload->size(), payload_str, (sizeof(add_slot_payload)-(MAX_SUGGESTED_SLOTS-number__of_proposals)*2));

	//Check if all is fine
	//print_packet_data(slot_allocation_payload);

	NS_LOG_INFO("Sending MAC_TDMA_PROPOSED_SLOTS packet to mac = " << remote_node << " , our mac address is " << payload.initiator);

	Ptr<Packet> slot_allocation_payload_packet = Create<Packet> (reinterpret_cast<const uint8_t*> (slot_allocation_payload->c_str()) , slot_allocation_payload->size());
	Mac48Address to = get_ns3_mac(remote_node);
	Mac48Address from = get_ns3_mac(this->own_mac_address);

	TDMA_MAC_send(slot_allocation_payload_packet, MAC_TDMA_ADD_SLOT, 0, to, from, true);
}

//Event 4, the receiver side gets the proposed_slots packet and initiates a GET selected slot message to AI to select amongst the proposed slots the best candidate
void MFTDMAMac::TDMA_send_GET_selected_slot_to_AI(mac_address_t initiator, uint8_t number__of_proposals, slot_reference slot_table[]){

	NS_LOG_INFO("NODE: " << initiator << " is asking to setup a slot to this node");

	//Create and send a ZMQ message to AI to ask for selected slot to this node from node with mac_addr_v
	Internal get_mes;
	get_mes.module = MODULE_AI;
	get_mes.attribute = MESSAGE_GET;
	get_mes.get.attribute = SELECTED_SLOT;

	Proposed_Slots slot_proposal_p;
	//Copy in the slot proposal container the slots coming in from the incoming packet
	slot_proposal_p.mac = initiator;

	for(uint8_t i=0; i<number__of_proposals; i++){
		MFTDMA_Slot test_slot = {slot_table[i].channel, slot_table[i].timeslot};
		slot_proposal_p.slots.push_back(test_slot);
	}

	get_mes.get.proposed_slots = slot_proposal_p;

	//Send message to AI
	get_mes.transaction_index = m_device->MacAIMessageIndexUpdate();
	m_device->SendMacAIMessage(get_mes);

	NS_LOG_INFO("**** Get Selected Slot command send to AI, carrying proposed slots *****");
}

void MFTDMAMac::TDMA_send_GET_selected_move_slot_to_AI(mac_address_t initiator, uint8_t number_of_proposals, slot_reference old_slot, slot_reference slot_table[])
{
	NS_LOG_INFO("NODE: " << initiator << " is asking to move a slot at this node");

	//Create and send a ZMQ message to AI to ask for selected slot to this node from node with mac_addr_v
	Internal get_mes;
	get_mes.module = MODULE_AI;
	get_mes.attribute = MESSAGE_GET;
	get_mes.get.attribute = SELECTED_MOVE_SLOT;

	Proposed_Move_Slots slot_proposal_p;
	//Copy in the slot proposal container the slots coming in from the incoming packet
	slot_proposal_p.mac = initiator;

	for(uint8_t i=0; i<number_of_proposals; i++){
		MFTDMA_Slot test_slot = {slot_table[i].channel, slot_table[i].timeslot};
		slot_proposal_p.slots.push_back(test_slot);
	}

	MFTDMA_Slot remove_slot = {old_slot.channel, old_slot.timeslot};
	slot_proposal_p.old_slot = remove_slot;

	get_mes.get.proposed_move_slots = slot_proposal_p;

	//Send message to AI
	get_mes.transaction_index = m_device->MacAIMessageIndexUpdate();
	m_device->SendMacAIMessage(get_mes);

	NS_LOG_INFO("**** Get Selected Move Slot command send to AI, carrying proposed slots *****");
}

//Event 5-6, AI responts with a proper selected slot and the process continues with sending a packet with the response back the the initiator of the procedure
void MFTDMAMac::TDMA_send_selected_slot_packet(mac_address_t initiator, slot_action action, uint8_t channel, uint8_t slot, uint8_t new_channel, uint8_t new_slot){
	//Create the payload container
	std::string* slot_selected_payload = new std::string();

	//Create packet header
	slot_transaction_result_payload payload;
	payload.initiator=initiator;
	payload.action=action;

	//Add response containing the selected slot from AI to packet
	payload.relevant_slot.channel=channel;
	payload.relevant_slot.timeslot=slot;

	//TODO check what we need to check if the slot response is not ok.
	if(channel!=255 && slot!=255)
		payload.result=SUCCESS;
	else
		payload.result=FAIL;

	payload.new_slot.channel = new_channel;
	payload.new_slot.timeslot = new_slot;
	if(action == move_slot)
	{
		if(new_channel!=255 && new_slot!=255)
			payload.result=SUCCESS;
		else
			payload.result=FAIL;
	}

	//Copy payload into std::string buffer
	const char* payload_str = (const char*)&payload;
	slot_selected_payload->insert(slot_selected_payload->size(), payload_str, sizeof(slot_transaction_result_payload));

	//Check if all is fine
	print_packet_data(slot_selected_payload);
	NS_LOG_INFO("SENDING SELECTED SLOT BACK: dest max = " << initiator << " , our mac address is " << this->own_mac_address << " action " << action << " (1 add, 2 del, 3 mv)");

	Ptr<Packet> slot_selected_payload_packet = Create<Packet> (reinterpret_cast<const uint8_t*> (slot_selected_payload->c_str()) , slot_selected_payload->size());
	Mac48Address to = get_ns3_mac(initiator);
	Mac48Address from = get_ns3_mac(this->own_mac_address);

	TDMA_MAC_send(slot_selected_payload_packet, MAC_TDMA_SLOT_TRANSACTION_RESULT, 0, to, from, true);
}

//Event 7, the initiator gets the packet with the selected slot and informs AI with a SET
void MFTDMAMac::TDMA_send_SET_slot_allocation_to_AI( mac_address_t remote_node, SLOT_MODE mode, uint8_t channel, uint8_t timeslot ){

	NS_LOG_INFO("Forwarding successful slot allocation response of NODE " << remote_node << " to AI");

	//Create and send a ZMQ message to AI to set the selected slot to remote node from this node
	Internal set_mes;
	set_mes.module = MODULE_AI;
	set_mes.attribute = MESSAGE_SET;
	set_mes.set.attribute = SLOT_ALLOCATION;

	Slot_Allocation slot_allocation_p;

	//Populate the message
	//Copy in the slot proposal container the slots coming in from the incoming packet
	MFTDMA_Slot selected_slot = {channel, timeslot};
	slot_allocation_p.slot = selected_slot;

	slot_allocation_p.mode = mode;
	slot_allocation_p.mac = remote_node;

	set_mes.set.slot_allocation = slot_allocation_p;

	this->final_slot_allocation_in_progress=true;

	//Send message to AI
	set_mes.transaction_index = m_device->MacAIMessageIndexUpdate();

	//Log this transaction in the lookup table and look later if you got a response
	//ADD this set type , and the transaction id and the set struct to our table
	add_procedure(SLOT_ALLOCATION, mode, set_mes.transaction_index, set_mes);

	m_device->SendMacAIMessage(set_mes);

	NS_LOG_INFO("**** Set Selected Slot command send to AI, carrying final slot selection *****");
}

void MFTDMAMac::TDMA_send_SET_slot_move_to_AI(mac_address_t remote_node, SLOT_MODE mode, uint8_t old_channel, uint8_t old_timeslot, uint8_t new_channel, uint8_t new_timeslot)
{
	NS_LOG_INFO("Forwarding successful slot move response of NODE " << remote_node << " to AI");

	//Create and send a ZMQ message to AI to set the selected slot to remote node from this node
	Internal set_mes;
	set_mes.module = MODULE_AI;
	set_mes.attribute = MESSAGE_SET;
	set_mes.set.attribute = SLOT_MOVE;

	Slot_Move slot_move_p;

	//Populate the message
	//Copy in the slot move container the slots coming in from the incoming packet
	MFTDMA_Slot selected_slot = {new_channel, new_timeslot};
	MFTDMA_Slot remove_slot = {old_channel, old_timeslot};

	slot_move_p.new_slot = selected_slot;
	slot_move_p.old_slot = remove_slot;

	slot_move_p.mode = mode;
	slot_move_p.mac = remote_node;

	set_mes.set.slot_move = slot_move_p;

	this->final_slot_allocation_in_progress=true;

	//Send message to AI
	set_mes.transaction_index = m_device->MacAIMessageIndexUpdate();

	//Log this transaction in the lookup table and look later if you got a response
  //ADD this set type , and the transaction id and the set struct to our table
	add_procedure(SLOT_MOVE, mode, set_mes.transaction_index, set_mes);

	m_device->SendMacAIMessage(set_mes);

	NS_LOG_INFO("**** Set Selected Move Slot command send to AI, carrying final slot selection *****");
}

//Event 6b and 9, both sides Send ACK message as a broadcast to inform all about the slot around them
#ifndef SLOT_ALLOCATION_EXPOSED_NODE
void MFTDMAMac::TDMA_send_transaction_ack_packet(mac_address_t remote_node, slot_action action,slot_reference slot, slot_reference slot_new){
#else
	void MFTDMAMac::TDMA_send_transaction_ack_packet(mac_address_t remote_node, slot_action action,slot_reference slot, SLOT_MODE slot_mode, slot_reference slot_new){
#endif
	//Create the payload container
	std::string* slot_selected_payload = new std::string();

	//Create packet header
	slot_transaction_ack_payload payload;
	payload.initiator=this->own_mac_address;
	payload.remote=remote_node;

	payload.action=action;

	//Add response containing the selected slot from AI to packet
	payload.relevant_slot.channel=slot.channel;
	payload.relevant_slot.timeslot=slot.timeslot;

	//TODO check what we need to check if the slot response is ok.
	if(slot.channel!=255 && slot.timeslot!=255)
		payload.result=SUCCESS;
	else
		payload.result=FAIL;

#ifdef SLOT_ALLOCATION_EXPOSED_NODE
	payload.mode = slot_mode;
#endif

	payload.new_slot.channel=slot_new.channel;
	payload.new_slot.timeslot=slot_new.timeslot;
	if(action == move_slot)
	{
		if(slot_new.channel!=255 && slot_new.timeslot!=255)
			payload.result=SUCCESS;
		else
			payload.result=FAIL;
	}

	//Copy payload into std::string buffer
	const char* payload_str = (const char*)&payload;
	slot_selected_payload->insert(slot_selected_payload->size(), payload_str, sizeof(slot_transaction_ack_payload));

	//Check if all is fine
	print_packet_data(slot_selected_payload);
	NS_LOG_INFO("Sending FINAL transaction slot action " << action << " (1 add, 2 del, 3 mv) packet for mac = " << BROADCAST_MAC_ADDRESS << " , our mac address is " << this->own_mac_address);

	Ptr<Packet> slot_selected_payload_packet = Create<Packet> (reinterpret_cast<const uint8_t*> (slot_selected_payload->c_str()) , slot_selected_payload->size());
	Mac48Address to = get_ns3_mac(BROADCAST_MAC_ADDRESS);
	Mac48Address from = get_ns3_mac(this->own_mac_address);

	TDMA_MAC_send(slot_selected_payload_packet, MAC_TDMA_SLOT_TRANSACTION_ACK, 0, to, from, false);
}

//Event 10, if this is an external slot allocation not relevant to this node, just inform AI to add it to its table
#ifndef SLOT_ALLOCATION_EXPOSED_NODE
void MFTDMAMac::TDMA_send_SET_external_slot_allocation_to_AI( mac_address_t sender,mac_address_t receiver , uint8_t channel, uint8_t timeslot ){
#else
	void MFTDMAMac::TDMA_send_SET_external_slot_allocation_to_AI( mac_address_t sender,mac_address_t receiver , uint8_t channel, uint8_t timeslot, SLOT_MODE slot_mode ){
#endif

#ifdef SLOT_ALLOCATION_EXPOSED_NODE
		if(slot_mode == SLOT_MODE::SLOT_MODE_UNKNOWN)
			return;
#endif

	NS_LOG_INFO("Forwarding successful slot allocation response between NODE " << sender << " and NODE " << receiver << " to AI");

	//Create and send a ZMQ message to AI to set the selected slot to remote node from this node
	Internal set_mes;
	set_mes.module = MODULE_AI;
	set_mes.attribute = MESSAGE_SET;
	set_mes.set.attribute = EXTERNAL_SLOT_ALLOCATION;

	External_Slot_Allocation external_slot_allocation_p;

	//Populate the message
	//Copy in the slot proposal container the slots coming in from the incoming packet
	MFTDMA_Slot selected_slot = {channel, timeslot};

	external_slot_allocation_p.slot = selected_slot;

	external_slot_allocation_p.dst_mac = receiver;
	external_slot_allocation_p.src_mac = sender;

#ifdef SLOT_ALLOCATION_EXPOSED_NODE
	external_slot_allocation_p.mode = slot_mode;
#endif

	set_mes.set.external_slot_allocation = external_slot_allocation_p;

	//Send message to AI
	set_mes.transaction_index = m_device->MacAIMessageIndexUpdate();
	m_device->SendMacAIMessage(set_mes);

	NS_LOG_INFO("**** Set External slot allocation command to AI, mark slot as used *****");
}

/**
 * Procedure to remove a slot and relevant functions
 */

//Step 1: AI informs MAC with a SET slot_removal message
//The receiving ZMQ message is received from MAC and as a response we got to step 2

//Step 2: MAC send a packet on the remote node to inform him of the slot deletion
void MFTDMAMac::TDMA_send_remove_slot_packet(mac_address_t remote_node, uint8_t channel, uint8_t slot){

	NS_LOG_INFO("Sending delete slot message to NODE " << remote_node << ", slot channel " << channel << " timeslot " << slot);

	//Create the payload container
	std::string* deleted_slot_payload = new std::string();

	//Create packet header
	del_slot_payload payload;
	payload.initiator=this->own_mac_address;

	//Add response containing the selected slot from AI to packet
	payload.del_slot.channel=channel;
	payload.del_slot.timeslot=slot;

	//Copy payload into std::string buffer
	const char* payload_str = (const char*)&payload;
	deleted_slot_payload->insert(deleted_slot_payload->size(), payload_str, sizeof(del_slot_payload));

	//Check if all is fine
	print_packet_data(deleted_slot_payload);
	NS_LOG_INFO("dest max = " << remote_node << " , our mac address is " << this->own_mac_address);

	Ptr<Packet> deleted_slot_payload_packet = Create<Packet> (reinterpret_cast<const uint8_t*> (deleted_slot_payload->c_str()) , deleted_slot_payload->size());
	Mac48Address to = get_ns3_mac(remote_node);
	Mac48Address from = get_ns3_mac(this->own_mac_address);

	TDMA_MAC_send(deleted_slot_payload_packet, MAC_TDMA_DELETE_SLOT, 0, to, from, true);
}

//Step 3: The remote node receives the packet and informs the remote AI about the slot deletion
void MFTDMAMac::TDMA_send_SET_Slot_removal_to_AI(mac_address_t initiator, uint8_t channel, uint8_t slot){

	NS_LOG_INFO("Forwarding slot deletion command from NODE " << initiator << " and NODE " << this->own_mac_address << " to AI");

	//Create and send a ZMQ message to AI to set the selected slot to remote node from this node
	Internal set_mes;
	set_mes.module = MODULE_AI;
	set_mes.attribute = MESSAGE_SET;
	set_mes.set.attribute = SLOT_REMOVAL;

	Slot_Removal slot_removal_p;

	//Populate the message
	//Copy in the slot proposal container the slots coming in from the incoming packet
	MFTDMA_Slot selected_slot = {channel, slot};

	slot_removal_p.slot = selected_slot;
	slot_removal_p.mac = initiator;

	set_mes.set.slot_removal = slot_removal_p;

	//Send message to AI
	set_mes.transaction_index = m_device->MacAIMessageIndexUpdate();

	//Log this transaction in the lookup table and look later if you got a response
	//ADD this set type , and the transaction id and the set struct to our table
	add_procedure(SLOT_REMOVAL, SLOT_MODE_UNKNOWN, set_mes.transaction_index, set_mes);

	m_device->SendMacAIMessage(set_mes);

	NS_LOG_INFO("**** Set slot deletion command to AI, mark slot as unused *****");
}

//Step 4: When the Set_R response arrives, we send a broadcast protocol packet to all neighboring nodes with DMA_send_transaction_ack_packet
//This is done with the already defined TDMA_send_transaction_ack_packet function

//Step 5: When other nodes hear the Protocol ACK they go ahead and remove the slot also in their AI by issuing a SET external_slot_removal message to AI
#ifndef SLOT_ALLOCATION_EXPOSED_NODE
void MFTDMAMac::TDMA_send_SET_external_slot_removal_to_AI( mac_address_t sender,mac_address_t receiver, uint8_t channel, uint8_t timeslot ){
#else
	void MFTDMAMac::TDMA_send_SET_external_slot_removal_to_AI( mac_address_t sender,mac_address_t receiver, uint8_t channel, uint8_t timeslot, SLOT_MODE slot_mode ){
#endif

#ifdef SLOT_ALLOCATION_EXPOSED_NODE
		if(slot_mode == SLOT_MODE::SLOT_MODE_UNKNOWN)
			return;
#endif

	NS_LOG_INFO("Forwarding external slot removal protocol ACK between NODE " << sender << " and NODE " << receiver << " to AI");

	//Create and send a ZMQ message to AI to delete the advertised slot
	Internal set_mes;
	set_mes.module = MODULE_AI;
	set_mes.attribute = MESSAGE_SET;
	set_mes.set.attribute = EXTERNAL_SLOT_REMOVAL;

	External_Slot_Removal  external_slot_removal_p;

	//Populate the message
	//Copy in the removed slot container the slots coming in from the incoming packet
	MFTDMA_Slot selected_slot = {channel, timeslot};
	external_slot_removal_p.slot = selected_slot;

	external_slot_removal_p.dst_mac = receiver;
	external_slot_removal_p.src_mac = sender;

#ifdef SLOT_ALLOCATION_EXPOSED_NODE
	external_slot_removal_p.mode = slot_mode;
#endif

	set_mes.set.external_slot_removal = external_slot_removal_p;

	//Send message to AI
	set_mes.transaction_index = m_device->MacAIMessageIndexUpdate();
	m_device->SendMacAIMessage(set_mes);

	NS_LOG_INFO("**** Set External slot removal command to AI, mark slot as used *****");
}

void MFTDMAMac::TDMA_send_SET_external_slot_move_to_AI( mac_address_t sender,mac_address_t receiver, uint8_t channel_old, uint8_t timeslot_old, uint8_t channel_new, uint8_t timeslot_new ){

	NS_LOG_INFO("Forwarding external slot move protocol ACK between NODE " << sender << " and NODE " << receiver << " to AI");

	Internal set_mes;
	set_mes.module = MODULE_AI;
	set_mes.attribute = MESSAGE_SET;
	set_mes.set.attribute = EXTERNAL_SLOT_MOVE;

	External_Slot_Move external_slot_move_p;

	//Populate the message
	MFTDMA_Slot test_slot_new = {channel_new, timeslot_new};
	MFTDMA_Slot test_slot_old = {channel_old, timeslot_old};

	external_slot_move_p.old_slot = test_slot_old;
	external_slot_move_p.new_slot = test_slot_new;

	external_slot_move_p.dst_mac = receiver;
	external_slot_move_p.src_mac = sender;

	set_mes.set.external_slot_move = external_slot_move_p;

	//Send message to AI
	set_mes.transaction_index = m_device->MacAIMessageIndexUpdate();
	m_device->SendMacAIMessage(set_mes);

	NS_LOG_INFO("**** Set External slot move command to AI, mark slot as used *****");

}

void MFTDMAMac::TDMA_send_proposed_move_slots_packet(mac_address_t remote_node, uint8_t number_of_proposals, slot_reference old_slot, slot_reference slot_table[] ){

	//Create the payload container
	std::string* slot_moved_payload = new std::string();

	//Find suggested slots and add them to the payload

	//Create packet header
	slot_move_payload payload;
	payload.initiator=this->own_mac_address;
	payload.number_of_suggested_slots=number_of_proposals;
	memset(payload.slot_table, 0, sizeof(payload.slot_table));

	payload.old_slot.channel = old_slot.channel;
	payload.old_slot.timeslot = old_slot.timeslot;

	//Add slots payload
	for(uint8_t i=0; i<number_of_proposals; i++){
		payload.slot_table[i].channel=	slot_table[i].channel;
		payload.slot_table[i].timeslot=	slot_table[i].timeslot;
	}

	//Copy payload into std::string buffer
	const char* payload_str = (const char*)&payload;
	slot_moved_payload->insert(slot_moved_payload->size(), payload_str, sizeof(slot_move_payload));

	//print_packet_data(slot_moved_payload);

	NS_LOG_INFO("Sending MAC_TDMA_MOVE_SLOT packet to mac = " << remote_node << " , our mac address is " << payload.initiator);

	Ptr<Packet> slot_moved_payload_packet = Create<Packet> (reinterpret_cast<const uint8_t*> (slot_moved_payload->c_str()) , slot_moved_payload->size());
	Mac48Address to = get_ns3_mac(remote_node);
	Mac48Address from = get_ns3_mac(this->own_mac_address);

	TDMA_MAC_send(slot_moved_payload_packet, MAC_TDMA_MOVE_SLOT, 0, to, from, true);
}

//ALWAYS on start of MAC, send the BC slots allocation to AI


void MFTDMAMac::TDMA_send_SET_BC_slot_allocation_to_AI( std::pair<uint8_t, slot_reference*> br_slots){

	//Set maximum packets per second allowed from upper layers
    this->max_packets_per_second = superframe->get_table_sframe_duration() / supeframe_num_slots - GUARD_MS_COUNT - TDMA_ACK_QUEUE_SIZE;

#ifndef STATIC_TABLE_TYPE
  NS_LOG_INFO("We are NODE: " << this->own_mac_address << ", sending BC slots to AI");

	//Create and send a ZMQ message to AI to ask for selected slot to this node from node with mac_addr_v
	Internal set_mes;

	set_mes.module = MODULE_AI;
	set_mes.attribute = MESSAGE_SET;

	BC_Slots_Announcement bc_slots_p;

	for(uint8_t i=0; i<br_slots.first; i++){
		MFTDMA_Slot tmp_slot;
		tmp_slot.frequency_num = br_slots.second[i].channel;
		tmp_slot.timeslot_num = br_slots.second[i].timeslot;
		bc_slots_p.slots.push_back(tmp_slot);
		NS_LOG_INFO("Slot " << (uint16_t)i << ": channel " << tmp_slot.frequency_num << ", slot number " << tmp_slot.timeslot_num);
	}

	set_mes.set.attribute = BC_SLOTS_ANNOUNCEMENT;
	set_mes.set.bc_slots_announcement = bc_slots_p;

	//Send message to AI
	set_mes.transaction_index = m_device->MacAIMessageIndexUpdate();

	//save the transactions as this is a set
	add_procedure(BC_SLOTS_ANNOUNCEMENT, SLOT_MODE_UNKNOWN, set_mes.transaction_index, set_mes);

	m_device->SendMacAIMessage(set_mes);

	NS_LOG_INFO("**** Sent BC Slot to AI *****");
#endif
}

//Function to prepare and send out our table . This should happen periodically so that we fix possible misalignment in nodes tables
void MFTDMAMac::TDMA_send_slot_table_usage_packet(){

	//Create the payload container
	std::string* slot_selected_payload = new std::string();

	//Create packet header
	slot_table_advertisement_payload payload;
	payload.table_owner=this->own_mac_address;
	memset(payload.list_slots, 0, sizeof(payload.list_slots));

	std::vector<std::pair<channel_t, slot_number_t>> slots;
#ifdef SLOT_ALLOCATION_EXPOSED_NODE
	std::vector<SLOT_MODE> slots_mode;
#endif

#ifdef ENABLE_IMPROVED_NEIGHBOR_REPORT
#ifndef SLOT_ALLOCATION_EXPOSED_NODE
	superframe->table_get_active_slots(slots);
#else
	superframe->table_get_active_slots_mode(slots, slots_mode);
#endif

	if(true){
#else
#ifndef SLOT_ALLOCATION_EXPOSED_NODE
	if(superframe->table_get_active_slots(slots)){
#else
	if(superframe->table_get_active_slots_mode(slots, slots_mode)){
#endif
#endif

		NS_LOG_INFO(" We are using " << slots.size() << " RX-TX slots");

		payload.number_of_used_slots=slots.size();

		for(uint8_t i=0; i<slots.size(); i++){
			payload.list_slots[i].channel=slots[i].first;
			payload.list_slots[i].timeslot=slots[i].second;
#ifdef SLOT_ALLOCATION_EXPOSED_NODE
			payload.list_slots_mode[i]=slots_mode[i];
#endif
		}
		//Copy payload into std::string buffer
		const char* payload_str = (const char*)&payload;
		slot_selected_payload->insert(slot_selected_payload->size(), payload_str, sizeof(slot_table_advertisement_payload));

		//Check if all is fine
		print_packet_data(slot_selected_payload);
		NS_LOG_INFO("Sending Table advertisement broadcast, our mac address is " << this->own_mac_address);

		Ptr<Packet> slot_selected_payload_packet = Create<Packet> (reinterpret_cast<const uint8_t*> (slot_selected_payload->c_str()) , slot_selected_payload->size());
		Mac48Address to = get_ns3_mac(BROADCAST_MAC_ADDRESS);
		Mac48Address from = get_ns3_mac(this->own_mac_address);

		TDMA_MAC_send(slot_selected_payload_packet, MAC_TDMA_ADVERTISE_SLOT_TABLE, 0, to, from, false);
	}

	delete slot_selected_payload;
}


}
