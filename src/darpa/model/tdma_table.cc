/*
 * tdma_table.cpp
 *
 *  Created on: Sep 21, 2017
 *      Author: Irfan Jabandzic
 */

#include "tdma_table.h"
#include "mftdma-mac.h"
#include "ns3/log.h"
#include "ns3/simulator.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TDMA_table");

NS_OBJECT_ENSURE_REGISTERED (TDMA_table);

#define printf(...)

TypeId
TDMA_table::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TDMA_table")
    .SetParent<Object> ()
    .SetGroupName("Darpa")
    .AddConstructor<TDMA_table> ()
    .AddTraceSource ("TxSlotsCount",
                     "Tx slots count.",
                     MakeTraceSourceAccessor (&TDMA_table::send_slots_active),
                     "ns3::TracedValueCallback::Uint8")
		.AddTraceSource ("RxSlotsCount",
										 "Rx slots count.",
										 MakeTraceSourceAccessor (&TDMA_table::receive_slots_active),
										 "ns3::TracedValueCallback::Uint8")
		.AddTraceSource ("TimeoutDropCount",
										 "Timed out queue packets count.",
										 MakeTraceSourceAccessor (&TDMA_table::timeout_drop_count),
										 "ns3::TracedValueCallback::Uint32")
  ;
  return tid;
}

TDMA_table::TDMA_table() : m_mac(0),
		own_mac_address(0), table_channel_count(0), table_slot_count(0),
		table_sframe_duration(0), min_mcs(0), table_current_pos(0)
{
  NS_LOG_FUNCTION (this);
}

void TDMA_table::TDMA_table_init(mac_address_t own_mac_address,
					   uint8_t channel_count = TDMA_TABLE_CHANNELS,
					   uint8_t slot_count = SUPERFRAME_DURATION/TDMA_SLOT_DURATION,
					   uint16_t sframe_duration = SUPERFRAME_DURATION,
					   uint8_t table_min_mcs = 0)
{
  NS_LOG_FUNCTION (this);

	this->own_mac_address = own_mac_address;
	this->table_channel_count = channel_count;
	this->table_slot_count = slot_count;
	this->table_sframe_duration = sframe_duration;
	this->min_mcs = table_min_mcs;

	for(uint8_t i = 0; i < table_channel_count; i++)
	{
		for(uint8_t j = 0; j < table_slot_count; j++)
		{
			table[std::make_pair(i, j)] = CreateObject<TDMA_slot> ();
			table[std::make_pair(i, j)]->TDMA_slot_init(table_sframe_duration/table_slot_count, this->min_mcs);
		}
	}
    NS_LOG_DEBUG("Superframe created with duration: " << table_sframe_duration << ", number of slots: " << (uint16_t)table_slot_count << ", number of channels: " << (uint16_t)table_channel_count << ", slot_size: " << table_sframe_duration/table_slot_count << ", min MCS " << (uint16_t)this->min_mcs << " and own_mac_address: " << this->own_mac_address << "!");

    broadcast_slots_count=0;
    send_slots_active=0;
    receive_slots_active=0;
    timeout_drop_count=0;

#ifdef USE_QUEUES
    table_queues[BROADCAST_MAC_ADDRESS] = CreateObject<TDMA_queue> ();
    table_queues[BROADCAST_MAC_ADDRESS]->TDMA_queue_init(TDMA_queue::BROADCAST_CONTROL, table_sframe_duration * 1000000, own_mac_address, BROADCAST_MAC_ADDRESS);
#ifdef STATIC_TABLE_TYPE

    mac_address_t test_remote;
    if(own_mac_address == TEST_OWN_MAC_ADDRESS)
    	test_remote = TEST_REMOTE_MAC_ADDRESS;
    else
  		test_remote = TEST_OWN_MAC_ADDRESS;

    table_queues[test_remote] = CreateObject<TDMA_queue> ();
    table_queues[test_remote]->TDMA_queue_init(TDMA_queue::UNICAST, table_sframe_duration * 1000000, own_mac_address, test_remote);
#endif
#endif

#ifndef USE_SLOTTED_ALOHA
    table_generate_specific_values();
#else
    //ntable = std::unique_ptr<neighbour_table>(new neighbour_table());
    ntable = CreateObject<neighbour_table> ();
    ntable->neighbour_table_init();
#endif
}

Ptr<MFTDMAMac>
TDMA_table::GetMac (void) const
{
  NS_LOG_FUNCTION (this);
  return m_mac;
}
void
TDMA_table::SetMac (Ptr<MFTDMAMac> mac)
{
  NS_LOG_FUNCTION (this << mac);
  m_mac = mac;
}

void TDMA_table::set_channel_count(uint8_t channel_count)
{
	table.clear();

	for(uint8_t i = 0; i < channel_count; i++)
	{
		for(uint8_t j = 0; j < table_slot_count; j++)
		{
			table[std::make_pair(i, j)] = CreateObject<TDMA_slot> ();
			table[std::make_pair(i, j)]->TDMA_slot_init(table_sframe_duration/table_slot_count, this->min_mcs);
		}
	}

  NS_LOG_DEBUG("Number of channels modified from " << table_channel_count << " to " << channel_count << ".");
	table_channel_count = channel_count;
}

TDMA_table::table_operation_status TDMA_table::set_slot_count(uint8_t slot_count)
{
	uint16_t slot_size = table_sframe_duration/table_slot_count;

	if(slot_count > table_slot_count)
	{
		for(uint8_t i = 0; i < table_channel_count; i++)
		{
			for(uint8_t j = table_slot_count; j < slot_count; j++)
			{
				table[std::make_pair(i, j)] = CreateObject<TDMA_slot> ();
				table[std::make_pair(i, j)]->TDMA_slot_init(slot_size, this->min_mcs);
			}
		}
	}
	else if(slot_count < table_slot_count)
	{
		for(uint8_t i = 0; i < table_channel_count; i++)
		{
			for(uint8_t j = slot_count; j < table_slot_count; j++)
				table.erase(std::make_pair(i, j));
		}
	}

  NS_LOG_DEBUG("Superframe duration modified from " << table_sframe_duration << " to " << slot_size * slot_count << ". Number of slots modified from " << table_slot_count << " to " << slot_count << ".");
	table_sframe_duration = slot_size * slot_count;
	table_slot_count = slot_count;

#ifndef USE_SLOTTED_ALOHA
    table_generate_specific_values();
#endif

	return SUCCESS;
}

TDMA_table::table_operation_status TDMA_table::set_sframe_duration(uint16_t sframe_duration)
{
	uint16_t slot_size = table_sframe_duration/table_slot_count;

	table_operation_status status = check_if_divisible(sframe_duration, slot_size);

	if(status != SUCCESS)
	{
		NS_LOG_DEBUG("Superframe duration not modified. New superframe duration " << sframe_duration << " not divisible with slot size " << slot_size << ".");
		return status;
	}

	if(sframe_duration > table_sframe_duration)
	{
		for(uint8_t i = 0; i < table_channel_count; i++)
		{
			for(uint8_t j = table_slot_count; j < (sframe_duration/slot_size); j++)
			{
				table[std::make_pair(i, j)] = CreateObject<TDMA_slot> ();
				table[std::make_pair(i, j)]->TDMA_slot_init(slot_size, this->min_mcs);
			}
		}
	}
	else if(sframe_duration < table_sframe_duration)
	{
		for(uint8_t i = 0; i < table_channel_count; i++)
		{
			for(uint8_t j = (sframe_duration/slot_size); j < table_slot_count; j++)
				table.erase(std::make_pair(i, j));
		}
	}

	NS_LOG_DEBUG("Superframe duration modified from " << table_sframe_duration << " to " << sframe_duration << ". Number of slots modified from " << table_slot_count << " to " << sframe_duration/slot_size << ".");
	table_sframe_duration = sframe_duration;
	table_slot_count = sframe_duration/slot_size;

#ifndef USE_SLOTTED_ALOHA
    table_generate_specific_values();
#endif

	return status;
}

TDMA_table::table_operation_status TDMA_table::set_slot_count_fixed(uint8_t slot_count)
{
	table_operation_status status = check_if_divisible(table_sframe_duration, slot_count);

	if(status != SUCCESS)
	{
		NS_LOG_DEBUG("Number of slots not modified. Superframe duration " << table_sframe_duration << " not divisible with new number of slots " << slot_count << ".");
		return status;
	}

	uint16_t slot_size = table_sframe_duration/slot_count;

	table.clear();

	for(uint8_t i = 0; i < table_channel_count; i++)
	{
		for(uint8_t j = 0; j < slot_count; j++)
			table[std::make_pair(i, j)]->set_slot_size(slot_size);
	}

	NS_LOG_DEBUG("Number of slots modified from " << table_slot_count << " to " << slot_count << ".");
	table_slot_count = slot_count;

#ifndef USE_SLOTTED_ALOHA
    table_generate_specific_values();
#endif

	return status;
}

TDMA_table::table_operation_status TDMA_table::set_sframe_duration_fixed(uint16_t sframe_duration)
{
	table_operation_status status = check_if_divisible(sframe_duration, table_slot_count);

	if(status != SUCCESS)
	{
		NS_LOG_DEBUG("Superframe duration not modified. New superframe duration " << sframe_duration << " not divisible with number of slots " << table_slot_count << ".");
		return status;
	}

	uint16_t slot_size = sframe_duration/table_slot_count;

	for(uint8_t i = 0; i < table_channel_count; i++)
	{
		for(uint8_t j = 0; j < table_slot_count; j++)
			table[std::make_pair(i, j)]->set_slot_size(slot_size);
	}

	NS_LOG_DEBUG("Superframe duration modified from " << table_sframe_duration << " to " << sframe_duration << ".");
	table_sframe_duration = sframe_duration;

#ifndef USE_SLOTTED_ALOHA
    table_generate_specific_values();
#endif

	return status;
}

TDMA_table::table_operation_status TDMA_table::add_slot(channel_t channel, slot_number_t slot_number, TDMA_slot::tdma_slot_type slot_type, mac_address_t remote1 , mac_address_t remote2)
{
	if(remote2==0)
		remote2=this->own_mac_address;

	table_operation_status status = check_channel_and_slot_number(channel, slot_number);

	if(status != SUCCESS)
	{
		if(status == CHANNEL_OUT_OF_BOUNDS)
			NS_LOG_ERROR("Add slot operation failed. Channel number " << (uint16_t)channel << " is out of bounds.");
		else if(status == SLOT_OUT_OF_BOUNDS)
			NS_LOG_ERROR("Add slot operation failed. Slot number " << (uint16_t)slot_number << " is out of bounds.");

		return status;
	}

	if(table[std::make_pair(channel, slot_number)]->get_slot_type() != TDMA_slot::UNUSED)
	{
		NS_LOG_ERROR("Add slot operation failed. Slot at channel " << (uint16_t)channel << " and slot number " << (uint16_t)slot_number << " is already in use.");
		status = SLOT_USED;
	}
	else
	{
		switch (slot_type) {
		case TDMA_slot::UNUSED:
			table[std::make_pair(channel, slot_number)]->remove_slot();
			NS_LOG_DEBUG("Add slot succeed. Slot at channel " << (uint16_t)channel << " and slot number " << (uint16_t)slot_number << " set to UNUSED.");
			break;
		case TDMA_slot::USED:
			table[std::make_pair(channel, slot_number)]->set_used_slot(remote1,remote2);
			NS_LOG_DEBUG("Add used slot succeed. Slot at channel " << (uint16_t)channel << " and slot number " << (uint16_t)slot_number << " set to USED.");
			break;
		case TDMA_slot::BLOCKED:
			table[std::make_pair(channel, slot_number)]->block_slot();
			NS_LOG_DEBUG("Add slot succeed. Slot at channel " << (uint16_t)channel << " and slot number " << (uint16_t)slot_number << " set to BLOCKED.");
			break;
		case TDMA_slot::CONTROL_BROADCAST:
			table[std::make_pair(channel, slot_number)]->set_control_broadcast_slot(own_mac_address);
			NS_LOG_DEBUG("Add slot succeed. Slot at channel " << (uint16_t)channel << " and slot number " << (uint16_t)slot_number << " set to CONTROL_BROADCAST.");
			break;
		case TDMA_slot::RECEIVE:
			table[std::make_pair(channel, slot_number)]->set_receive_slot(remote2, remote1);
			this->receive_slots_active++;
			NS_LOG_DEBUG("Add slot succeed. Slot at channel " << (uint16_t)channel << " and slot number " << (uint16_t)slot_number << " set to RECEIVE. Remote mac address is " << remote1 << ".");
			break;
		case TDMA_slot::UNICAST_SEND:
			table[std::make_pair(channel, slot_number)]->set_unicast_send_slot(remote2, remote1);
			//Count active receive slots
			this->send_slots_active++;
			NS_LOG_DEBUG("Add slot succeed " << (uint16_t)this->send_slots_active << " send slots active. Slot at channel " << (uint16_t)channel << " and slot number " << (uint16_t)slot_number << " set to UNICAST_SEND. Remote mac address is " << remote1 << ".");
			break;
		default:
			status = WRONG_SLOT_TYPE;
		}
	}
	//Print the table to see the differences
	//TDMA_table::print_table_info(); //TODO uncomment this
	return status;
}

TDMA_table::table_operation_status TDMA_table::update_slot(channel_t channel, slot_number_t slot_number, TDMA_slot::tdma_slot_type slot_type, mac_address_t remote1, mac_address_t remote2)
{
	if(remote2==0)
			remote2=this->own_mac_address;

	table_operation_status status = check_channel_and_slot_number(channel, slot_number);

	if(status != SUCCESS)
	{
		if(status == CHANNEL_OUT_OF_BOUNDS)
			NS_LOG_ERROR("Update slot operation failed. Channel number " << channel << " is out of bounds.");
		else if(status == SLOT_OUT_OF_BOUNDS)
			NS_LOG_ERROR("Update slot operation failed. Slot number " << slot_number << " is out of bounds.");

		return status;
	}

	if(table[std::make_pair(channel, slot_number)]->get_slot_type() == TDMA_slot::UNUSED)
	{
		status = SLOT_UNUSED;
		NS_LOG_ERROR("Update slot operation failed. Slot at channel " << channel << " and slot number " << slot_number << " is unused.");
	}
	else
	{
		switch (slot_type) {
		case TDMA_slot::UNUSED:
			table[std::make_pair(channel, slot_number)]->remove_slot();
			NS_LOG_DEBUG("Update slot succeed. Slot at channel " << channel << " and slot number " << slot_number << " set to UNUSED.");
			break;
		case TDMA_slot::USED:
			table[std::make_pair(channel, slot_number)]->remove_slot();
			table[std::make_pair(channel, slot_number)]->set_used_slot(remote2, remote1);
			NS_LOG_DEBUG("Update slot succeed. Slot at channel " << channel << " and slot number " << slot_number << " set to USED. Remote mac address is " << remote1 << ".");
			break;
		case TDMA_slot::BLOCKED:
			table[std::make_pair(channel, slot_number)]->block_slot();
			NS_LOG_DEBUG("Update slot succeed. Slot at channel " << channel << " and slot number " << slot_number << " set to BLOCKED.");
			break;
		case TDMA_slot::CONTROL_BROADCAST:
			table[std::make_pair(channel, slot_number)]->remove_slot();
			table[std::make_pair(channel, slot_number)]->set_control_broadcast_slot(remote2);
			NS_LOG_DEBUG("Update slot succeed. Slot at channel " << channel << " and slot number " << slot_number << " set to CONTROL_BROADCAST.");
			break;
		case TDMA_slot::RECEIVE:
			table[std::make_pair(channel, slot_number)]->remove_slot();
			table[std::make_pair(channel, slot_number)]->set_receive_slot(remote2, remote1);
			NS_LOG_DEBUG("Update slot succeed. Slot at channel " << channel << " and slot number " << slot_number << " set to RECEIVE. Remote mac address is " << remote1 << ".");
			break;
		case TDMA_slot::UNICAST_SEND:
			table[std::make_pair(channel, slot_number)]->remove_slot();
			table[std::make_pair(channel, slot_number)]->set_unicast_send_slot(remote2, remote1);
			NS_LOG_DEBUG("Update slot succeed. Slot at channel " << channel << " and slot number " << slot_number << " set to UNICAST_SEND. Remote mac address is " << remote1 << ".");
			break;
		default:
			status = WRONG_SLOT_TYPE;
		}
	}

	return status;
}

TDMA_table::table_operation_status TDMA_table::remove_slot(channel_t channel, slot_number_t slot_number)
{
	table_operation_status status = check_channel_and_slot_number(channel, slot_number);

	if(status != SUCCESS)
	{
		if(status == CHANNEL_OUT_OF_BOUNDS)
			NS_LOG_ERROR("Remove slot operation failed. Channel number " << (uint16_t)channel << " is out of bounds.");
		else if(status == SLOT_OUT_OF_BOUNDS)
			NS_LOG_ERROR("Remove slot operation failed. Slot number " << (uint16_t)slot_number << " is out of bounds.");

		return status;
	}

	if(table[std::make_pair(channel, slot_number)]->get_slot_type() == TDMA_slot::UNUSED)
	{
		NS_LOG_ERROR("Remove slot operation failed. Slot at channel " << (uint16_t)channel << " and slot number " << (uint16_t)slot_number << " is already not in use.");
		status = SLOT_UNUSED;
	}
	else
	{
		//Check if its a send slot
		if(table[std::make_pair(channel, slot_number)]->get_slot_type() == TDMA_slot::UNICAST_SEND){
			//Count active receive slots
			this->send_slots_active--;
		}

		//Check if its a send slot
		if(table[std::make_pair(channel, slot_number)]->get_slot_type() == TDMA_slot::RECEIVE){
			//Count active receive slots
			this->receive_slots_active--;
		}

		NS_LOG_INFO("Remove slot operation succeeded " << (uint16_t)this->send_slots_active << " send slots remaining. Slot at channel " << (uint16_t)channel << " and slot number " << (uint16_t)slot_number << " is removed.");

		//Now remove the slot whatever it is
		table[std::make_pair(channel, slot_number)]->remove_slot();
	}
	//Print the table to see the differences
	//TDMA_table::print_table_info(); //TODO uncomment this

	return status;
}

bool TDMA_table::slot_exists_to(mac_address_t destination){

	for(uint8_t i = 0; i < table_channel_count; i++){
		for(uint8_t j = 0; j < table_slot_count; j++){

			if(table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::UNICAST_SEND)
					if(table[std::make_pair(i, j)]->get_remote_mac_address()==destination)
						return true;
		}
	}

return false;
}

bool TDMA_table::check_specific_slot_exists_to(mac_address_t destination, uint8_t channel, uint8_t slot){

	if(table[std::make_pair(channel, slot)]->get_slot_type() == TDMA_slot::UNICAST_SEND)
		if(table[std::make_pair(channel, slot)]->get_remote_mac_address()==destination)
			return true;

return false;
}

bool TDMA_table::check_specific_slot_exists_from(mac_address_t remote, uint8_t channel, uint8_t slot){

	if(table[std::make_pair(channel, slot)]->get_slot_type() == TDMA_slot::RECEIVE)
		if(table[std::make_pair(channel, slot)]->get_remote_mac_address()==remote)
			return true;

return false;
}

TDMA_table::table_operation_status TDMA_table::test_table()
{
	NS_LOG_DEBUG("Starting test superframe function.");

	table_operation_status status;

	set_channel_count(15);
	set_channel_count(10);
	status = set_slot_count(10);
	status = set_slot_count(20);
	status = set_slot_count_fixed(10);
	status = set_slot_count_fixed(20);
	status = set_sframe_duration(1200);
	status = set_sframe_duration(1000);
	status = set_sframe_duration_fixed(1200);
	status = set_sframe_duration_fixed(1000);
	status = add_slot(10, 15, TDMA_slot::CONTROL_BROADCAST);
	status = add_slot(0, 20, TDMA_slot::CONTROL_BROADCAST);
	status = update_slot(0, 0, TDMA_slot::CONTROL_BROADCAST);
	status = add_slot(0, 0, TDMA_slot::CONTROL_BROADCAST);
	status = update_slot(0, 0, TDMA_slot::RECEIVE, 10);
	status = remove_slot(1, 1);
	status = remove_slot(0, 0);
	status = update_slot(0, 0, TDMA_slot::CONTROL_BROADCAST);
	status = add_slot(0, 0, TDMA_slot::CONTROL_BROADCAST);
	status = update_slot(0, 0, TDMA_slot::RECEIVE, 10);

	return status;
}

void TDMA_table::table_align()
{
	while(((clock_get_time_ns()/1000) % (table_sframe_duration*1000 + ALLIGN_OFFSET)) != 0);

	NS_LOG_DEBUG("Superframe aligned to system time.");
}

int32_t TDMA_table::table_check_drift()
{
	int32_t drift = clock_get_time_ms() % table_sframe_duration;
	if(drift < table_sframe_duration / 2)
	{
		NS_LOG_LOGIC("Superframe drift " << drift << ".");
		return drift;
	}
	else
	{
		NS_LOG_LOGIC("Superframe drift " << drift - table_sframe_duration << ".");
		return drift - table_sframe_duration;
	}
}

void TDMA_table::update_table_current_position(uint64_t value)
{
#ifdef USE_TABLE_COUNTER
	table_current_pos = clock_get_time_ms() + ROUND_TX_MS_TO_HIGHER_VALUE + TX_OFFSET_MS + value;
#endif
}

TDMA_table::table_operation_status TDMA_table::table_get_next_rx_wkup(rx_timer_container &rx_container, channel_t &rx_channel)
{
	table_operation_status status = NO_SLOTS_AVAILABLE;

	uint8_t slot_number = (table_get_slot_number() + 1) % table_slot_count;
	printf("slot_number_rx: %d\n", slot_number);
	uint16_t ms_in_slot = table_get_ms_in_slot();
	printf("ms in slot is:%d\n", ms_in_slot);
	uint16_t slot_size = table_sframe_duration/table_slot_count;

	uint8_t i, j;
	uint8_t next_rx_slot_number = 0;

	//Check for BROADCAST slots
	for(i = 0; i < table_channel_count; i++)
	{
		if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::CONTROL_BROADCAST && i != rx_channel)
		{
			status = SUCCESS;
			rx_container.rx_number_of_changes = 1;
			rx_container.rx_first_change = slot_size - ms_in_slot - RX_TIMER_OFFSET_MS; //try with zero also
			rx_container.next_rx_wkup = (slot_size - ms_in_slot - RX_TIMER_OFFSET_MS) + slot_size;
			rx_container.rx_first_channel = i;
			rx_channel = i;
			break;
		}
	}

	if(status == SUCCESS)
	{
		NS_LOG_DEBUG("Next BROADCAST_rx slot number " << slot_number << " on channel " << rx_container.rx_first_channel << ".");
		return status;
	}

	//Check for RECEIVE slots
	for(i = 0; i < table_channel_count; i++)
	{
		if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::RECEIVE && i != rx_channel)
		{
			status = SUCCESS;
			rx_container.rx_number_of_changes = 1;
			rx_container.rx_first_change = slot_size - ms_in_slot - RX_TIMER_OFFSET_MS; //try with zero also
			rx_container.next_rx_wkup = (slot_size - ms_in_slot - RX_TIMER_OFFSET_MS) + slot_size;
			rx_container.rx_first_channel = i;
			rx_channel = i;
			break;
		}
	}

	//Check for UNICAST_SEND slots
	if(status == SUCCESS)
	{
		status = NO_SLOTS_AVAILABLE;
		NS_LOG_DEBUG("Next RECEIVE_rx slot number " << slot_number << " on channel " << rx_container.rx_first_channel << ".");
		for(i = 0; i < table_channel_count; i++)
		{
			if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::UNICAST_SEND)
			{
				status = SUCCESS;
				rx_container.rx_number_of_changes = 2;
				rx_container.rx_second_change = (slot_size - ms_in_slot - RX_TIMER_OFFSET_MS) + slot_size - TDMA_ACK_QUEUE_SIZE;
				rx_container.rx_second_channel = i;
				break;
			}
		}
		if(status == SUCCESS)
			NS_LOG_DEBUG("Next UNICAST_SEND_rx slot number " << slot_number << " on channel " << rx_container.rx_second_channel << ".");
		return SUCCESS;
	}
	else
	{
		for(i = 0; i < table_channel_count; i++)
		{
			if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::UNICAST_SEND && i != rx_channel)
			{
				status = SUCCESS;
				rx_container.rx_number_of_changes = 1;
				rx_container.rx_first_change = (slot_size - ms_in_slot - RX_TIMER_OFFSET_MS) + slot_size - TDMA_ACK_QUEUE_SIZE;
				rx_container.next_rx_wkup = (slot_size - ms_in_slot - RX_TIMER_OFFSET_MS) + slot_size;
				rx_container.rx_first_channel = i;
				rx_channel = i;
				break;
			}
		}
		if(status == SUCCESS)
		{
			NS_LOG_DEBUG("Next UNICAST_SEND_rx slot number " << slot_number << " on channel " << rx_container.rx_first_channel << ".");
			return status;
		}
	}

	//Check for next Rx slots
	for(j = slot_number + 1; j < table_slot_count; j++)
	{
		for(i = 0; i < table_channel_count; i++)
		{
			if((table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::RECEIVE || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::CONTROL_BROADCAST || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::UNICAST_SEND) && i != rx_channel)
			{
				status = SUCCESS;
				next_rx_slot_number = j;
				break;
			}
		}
		if(status == SUCCESS)
			break;
	}

	if(status != SUCCESS)
	{
		for(j = 0; j <= slot_number; j++)
		{
			for(i = 0; i < table_channel_count; i++)
			{
				if((table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::RECEIVE || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::CONTROL_BROADCAST || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::UNICAST_SEND) && i != rx_channel)
				{
					status = SUCCESS;
					next_rx_slot_number = j + table_slot_count;
					break;
				}
			}
			if(status == SUCCESS)
				break;
		}
	}

	if(status != SUCCESS)
		return status;

	rx_container.next_rx_wkup = (slot_size - ms_in_slot - RX_TIMER_OFFSET_MS) + slot_size * (next_rx_slot_number - slot_number);

	return status;
}


TDMA_table::table_operation_status TDMA_table::table_get_next_rx_message_no1(rx_timer_container &rx_container, channel_t &rx_channel)
{
	table_operation_status status = NO_SLOTS_AVAILABLE;

	uint8_t slot_number = (table_get_slot_number() + 1) % table_slot_count;
	printf("slot_number_rx: %d\n", slot_number);
	uint16_t ms_in_slot = table_get_ms_in_slot();
	printf("ms in slot is:%d\n", ms_in_slot);
	uint16_t slot_size = table_sframe_duration/table_slot_count;

	uint8_t next_rx_slot_number = 0;
	uint8_t i, j;

	//Check for BROADCAST slots
	for(i = 0; i < table_channel_count; i++)
	{
		if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::CONTROL_BROADCAST && i != rx_channel)
		{
			status = SUCCESS;
			rx_container.rx_number_of_changes = 1;
			rx_container.rx_first_change = slot_size - ms_in_slot - RX_TIMER_OFFSET_MS; //try with zero also
			rx_container.next_rx_wkup = (slot_size - ms_in_slot - RX_TIMER_OFFSET_MS) + slot_size;
			rx_container.rx_first_channel = i;
			rx_channel = i;

			mac_container_populate_slot_info(rx_container.rx_first_container, rx_channel, slot_number, table[std::make_pair(i, slot_number)]->get_slot_type());

			mac_container_populate_ctrl_info(rx_container.rx_first_container, RX, rx_channel,
																			 slot_size + RX_TIMER_OFFSET_MS - ms_in_slot,
																			 this->min_mcs, DEFAULT_RX_GAIN, 1);

			rx_container.rx_first_container.packets_in_data = 1;

			break;
		}
	}

	if(status == SUCCESS)
	{
		NS_LOG_DEBUG("Next BROADCAST_rx slot number " << slot_number << " on channel " << rx_container.rx_first_channel << ".");
		return status;
	}

	//Check for RECEIVE slots
	for(i = 0; i < table_channel_count; i++)
	{
		if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::RECEIVE && i != rx_channel)
		{
			status = SUCCESS;
			rx_container.rx_number_of_changes = 1;
			rx_container.rx_first_change = slot_size - ms_in_slot - RX_TIMER_OFFSET_MS; //try with zero also
			rx_container.next_rx_wkup = (slot_size - ms_in_slot - RX_TIMER_OFFSET_MS) + slot_size;
			rx_container.rx_first_channel = i;
			rx_channel = i;

			mac_container_populate_slot_info(rx_container.rx_first_container, rx_channel, slot_number, table[std::make_pair(i, slot_number)]->get_slot_type());

			mac_container_populate_ctrl_info(rx_container.rx_first_container, RX, rx_channel,
											 	 	 	 	 	 	 	 	 slot_size + RX_TIMER_OFFSET_MS - ms_in_slot,
																			 this->min_mcs, DEFAULT_RX_GAIN, 1);

			rx_container.rx_first_container.packets_in_data = 1;

			break;
		}
	}
	//Check for UNICAST_SEND slots
	if(status == SUCCESS)
	{
		status = NO_SLOTS_AVAILABLE;
		NS_LOG_DEBUG("Next RECEIVE_rx slot number " << slot_number << " on channel " << rx_container.rx_first_channel << ".");
		for(i = 0; i < table_channel_count; i++)
		{
			if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::UNICAST_SEND)
			{
				status = SUCCESS;
				rx_container.rx_number_of_changes = 2;
				rx_container.rx_second_change = (slot_size - ms_in_slot - RX_TIMER_OFFSET_MS) + slot_size - TDMA_ACK_QUEUE_SIZE;
				rx_container.rx_second_channel = i;

				mac_container_populate_slot_info(rx_container.rx_second_container, rx_container.rx_second_channel, slot_number, table[std::make_pair(i, slot_number)]->get_slot_type());

				mac_container_populate_ctrl_info(rx_container.rx_second_container, RX, rx_container.rx_second_channel,
												 	 	 	 	 	 	 	 	 slot_size + RX_TIMER_OFFSET_MS - ms_in_slot,
																				 this->min_mcs, DEFAULT_RX_GAIN, 1);

				rx_container.rx_second_container.packets_in_data = 1;

				break;
			}
		}
		if(status == SUCCESS)
			NS_LOG_DEBUG("Next UNICAST_SEND_rx slot number " << slot_number << " on channel " << rx_container.rx_second_channel << ".");
		return SUCCESS;
	}
	else
	{
		for(i = 0; i < table_channel_count; i++)
		{
			if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::UNICAST_SEND && i != rx_channel)
			{
				status = SUCCESS;
				rx_container.rx_number_of_changes = 1;
				rx_container.rx_first_change = (slot_size - ms_in_slot - RX_TIMER_OFFSET_MS) + slot_size - TDMA_ACK_QUEUE_SIZE;
				rx_container.next_rx_wkup = (slot_size - ms_in_slot - RX_TIMER_OFFSET_MS) + slot_size;
				rx_container.rx_first_channel = i;
				rx_channel = i;

				mac_container_populate_slot_info(rx_container.rx_first_container, rx_channel, slot_number, table[std::make_pair(i, slot_number)]->get_slot_type());

				mac_container_populate_ctrl_info(rx_container.rx_first_container, RX, rx_channel,
																				 slot_size + RX_TIMER_OFFSET_MS - ms_in_slot,
																				 this->min_mcs, DEFAULT_RX_GAIN, 1);

				rx_container.rx_first_container.packets_in_data = 1;

				break;
			}
		}
		if(status == SUCCESS)
		{
			NS_LOG_DEBUG("Next UNICAST_SEND_rx slot number " << slot_number << " on channel " << rx_container.rx_first_channel << ".");
			return status;
		}
	}
	//Check for next Rx slots
	for(j = slot_number + 1; j < table_slot_count; j++)
	{
		for(i = 0; i < table_channel_count; i++)
		{
			if((table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::RECEIVE || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::CONTROL_BROADCAST || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::UNICAST_SEND) && i != rx_channel)
			{
				status = SUCCESS;
				next_rx_slot_number = j;
				printf("next_rx_slot_number1: %d\n", next_rx_slot_number);
				break;
			}
		}
		if(status == SUCCESS)
			break;
	}

	if(status != SUCCESS)
	{
		for(j = 0; j <= slot_number; j++)
		{
			for(i = 0; i < table_channel_count; i++)
			{
				if((table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::RECEIVE || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::CONTROL_BROADCAST || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::UNICAST_SEND) && i != rx_channel)
				{
					status = SUCCESS;
					next_rx_slot_number = j + table_slot_count;
					printf("next_rx_slot_number2: %d\n", next_rx_slot_number);
					break;
				}
			}
			if(status == SUCCESS)
				break;
		}
	}

	if(status != SUCCESS)
		return status;

	rx_container.next_rx_wkup = (slot_size - ms_in_slot - RX_TIMER_OFFSET_MS) + slot_size * (next_rx_slot_number - slot_number);

	return status;
}

TDMA_table::table_operation_status TDMA_table::table_get_next_rx_message_no2(rx_timer_container &rx_container, channel_t &rx_channel)
{
	table_operation_status status = NO_SLOTS_AVAILABLE;

	uint8_t slot_number = table_get_slot_number();
	printf("slot_number_rx: %d\n", slot_number);
	uint16_t ms_in_slot = table_get_ms_in_slot();
	printf("ms in slot is:%d\n", ms_in_slot);
	uint16_t slot_size = table_sframe_duration/table_slot_count;

	uint8_t next_rx_slot_number = 0;
	uint8_t i, j;

	//Check for BROADCAST slots
	for(i = 0; i < table_channel_count; i++)
	{
		if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::CONTROL_BROADCAST && i != rx_channel)
		{
			status = SUCCESS;
			rx_container.rx_number_of_changes = 1;
			rx_container.rx_first_change = 0; //1 for PE1
			rx_container.next_rx_wkup = slot_size - ms_in_slot + RX_TIMER_OFFSET_MS;
			rx_container.rx_first_channel = i;
			rx_channel = i;

			mac_container_populate_slot_info(rx_container.rx_first_container, rx_channel, slot_number, table[std::make_pair(i, slot_number)]->get_slot_type());

			mac_container_populate_ctrl_info(rx_container.rx_first_container, RX, rx_channel,
																			 slot_size + RX_TIMER_OFFSET_MS - ms_in_slot,
																			 this->min_mcs, DEFAULT_RX_GAIN, 1);

			rx_container.rx_first_container.packets_in_data = 1;

			break;
		}
	}

	if(status == SUCCESS)
	{
		NS_LOG_DEBUG("Next BROADCAST_rx slot number " << slot_number << " on channel " << rx_container.rx_first_channel << ".");
		return status;
	}

	//Check for RECEIVE slots
	for(i = 0; i < table_channel_count; i++)
	{
		if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::RECEIVE && i != rx_channel)
		{
			status = SUCCESS;
			rx_container.rx_number_of_changes = 1;
			rx_container.rx_first_change = 0; //1 for PE1
			rx_container.next_rx_wkup = slot_size - ms_in_slot + RX_TIMER_OFFSET_MS;
			rx_container.rx_first_channel = i;
			rx_channel = i;

			mac_container_populate_slot_info(rx_container.rx_first_container, rx_channel, slot_number, table[std::make_pair(i, slot_number)]->get_slot_type());

			mac_container_populate_ctrl_info(rx_container.rx_first_container, RX, rx_channel,
																			 slot_size + RX_TIMER_OFFSET_MS - ms_in_slot,
																			 this->min_mcs, DEFAULT_RX_GAIN, 1);

			rx_container.rx_first_container.packets_in_data = 1;

			break;
		}
	}
	//Check for UNICAST_SEND slots
	if(status == SUCCESS)
	{
		status = NO_SLOTS_AVAILABLE;
		NS_LOG_DEBUG("Next RECEIVE_rx slot number " << slot_number << " on channel " << rx_container.rx_first_channel << ".");
		for(i = 0; i < table_channel_count; i++)
		{
			if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::UNICAST_SEND)
			{
				status = SUCCESS;
				rx_container.rx_number_of_changes = 2;
				rx_container.rx_second_change = slot_size - ms_in_slot + RX_TIMER_OFFSET_MS - TDMA_ACK_QUEUE_SIZE; //(slot_size - ms_in_slot - RX_TIMER_OFFSET_MS) - TDMA_ACK_QUEUE_SIZE;
				rx_container.rx_second_channel = i;

				mac_container_populate_slot_info(rx_container.rx_second_container, rx_container.rx_second_channel, slot_number, table[std::make_pair(i, slot_number)]->get_slot_type());

				mac_container_populate_ctrl_info(rx_container.rx_second_container, RX, rx_container.rx_second_channel,
																				 slot_size + RX_TIMER_OFFSET_MS - ms_in_slot,
																				 this->min_mcs, DEFAULT_RX_GAIN, 1);

				rx_container.rx_second_container.packets_in_data = 1;

				break;
			}
		}
		if(status == SUCCESS)
			NS_LOG_DEBUG("Next UNICAST_SEND_rx slot number " << slot_number << " on channel " << rx_container.rx_second_channel << ".");
		return SUCCESS;
	}
	else
	{
		for(i = 0; i < table_channel_count; i++)
		{
			if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::UNICAST_SEND && i != rx_channel)
			{
				status = SUCCESS;
				rx_container.rx_number_of_changes = 1;
				rx_container.rx_first_change = slot_size - ms_in_slot + RX_TIMER_OFFSET_MS - TDMA_ACK_QUEUE_SIZE; //(slot_size - ms_in_slot - RX_TIMER_OFFSET_MS) - TDMA_ACK_QUEUE_SIZE;
				rx_container.next_rx_wkup = slot_size - ms_in_slot + RX_TIMER_OFFSET_MS;
				rx_container.rx_first_channel = i;
				rx_channel = i;

				mac_container_populate_slot_info(rx_container.rx_first_container, rx_channel, slot_number, table[std::make_pair(i, slot_number)]->get_slot_type());

				mac_container_populate_ctrl_info(rx_container.rx_first_container, RX, rx_channel,
																				 slot_size + RX_TIMER_OFFSET_MS - ms_in_slot,
																				 this->min_mcs, DEFAULT_RX_GAIN, 1);

				rx_container.rx_first_container.packets_in_data = 1;

				break;
			}
		}
		if(status == SUCCESS)
		{
			NS_LOG_DEBUG("Next UNICAST_SEND_rx slot number " << slot_number << " on channel " << rx_container.rx_first_channel << ".");
			return status;
		}
	}
	//Check for next Rx slots
	for(j = slot_number + 1; j < table_slot_count; j++)
	{
		for(i = 0; i < table_channel_count; i++)
		{
			if((table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::RECEIVE || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::CONTROL_BROADCAST || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::UNICAST_SEND) && i != rx_channel)
			{
				status = SUCCESS;
				next_rx_slot_number = j;
				printf("next_rx_slot_number1: %d\n", next_rx_slot_number);
				break;
			}
		}
		if(status == SUCCESS)
			break;
	}

	if(status != SUCCESS)
	{
		for(j = 0; j <= slot_number; j++)
		{
			for(i = 0; i < table_channel_count; i++)
			{
				if((table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::RECEIVE || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::CONTROL_BROADCAST || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::UNICAST_SEND) && i != rx_channel)
				{
					status = SUCCESS;
					next_rx_slot_number = j + table_slot_count;
					printf("next_rx_slot_number2: %d\n", next_rx_slot_number);
					break;
				}
			}
			if(status == SUCCESS)
				break;
		}
	}

	if(status != SUCCESS)
		return status;

	rx_container.next_rx_wkup = slot_size * (next_rx_slot_number - slot_number);

	return status;
}

TDMA_table::table_operation_status TDMA_table::table_get_next_tx_wkup(uint16_t &next_tx_wkup)
{
#ifdef USE_TABLE_COUNTER
	if(table_current_pos == 0)
		table_current_pos = clock_get_time_ms() + ROUND_TX_MS_TO_HIGHER_VALUE + TX_OFFSET_MS;
#endif

	next_tx_wkup = 1;
	table_operation_status status = NO_SLOTS_AVAILABLE;

	uint8_t slot_number = table_get_tx_slot_number();
	uint16_t ms_in_slot = table_get_tx_ms_in_slot();

	uint8_t i;

	//Clear previous slots
	for(i = 0; i < table_channel_count; i++)
		table[std::make_pair(i, (slot_number + table_slot_count - 1) % table_slot_count)]->set_slot_sent_index(0);

	//Check if we are in guard space
	if(ms_in_slot < GUARD_MS_COUNT)
	{
#ifdef USE_TABLE_COUNTER
		table_current_pos += 1;
#endif
		return status;
	}

	//Check for BROADCAST slots
	for(i = 0; i < table_channel_count; i++)
	{
		if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::CONTROL_BROADCAST)
		{
			status = SUCCESS;
			break;
		}
	}

	//Check if we found broadcast slot or if we are in guard space
	if(status == SUCCESS || ms_in_slot == (table_sframe_duration/table_slot_count - TDMA_ACK_QUEUE_SIZE))
	{
#ifdef USE_TABLE_COUNTER
		table_current_pos += 1;
#endif
		return status;
	}

	uint8_t next_tx_slot_number = 0;

	//Check for RECEIVE slots
	if(ms_in_slot > (table_sframe_duration/table_slot_count - TDMA_ACK_QUEUE_SIZE))
	{
		//Clear previous slots
		for(i = 0; i < table_channel_count; i++)
		{
			if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::UNICAST_SEND)
			{
				table[std::make_pair(i, slot_number)]->set_slot_sent_index(0);
				break;
			}
		}
		for(i = 0; i < table_channel_count; i++)
		{
			if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::RECEIVE)
			{
				status = SUCCESS;
				break;
			}
		}
	}
	//Check for UNICAST_SEND slots
	else
	{
		for(i = 0; i < table_channel_count; i++)
		{
			if(table[std::make_pair(i, slot_number)]->get_slot_type() == TDMA_slot::UNICAST_SEND)
			{
				status = SUCCESS;
				break;
			}
		}
	}

	if(status == SUCCESS)
	{
#ifdef USE_TABLE_COUNTER
		table_current_pos += 1;
#endif
		return status;
	}

	//Find next Tx slot
	for(uint8_t j = slot_number + 1; j < table_slot_count; j++)
	{
		for(uint8_t i = 0; i < table_channel_count; i++)
		{
			if(table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::UNICAST_SEND || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::RECEIVE || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::CONTROL_BROADCAST)
			{
				status = SUCCESS;
				next_tx_slot_number = j;
				break;
			}
		}
		if(status == SUCCESS)
			break;
	}

	if(status != SUCCESS)
	{
		for(uint8_t j = 0; j < slot_number; j++)
		{
			for(uint8_t i = 0; i < table_channel_count; i++)
			{
				if(table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::UNICAST_SEND || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::RECEIVE || table[std::make_pair(i, j)]->get_slot_type() == TDMA_slot::CONTROL_BROADCAST)
				{
					status = SUCCESS;
					next_tx_slot_number = j + table_slot_count;
					break;
				}
			}
			if(status == SUCCESS)
				break;
		}
	}

	if(status != SUCCESS)
	{
		NS_LOG_WARN("No transmit slots available.");
		return status;
	}

	next_tx_wkup = (table_sframe_duration/table_slot_count - table_get_tx_ms_in_slot()) + table_sframe_duration/table_slot_count * (next_tx_slot_number - 1 - slot_number);
#ifdef USE_TABLE_COUNTER
	table_current_pos += next_tx_wkup;
#endif

	return status;
}

#ifdef USE_QUEUES
TDMA_table::table_operation_status TDMA_table::table_get_next_tx_message(mac_container &container, uint16_t &next_tx_wkup)
{
#ifdef USE_TABLE_COUNTER
	if(table_current_pos == 0)
		table_current_pos = clock_get_time_ms() + ROUND_TX_MS_TO_HIGHER_VALUE + TX_OFFSET_MS;
#endif

	table_operation_status status = NOTHING_TO_SEND;
	next_tx_wkup = 1;
	std::vector<Ptr<Packet>> data;
	std::pair<mcs_t, uint32_t> mcs_gain;
	std::vector<udp_port_t> port;
	std::vector<uint32_t> payload_size;

	uint8_t slot_number = table_get_tx_slot_number();
	uint16_t ms_in_slot = table_get_tx_ms_in_slot();

	uint8_t i;
	uint16_t packets_count;

	//Clear previous slots
	for(i = 0; i < table_channel_count; i++)
		table[std::make_pair(i, (slot_number + table_slot_count - 1) % table_slot_count)]->set_slot_sent_index(0);

	//Check if we are in guard space
	if(ms_in_slot < GUARD_MS_COUNT)
	{
#ifdef USE_TABLE_COUNTER
		table_current_pos += 1;
#endif
		return status;
	}

	TDMA_slot::tdma_slot_type type_us;
	uint16_t size_us;
	mac_address_t remote_address_us;

	//Check for BROADCAST slots
	for(i = 0; i < table_channel_count; i++)
	{
		type_us = table[std::make_pair(i, slot_number)]->get_slot_type();
		if(type_us == TDMA_slot::CONTROL_BROADCAST)
		{
#ifdef USE_SLOTTED_ALOHA
			if(calculate_probability(ntable->neighbour_table_get_size() + 1))
#else
			if(table_randomize_bc_slot(next_tx_wkup, ms_in_slot))
#endif
			{
				TDMA_queue::queue_operation_status queue_status;
				uint16_t reduce_value = 0;
#ifndef USE_SLOTTED_ALOHA
				packets_count = table_sframe_duration/table_slot_count - ms_in_slot;
#else
				packets_count = 1;
#endif
				queue_status = table_queues[BROADCAST_MAC_ADDRESS]->queue_get_data_mcs_gain(data, mcs_gain, port, payload_size, packets_count, reduce_value);
				if(queue_status == TDMA_queue::SUCCESS)
					status = SUCCESS;
				break;
			}
			else
			{
#ifdef USE_TABLE_COUNTER
#ifdef USE_SLOTTED_ALOHA
				table_current_pos += 1;
#else
				table_current_pos += next_tx_wkup;
#endif
#endif
				return status;
			}
		}
	}

	if(status != SUCCESS)
	{
		if(ms_in_slot != (table_sframe_duration/table_slot_count - TDMA_ACK_QUEUE_SIZE))
		{
			//Check for RECEIVE slots
			if(ms_in_slot > (table_sframe_duration/table_slot_count - TDMA_ACK_QUEUE_SIZE))
			{
				for(i = 0; i < table_channel_count; i++)
				{
					table[std::make_pair(i, slot_number)]->get_slot_info(type_us, size_us, remote_address_us);
					if(type_us == TDMA_slot::RECEIVE)
					{
						TDMA_slot::queue_operation_status slot_status;
						packets_count = table_sframe_duration/table_slot_count - ms_in_slot;
						packets_count = 1;
						slot_status = table[std::make_pair(i, slot_number)]->get_data_mcs_gain(data, mcs_gain, packets_count);
						if(slot_status == TDMA_slot::SUCCESS)
						{
							for(uint8_t j = 0; j < packets_count; j++)
								port.push_back(0);
							status = SUCCESS;
						}
						else
						{
							if(ms_in_slot == (table_sframe_duration/table_slot_count - LAST_ACK_CREATE))
							{
							    if(m_mac->TDMA_prepare_and_send_last_ack(remote_address_us, data, mcs_gain, std::make_pair(i, slot_number)))
							    {
							    	port.push_back(0);
							    	packets_count = 1;
							    	status = SUCCESS;
							    }
							}
						}
						break;
					}
				}
			}
			//Check for UNICAST_SEND slots
			else
			{
				for(i = 0; i < table_channel_count; i++)
				{
					table[std::make_pair(i, slot_number)]->get_slot_info(type_us, size_us, remote_address_us);
					if(type_us == TDMA_slot::UNICAST_SEND)
					{
						TDMA_queue::queue_operation_status queue_status;
						uint16_t reduce_value = 0;
						packets_count = size_us - ms_in_slot;
						packets_count = 1;
						queue_status = table_queues[remote_address_us]->queue_get_data_mcs_gain(data, mcs_gain, port, payload_size, packets_count, reduce_value);
						if(queue_status == TDMA_queue::SUCCESS)
						{
							m_mac->reduce_previous_packets_per_second(reduce_value);
							status = SUCCESS;
						}
						break;
					}
				}
			}
		}
	}

	if(status != SUCCESS)
	{
#ifdef USE_TABLE_COUNTER
		table_current_pos += 1;
#endif
		return status;
	}

	//Populate mac container
	//mac_container_populate_data(container, data);

	for(uint8_t populate_index = 0; populate_index < data.size(); populate_index++)
		container.data.push_back(data[populate_index]);
		//container.data[populate_index] = data[populate_index];

	mac_container_populate_port_payload_size(&container, port, payload_size);
	mac_container_populate_slot_info(container, i, slot_number, type_us);
#ifdef USE_TABLE_COUNTER
	mac_container_populate_ctrl_info(container, TX, i, table_current_pos, mcs_gain.first, mcs_gain.second, get_bytes_in_slot(Five, mcs_gain.first) * data.size()); //TODO hardcoded fix this
	table_current_pos += packets_count;
#else
	//Before ns3 the first one
	mac_container_populate_ctrl_info(container, TX, i, clock_get_time_ms() + ROUND_TX_MS_TO_HIGHER_VALUE + TX_OFFSET_MS, mcs_gain.first, mcs_gain.second, get_bytes_in_slot(Five, mcs_gain.first) * data.size());
	//mac_container_populate_ctrl_info(container, TX, i, clock_get_time_ms() + RX_TIMER_OFFSET_MS, mcs_gain.first, mcs_gain.second, get_bytes_in_slot(Five, mcs_gain.first) * data.size());
	//mac_container_populate_ctrl_info(container, TX, i, 0, mcs_gain.first, mcs_gain.second, get_bytes_in_slot(Five, mcs_gain.first) * data.size());
#endif
	container.packets_in_data = packets_count;

	next_tx_wkup = packets_count;

	return status;
}

TDMA_table::table_operation_status TDMA_table::table_get_cb_message(mac_container &container, uint16_t &next_tx_wkup)
{
#ifdef USE_TABLE_COUNTER
	if(table_current_pos == 0)
		table_current_pos = clock_get_time_ms() + ROUND_TX_MS_TO_HIGHER_VALUE + TX_OFFSET_MS;
#endif

	table_operation_status status = NOTHING_TO_SEND;
	next_tx_wkup = 1;
	std::vector<Ptr<Packet>> data;
	std::pair<mcs_t, uint32_t> mcs_gain;
	std::vector<udp_port_t> port;
	std::vector<uint32_t> payload_size;

	uint8_t slot_number = table_get_tx_slot_number();
	uint16_t ms_in_slot = table_get_tx_ms_in_slot();

	uint8_t i;
	uint16_t packets_count;

	TDMA_slot::tdma_slot_type type_us;
	uint16_t size_us;
	mac_address_t remote_address_us;

	//Check for BROADCAST slots
	for(i = 0; i < table_channel_count; i++)
	{
		type_us = table[std::make_pair(i, slot_number)]->get_slot_type();
		if(type_us == TDMA_slot::CONTROL_BROADCAST)
		{
#ifdef USE_SLOTTED_ALOHA
			if(true)
#else
			if(table_randomize_bc_slot(next_tx_wkup, ms_in_slot))
#endif
			{
				TDMA_queue::queue_operation_status queue_status;
				uint16_t reduce_value = 0;
#ifndef USE_SLOTTED_ALOHA
				packets_count = table_sframe_duration/table_slot_count - ms_in_slot;
#else
				packets_count = 1;
#endif
				queue_status = table_queues[BROADCAST_MAC_ADDRESS]->queue_get_data_mcs_gain(data, mcs_gain, port, payload_size, packets_count, reduce_value);
				if(queue_status == TDMA_queue::SUCCESS)
					status = SUCCESS;
				break;
			}
			else
			{
#ifdef USE_TABLE_COUNTER
#ifdef USE_SLOTTED_ALOHA
				table_current_pos += 1;
#else
				table_current_pos += next_tx_wkup;
#endif
#endif
				return status;
			}
		}
	}

	if(status != SUCCESS)
	{
#ifdef USE_TABLE_COUNTER
		table_current_pos += 1;
#endif
		return status;
	}

	//Populate mac container
	//mac_container_populate_data(container, data);

	for(uint8_t populate_index = 0; populate_index < data.size(); populate_index++)
		container.data.push_back(data[populate_index]);
		//container.data[populate_index] = data[populate_index];

	mac_container_populate_port_payload_size(&container, port, payload_size);
	mac_container_populate_slot_info(container, i, slot_number, type_us);
#ifdef USE_TABLE_COUNTER
	mac_container_populate_ctrl_info(container, TX, i, table_current_pos, mcs_gain.first, mcs_gain.second, get_bytes_in_slot(Five, mcs_gain.first) * data.size()); //TODO hardcoded fix this
	table_current_pos += packets_count;
#else
	//Before ns3 the first one
	mac_container_populate_ctrl_info(container, TX, i, clock_get_time_ms() + ROUND_TX_MS_TO_HIGHER_VALUE + TX_OFFSET_MS, mcs_gain.first, mcs_gain.second, get_bytes_in_slot(Five, mcs_gain.first) * data.size());
	//mac_container_populate_ctrl_info(container, TX, i, clock_get_time_ms() + RX_TIMER_OFFSET_MS, mcs_gain.first, mcs_gain.second, get_bytes_in_slot(Five, mcs_gain.first) * data.size());
	//mac_container_populate_ctrl_info(container, TX, i, 0, mcs_gain.first, mcs_gain.second, get_bytes_in_slot(Five, mcs_gain.first) * data.size());
#endif
	container.packets_in_data = packets_count;

	next_tx_wkup = packets_count;

	return status;
}

TDMA_table::table_operation_status TDMA_table::table_add_packets(std::vector<Ptr<Packet>> data, mcs_t mcs, uint8_t tx_gain, udp_port_t port, std::vector<uint32_t> payload_size, TDMA_queue::tdma_queue_packet_type packet_type, mac_address_t mac_address)
{
	TDMA_queue::queue_operation_status status = TDMA_queue::QUEUE_FULL;
	timestamp_t timestamp = clock_get_time_ns();
	uint16_t i;
	uint16_t data_size = data.size();

	switch (packet_type)
	{
	case TDMA_queue::APP_HIGH_PRIORITY:
		if(table_queues.find(mac_address) == table_queues.end())
		{
			table_queues[mac_address] = CreateObject<TDMA_queue> ();
			table_queues[mac_address]->TDMA_queue_init(TDMA_queue::UNICAST,  this->table_sframe_duration * 1000000, own_mac_address, mac_address);
#ifdef USE_SLOTTED_ALOHA
			ntable->neighbour_table_manual_update(mac_address);
#endif
		}
#if DATA_ORDER == OLDER_DATA_LAST
		for(i = 1; i <= data_size; i++)
			status = table_queues[mac_address]->push_front_high_priority(data[data_size - i], packet_type, mcs, tx_gain, port, payload_size[i], timestamp);
#elif DATA_ORDER == OLDER_DATA_FIRST
		for(i = 0; i < data_size; i++)
			status = table_queues[mac_address]->push_back_high_priority(data[i], packet_type, mcs, tx_gain, port, payload_size[i], timestamp);
#endif
		break;
	case TDMA_queue::APP_LOW_PRIORITY:
		if(table_queues.find(mac_address) == table_queues.end())
		{
			table_queues[mac_address] = CreateObject<TDMA_queue> ();
			table_queues[mac_address]->TDMA_queue_init(TDMA_queue::UNICAST, this->table_sframe_duration * 1000000, own_mac_address, mac_address);
#ifdef USE_SLOTTED_ALOHA
			ntable->neighbour_table_manual_update(mac_address);
#endif
		}
#if DATA_ORDER == OLDER_DATA_LAST
		for(i = 1; i <= data_size; i++)
			status = table_queues[mac_address]->push_to_position_low_priority(data[data_size - i], packet_type, mcs, tx_gain, port, payload_size[data_size - i], timestamp, m_mac->get_previous_packets_per_second());
		//status = table_queues[mac_address]->push_front_low_priority(data[data_size - i], packet_type, mcs, tx_gain, port, payload_size[data_size - i], timestamp);
		m_mac->increase_previous_packets_per_second(data_size);
#elif DATA_ORDER == OLDER_DATA_FIRST
		for(i = 0; i < data_size; i++)
			status = table_queues[mac_address]->push_back_low_priority(data[i], packet_type, mcs, tx_gain, port, payload_size[i], timestamp);
#endif
		break;
	case TDMA_queue::AI:
        NS_LOG_INFO("AI message added to BC queue at time: " << timestamp << ".");
		for(i = 1; i <= data_size; i++)
			status = table_queues[BROADCAST_MAC_ADDRESS]->push_back_high_priority(data[data_size - i], packet_type, mcs, tx_gain, port, payload_size[data_size - i], timestamp);
		break;
	case TDMA_queue::BROADCAST:
		for(i = 1; i <= data_size; i++)
			status = table_queues[BROADCAST_MAC_ADDRESS]->push_back_high_priority(data[data_size - i], packet_type, mcs, tx_gain, port, payload_size[data_size - i], timestamp);
		break;
	case TDMA_queue::CONTROL:
		for(i = 1; i <= data_size; i++)
			status = table_queues[BROADCAST_MAC_ADDRESS]->push_back_high_priority(data[data_size - i], packet_type, mcs, tx_gain, port, payload_size[data_size - i], timestamp);
		break;
	case TDMA_queue::RETRANSMISSION:
		for(i = 1; i <= data_size; i++)
			status = table_queues[mac_address]->push_front_high_priority(data[data_size - i], packet_type, mcs, tx_gain, port, payload_size[data_size - i], timestamp);
		break;
	default:
		break;
	}

	return status == TDMA_queue::SUCCESS ? SUCCESS : NOT_ENOUGH_SPACE;
}

TDMA_table::table_operation_status TDMA_table::table_add_packet(Ptr<Packet> data, mcs_t mcs, uint8_t tx_gain, udp_port_t port, uint32_t payload_size, TDMA_queue::tdma_queue_packet_type packet_type, mac_address_t mac_address)
{
	TDMA_queue::queue_operation_status status = TDMA_queue::QUEUE_FULL;
	timestamp_t timestamp = clock_get_time_ns();

	switch (packet_type)
	{
	case TDMA_queue::APP_HIGH_PRIORITY:
		if(table_queues.find(mac_address) == table_queues.end())
		{
			table_queues[mac_address] = CreateObject<TDMA_queue> ();
			table_queues[mac_address]->TDMA_queue_init(TDMA_queue::UNICAST, this->table_sframe_duration * 1000000, own_mac_address, mac_address);
		}
#if DATA_ORDER == OLDER_DATA_LAST
		status = table_queues[mac_address]->push_front_high_priority(data, packet_type, mcs, tx_gain, port, payload_size, timestamp);
#elif DATA_ORDER == OLDER_DATA_FIRST
		status = table_queues[mac_address]->push_back_high_priority(data, packet_type, mcs, tx_gain, port, payload_size, timestamp);
#endif
		break;
	case TDMA_queue::APP_LOW_PRIORITY:
		if(table_queues.find(mac_address) == table_queues.end())
		{
			table_queues[mac_address] = CreateObject<TDMA_queue> ();
			table_queues[mac_address]->TDMA_queue_init(TDMA_queue::UNICAST, this->table_sframe_duration * 1000000, own_mac_address, mac_address);
		}
#if DATA_ORDER == OLDER_DATA_LAST
		status = table_queues[mac_address]->push_front_low_priority(data, packet_type, mcs, tx_gain, port, payload_size, timestamp);
#elif DATA_ORDER == OLDER_DATA_FIRST
		status = table_queues[mac_address]->push_back_low_priority(data, packet_type, mcs, tx_gain, port, payload_size, timestamp);
#endif
		break;
	case TDMA_queue::BROADCAST:
		status = table_queues[BROADCAST_MAC_ADDRESS]->push_back_high_priority(data, packet_type, mcs, tx_gain, port, payload_size, timestamp);
		break;
	case TDMA_queue::CONTROL:
		status = table_queues[BROADCAST_MAC_ADDRESS]->push_back_high_priority(data, packet_type, mcs, tx_gain, port, payload_size, timestamp);
		break;
	case TDMA_queue::CONTROL_ACK:
		status = table_queues[BROADCAST_MAC_ADDRESS]->push_front_high_priority(data, packet_type, mcs, tx_gain, port, payload_size, timestamp);
		break;
	case TDMA_queue::RETRANSMISSION:
		status = table_queues[mac_address]->push_front_high_priority(data, packet_type, mcs, tx_gain, port, payload_size, timestamp);
		break;
	default:
		break;
	}

	return status == TDMA_queue::SUCCESS ? SUCCESS : NOT_ENOUGH_SPACE;
}

TDMA_table::table_operation_status TDMA_table::table_add_control_ack(Ptr<Packet> data, mac_address_t mac_address, uint8_t tx_gain, mcs_t mcs)
{
	return table_add_packet(data, mcs, tx_gain, 0, 0, TDMA_queue::CONTROL_ACK, mac_address);
}

TDMA_table::table_operation_status TDMA_table::table_add_unicast_queue(mac_address_t remote_mac_address)
{
	table_operation_status status = SUCCESS;
	if(table_queues.find(remote_mac_address) == table_queues.end())
	{
		table_queues[remote_mac_address] = CreateObject<TDMA_queue> ();
		table_queues[remote_mac_address]->TDMA_queue_init(TDMA_queue::UNICAST, this->table_sframe_duration * 1000000, own_mac_address, remote_mac_address);
		NS_LOG_INFO("Queue added for remote mac address " << remote_mac_address << ".");
	}
	else
	{
		status = QUEUE_EXIST;
		NS_LOG_INFO("No queue added for remote mac address " << remote_mac_address << ". This queue already exist!");
	}

	return status;
}

std::map<mac_address_t,uint16_t>* TDMA_table::table_queue_check_timeout(timestamp_t timestamp)
{
	std::map<mac_address_t,uint16_t>* temp_mac_address_set = new std::map<mac_address_t,uint16_t>;
	std::map<mac_address_t,uint16_t>::iterator mac_it;

	std::map<mac_address_t, Ptr<TDMA_queue>>::iterator it;

	for (it = table_queues.begin(); it != table_queues.end(); ++it)
    {
    	if(it->second->get_queue_type() != TDMA_queue::BROADCAST_CONTROL)
    	{
    		uint16_t queue_size = it->second->get_queue_low_priority_size();
#ifdef FILTER_HIGH_PRIORITY_APP_DATA
    		queue_size += it->second->get_queue_high_priority_data_size();
#endif
    		NS_LOG_LOGIC("Queue for remote mac address " << it->second->get_remote_mac_address() << " before timeout check has " << queue_size << " packets.");

    		//If the queue loses packets because of delayed packets in the queue, clear those packets and return the relevant mac addresses
    		uint16_t packets_dropped=it->second->queue_check_timeout(timestamp);
    		timeout_drop_count += packets_dropped;
    		if(packets_dropped!=0 && it->second->get_remote_mac_address()!=BROADCAST_MAC_ADDRESS){
    			(*temp_mac_address_set)[(mac_address_t)it->second->get_remote_mac_address()]=packets_dropped;
    		}

    		queue_size = it->second->get_queue_low_priority_size();
#ifdef FILTER_HIGH_PRIORITY_APP_DATA
    		queue_size += it->second->get_queue_high_priority_data_size();
#endif
    		NS_LOG_LOGIC("Queue for remote mac address " << it->second->get_remote_mac_address() << " after timeout check has " << queue_size << " packets.");
    		NS_LOG_LOGIC("Queue for remote mac address " << it->second->get_remote_mac_address() << " has retransmission " << it->second->get_queue_retransmission_size() << " packets.");
    	}
    	else
    	{
    		NS_LOG_LOGIC("Queue for control_broadcast packets has " << it->second->get_queue_size() << " packets.");
    	}
    }

	mac_it = temp_mac_address_set->begin();

	while(mac_it!=temp_mac_address_set->end()){
	    if(mac_it->second<PACKETS_DROPPED_THRESHOLD){
	    	NS_LOG_ERROR("No enough deleted packets (" << mac_it->second << ")for mac address " << mac_it->first << " added.");
	    	temp_mac_address_set->erase(mac_it++);
	    	}
	    	else{
	    	NS_LOG_ERROR("Node had enough deleted packets (" << mac_it->second << ")for mac address " << mac_it->first << ".");
	        ++mac_it;
	    }
	}

/**
	for (mac_it = temp_mac_address_set->begin(); mac_it != temp_mac_address_set->end(); ++mac_it){

		if(mac_it->second<PACKETS_DROPPED_THRESHOLD){
			mac_it=temp_mac_address_set->erase(mac_it);
			Logger::log_info<TDMA_table>("No enough deleted packets ({})for mac address {} added.", mac_it->second, mac_it->first);
		}
		else
			Logger::log_info<TDMA_table>("Node had enough deleted packets ({})for mac address {}.", mac_it->second, mac_it->first);
	}
**/
    return temp_mac_address_set;
}
#endif

TDMA_table::table_operation_status TDMA_table::table_add_ack(Ptr<Packet> data, mac_address_t mac_address, std::pair<channel_t, slot_number_t> &slot_info, uint8_t tx_gain, mcs_t mcs)
{
	std::vector<Ptr<TDMA_slot>> slots;
	std::vector<std::pair<channel_t, slot_number_t>> slots_info;
	bool check_type;

	table_operation_status status = table_get_slots(slots, slots_info, TDMA_slot::RECEIVE, check_type, mac_address);

	if(status != SUCCESS)
	{
		NS_LOG_ERROR("No packets added. No RECEIVE_tx slots for remote mac address " << mac_address << ".");
		return status;
	}

	std::vector<uint16_t> slot_free_sizes;
	status = check_data_size(1, slot_free_sizes, slots, check_type);

	if(status != SUCCESS)
	{
		NS_LOG_INFO("No packets added. Currently not enough space in RECEIVE_tx slots, remote mac address " << mac_address << " for 1 packets.");
		return status;
	}

	uint8_t k = 0;
	while(slot_free_sizes[k] < 1)
		k++;

	if(slots[k]->push_back(data, mcs, tx_gain) != TDMA_slot::SUCCESS)
	{
		NS_LOG_ERROR("Not enough space in slot for packet 1. Free space of slot calculated previously " << slot_free_sizes[k] << ". THIS SHOULD NEVER HAPPEN!");
		return NOT_ENOUGH_SPACE;
	}

	slot_info = slots_info[k];

	return status;
}

TDMA_table::table_operation_status TDMA_table::table_create_static(mac_address_t own_mac_address, mac_address_t remote_mac_address, uint8_t cb_slots_count)
{
	table_operation_status status;
/**
#ifdef USE_QUEUES
	status = table_add_unicast_queue(remote_mac_address);
#endif
**/

#ifndef STATIC_TABLE_TYPE
	status = table_create_static_bc_slots(cb_slots_count);
#else
	status = add_slot(0, 0, TDMA_slot::CONTROL_BROADCAST);
#endif

	if(status!=SUCCESS)
		return status;

	for(uint8_t i = 1; i < table_slot_count; i++)
	{
#if STATIC_TABLE_TYPE == STATIC_TABLE_CRISS_CROSS_ONE_CH
		if((i % 2) == 0)
		{
			if(remote_mac_address == TEST_MAC_ADDRESS1)
				status = add_slot(0, i, TDMA_slot::UNICAST_SEND, remote_mac_address);
			else if(remote_mac_address == TEST_MAC_ADDRESS2)
				status = add_slot(0, i, TDMA_slot::RECEIVE, remote_mac_address);
			if(status != TDMA_table::SUCCESS)
				return status;
		}
		else
		{
			if(remote_mac_address == TEST_MAC_ADDRESS1)
				status = add_slot(2, i, TDMA_slot::RECEIVE, remote_mac_address);
			else if(remote_mac_address == TEST_MAC_ADDRESS2)
				status = add_slot(2, i, TDMA_slot::UNICAST_SEND, remote_mac_address);
			if(status != TDMA_table::SUCCESS)
				return status;
		}
#elif STATIC_TABLE_TYPE == STATIC_TABLE_CRISS_CROSS_TWO_CH
		if((i % 2) == 0)
		{
			if(remote_mac_address == TEST_MAC_ADDRESS1)
			{
				status = add_slot(2, i, TDMA_slot::UNICAST_SEND, remote_mac_address);
				status = add_slot(0, i, TDMA_slot::RECEIVE, remote_mac_address);
			}
			else if(remote_mac_address == TEST_MAC_ADDRESS2)
			{
				status = add_slot(2, i, TDMA_slot::RECEIVE, remote_mac_address);
			}
			if(status != TDMA_table::SUCCESS)
				return status;
		}
		else
		{
			if(remote_mac_address == TEST_MAC_ADDRESS1)
			{
				status = add_slot(2, i, TDMA_slot::RECEIVE, remote_mac_address);
			}
			else if(remote_mac_address == TEST_MAC_ADDRESS2)
			{
				status = add_slot(2, i, TDMA_slot::UNICAST_SEND, remote_mac_address);
				status = add_slot(0, i, TDMA_slot::RECEIVE, remote_mac_address);
			}
			if(status != TDMA_table::SUCCESS)
				return status;
		}
#elif STATIC_TABLE_TYPE == STATIC_TABLE_FULL_TWO_CH
		if(remote_mac_address == TEST_MAC_ADDRESS1)
		{
			status = add_slot(0, i, TDMA_slot::UNICAST_SEND, remote_mac_address);
			status = add_slot(2, i, TDMA_slot::RECEIVE, remote_mac_address);
		}
		else if(remote_mac_address == TEST_MAC_ADDRESS2)
		{
			status = add_slot(2, i, TDMA_slot::UNICAST_SEND, remote_mac_address);
			status = add_slot(0, i, TDMA_slot::RECEIVE, remote_mac_address);
		}
		if(status != TDMA_table::SUCCESS)
			return status;
#elif STATIC_TABLE_TYPE == STATIC_TABLE_CRISS_CROSS_TWO_CH_THREE_NODES
		if((i % 2) == 0)
		{
			if(remote_mac_address == TEST_MAC_ADDRESS1)
			{
				if(own_mac_address == TEST_MAC_ADDRESS2)
				{
					status = add_slot(0, i, TDMA_slot::UNICAST_SEND, remote_mac_address);
					status = add_slot(2, i, TDMA_slot::RECEIVE, remote_mac_address);
				}
				else if(own_mac_address == TEST_MAC_ADDRESS3)
				{
					status = add_slot(3, i, TDMA_slot::RECEIVE, remote_mac_address);
				}
			}
			else if(remote_mac_address == TEST_MAC_ADDRESS2)
			{
				status = add_slot(2, i, TDMA_slot::UNICAST_SEND, remote_mac_address);
				status = add_slot(0, i, TDMA_slot::RECEIVE, remote_mac_address);
			}
			if(status != TDMA_table::SUCCESS)
				return status;
		}
		else
		{
			if(remote_mac_address == TEST_MAC_ADDRESS1)
			{
				if(own_mac_address == TEST_MAC_ADDRESS2)
				{
					status = add_slot(3, i, TDMA_slot::RECEIVE, remote_mac_address);
				}
				else if(own_mac_address == TEST_MAC_ADDRESS3)
				{
					status = add_slot(1, i, TDMA_slot::UNICAST_SEND, remote_mac_address);
					status = add_slot(2, i, TDMA_slot::RECEIVE, remote_mac_address);
				}
			}
			else if(remote_mac_address == TEST_MAC_ADDRESS2)
			{
				status = add_slot(2, i, TDMA_slot::UNICAST_SEND, remote_mac_address + 1);
				status = add_slot(1, i, TDMA_slot::RECEIVE, remote_mac_address + 1);
			}
			if(status != TDMA_table::SUCCESS)
				return status;
		}
#elif STATIC_TABLE_TYPE == STATIC_TABLE_ONE_RX_ONE_TX
		if(remote_mac_address == TEST_MAC_ADDRESS1)
		{
			status = add_slot(0, 5, TDMA_slot::UNICAST_SEND, remote_mac_address);
			status = add_slot(2, 15, TDMA_slot::RECEIVE, remote_mac_address);
		}
		else if(remote_mac_address == TEST_MAC_ADDRESS2)
		{
			status = add_slot(2, 15, TDMA_slot::UNICAST_SEND, remote_mac_address);
			status = add_slot(0, 5, TDMA_slot::RECEIVE, remote_mac_address);
		}
		if(status != TDMA_table::SUCCESS)
			return status;
#elif STATIC_TABLE_TYPE == STATIC_TABLE_RANDOM
		static const uint8_t tx[19] = {3, 1, 2, 1, 0, 3, 3, 1, 0, 2, 0, 1, 2, 3, 0, 2, 2, 3, 3};
		static const uint8_t rx[19] = {1, 0, 3, 2, 2, 0, 0, 3, 2, 1, 3, 0, 3, 2, 1, 1, 0, 0, 1};

		if(remote_mac_address == TEST_MAC_ADDRESS1)
		{
			status = add_slot(tx[i-1], i, TDMA_slot::UNICAST_SEND, remote_mac_address);
			status = add_slot(rx[i-1], i, TDMA_slot::RECEIVE, remote_mac_address);
		}
		else if(remote_mac_address == TEST_MAC_ADDRESS2)
		{
			status = add_slot(rx[i-1], i, TDMA_slot::UNICAST_SEND, remote_mac_address);
			status = add_slot(tx[i-1], i, TDMA_slot::RECEIVE, remote_mac_address);
		}
		if(status != TDMA_table::SUCCESS)
			return status;
#endif
	}
	//print_table_info(); //TODO uncomment this
return status;
}

TDMA_table::table_operation_status TDMA_table::table_create_static_bc_slots(uint8_t cb_slots_count)
{
	table_operation_status status;

	//Create the static broadcast and control slots based on the number of available channels
	switch(this->get_table_channel_count())
	{
		case 1:
			if(cb_slots_count > 4)
				this->broadcast_slots_count=4;
			else if(cb_slots_count == 0)
				this->broadcast_slots_count=1;
			else
				this->broadcast_slots_count=cb_slots_count;

			switch (this->broadcast_slots_count) {
			case 4:
				this->broadcast_slots[3].channel=0;
				this->broadcast_slots[3].timeslot=(slot_number_t)(this->get_table_slot_count()*3/4+1);
			case 3:
				this->broadcast_slots[2].channel=0;
				this->broadcast_slots[2].timeslot=(slot_number_t)(this->get_table_slot_count()*2/4+1);
			case 2:
				this->broadcast_slots[1].channel=0;
				this->broadcast_slots[1].timeslot=(slot_number_t)(this->get_table_slot_count()/4+1);
			case 1:
				this->broadcast_slots[0].channel=0;
				this->broadcast_slots[0].timeslot=0;
				break;
			}

			break;
		case 2:
			if(cb_slots_count > 4)
				this->broadcast_slots_count=4;
			else if(cb_slots_count == 0)
				this->broadcast_slots_count=1;
			else
				this->broadcast_slots_count=cb_slots_count;

			switch (this->broadcast_slots_count) {
			case 4:
				this->broadcast_slots[3].channel=1;
				this->broadcast_slots[3].timeslot=(slot_number_t)(this->get_table_slot_count()*3/4+1);
			case 3:
				this->broadcast_slots[2].channel=1;
				this->broadcast_slots[2].timeslot=(slot_number_t)(this->get_table_slot_count()*2/4+1);
			case 2:
				this->broadcast_slots[1].channel=0;
				this->broadcast_slots[1].timeslot=(slot_number_t)(this->get_table_slot_count()/4+1);
			case 1:
				this->broadcast_slots[0].channel=0;
				this->broadcast_slots[0].timeslot=0;
				break;
			}

			break;
		case 3:
			if(cb_slots_count > 4)
				this->broadcast_slots_count=4;
			else if(cb_slots_count == 0)
				this->broadcast_slots_count=1;
			else
				this->broadcast_slots_count=cb_slots_count;

			switch (this->broadcast_slots_count) {
			case 4:
				this->broadcast_slots[3].channel=2;
				this->broadcast_slots[3].timeslot=(slot_number_t)(this->get_table_slot_count()*3/4+1);
			case 3:
				this->broadcast_slots[2].channel=2;
				this->broadcast_slots[2].timeslot=(slot_number_t)(this->get_table_slot_count()*2/4+1);
			case 2:
				this->broadcast_slots[1].channel=1;
				this->broadcast_slots[1].timeslot=(slot_number_t)(this->get_table_slot_count()/4+1);
			case 1:
				this->broadcast_slots[0].channel=0;
				this->broadcast_slots[0].timeslot=0;
				break;
			}

			break;
		case 4:
			if(cb_slots_count > 4)
				this->broadcast_slots_count=4;
			else if(cb_slots_count == 0)
				this->broadcast_slots_count=1;
			else
				this->broadcast_slots_count=cb_slots_count;

			switch (this->broadcast_slots_count) {
			case 4:
				this->broadcast_slots[3].channel=3;
				this->broadcast_slots[3].timeslot=(slot_number_t)(this->get_table_slot_count()*3/4+1);
				this->broadcast_slots[2].channel=2;
				this->broadcast_slots[2].timeslot=(slot_number_t)(this->get_table_slot_count()*2/4+1);
				this->broadcast_slots[1].channel=1;
				this->broadcast_slots[1].timeslot=(slot_number_t)(this->get_table_slot_count()/4+1);
				this->broadcast_slots[0].channel=0;
				this->broadcast_slots[0].timeslot=0;
				break;
			case 3:
				this->broadcast_slots[2].channel=2;
				this->broadcast_slots[2].timeslot=(slot_number_t)(this->get_table_slot_count()*2/3+1);
				this->broadcast_slots[1].channel=1;
				this->broadcast_slots[1].timeslot=(slot_number_t)(this->get_table_slot_count()/3+1);
				this->broadcast_slots[0].channel=0;
				this->broadcast_slots[0].timeslot=0;
				break;
			case 2:
				this->broadcast_slots[1].channel=1;
				this->broadcast_slots[1].timeslot=(slot_number_t)(this->get_table_slot_count()/2+1);
				this->broadcast_slots[0].channel=0;
				this->broadcast_slots[0].timeslot=0;
				break;
			case 1:
				this->broadcast_slots[0].channel=0;
				this->broadcast_slots[0].timeslot=0;
				break;
			}

			break;
		case 5:
			if(cb_slots_count > 4)
				this->broadcast_slots_count=4;
			else if(cb_slots_count == 0)
				this->broadcast_slots_count=1;
			else
				this->broadcast_slots_count=cb_slots_count;

			switch (this->broadcast_slots_count) {
			case 4:
				this->broadcast_slots[3].channel=4;
				this->broadcast_slots[3].timeslot=(slot_number_t)(this->get_table_slot_count()*3/4+1);
			case 3:
				this->broadcast_slots[2].channel=3;
				this->broadcast_slots[2].timeslot=(slot_number_t)(this->get_table_slot_count()*2/4+1);
			case 2:
				this->broadcast_slots[1].channel=2;
				this->broadcast_slots[1].timeslot=(slot_number_t)(this->get_table_slot_count()/4+1);
			case 1:
				this->broadcast_slots[0].channel=0;
				this->broadcast_slots[0].timeslot=0;
				break;
			}

			break;
		case 6:
		case 7:
			if(cb_slots_count > 4)
				this->broadcast_slots_count=4;
			else if(cb_slots_count == 0)
				this->broadcast_slots_count=1;
			else
				this->broadcast_slots_count=cb_slots_count;

			switch (this->broadcast_slots_count) {
			case 4:
				this->broadcast_slots[3].channel=5;
				this->broadcast_slots[3].timeslot=(slot_number_t)(this->get_table_slot_count()*3/4+1);
			case 3:
				this->broadcast_slots[2].channel=3;
				this->broadcast_slots[2].timeslot=(slot_number_t)(this->get_table_slot_count()*2/4+1);
			case 2:
				this->broadcast_slots[1].channel=2;
				this->broadcast_slots[1].timeslot=(slot_number_t)(this->get_table_slot_count()/4+1);
			case 1:
				this->broadcast_slots[0].channel=0;
				this->broadcast_slots[0].timeslot=0;
				break;
			}

			break;
		default:
			if(cb_slots_count > 5)
				this->broadcast_slots_count=5;
			else if(cb_slots_count == 0)
				this->broadcast_slots_count=1;
			else
				this->broadcast_slots_count=cb_slots_count;

			switch (this->broadcast_slots_count) {
			case 5:
				this->broadcast_slots[4].channel=9;
				this->broadcast_slots[4].timeslot=(slot_number_t)(this->get_table_slot_count()*4/5+1);
				this->broadcast_slots[3].channel=7;
				this->broadcast_slots[3].timeslot=(slot_number_t)(this->get_table_slot_count()*3/5+1);
				this->broadcast_slots[2].channel=5;
				this->broadcast_slots[2].timeslot=(slot_number_t)(this->get_table_slot_count()*2/5+1);
				this->broadcast_slots[1].channel=3;
				this->broadcast_slots[1].timeslot=(slot_number_t)(this->get_table_slot_count()/5+1);
				this->broadcast_slots[0].channel=0;
				this->broadcast_slots[0].timeslot=0;
				break;
			case 4:
				this->broadcast_slots[3].channel=7;
				this->broadcast_slots[3].timeslot=(slot_number_t)(this->get_table_slot_count()*3/4+1);
				this->broadcast_slots[2].channel=5;
				this->broadcast_slots[2].timeslot=(slot_number_t)(this->get_table_slot_count()/2+1);
				this->broadcast_slots[1].channel=3;
				this->broadcast_slots[1].timeslot=(slot_number_t)(this->get_table_slot_count()/4+1);
				this->broadcast_slots[0].channel=0;
				this->broadcast_slots[0].timeslot=0;
				break;
			case 3:
				this->broadcast_slots[2].channel=5;
				this->broadcast_slots[2].timeslot=(slot_number_t)(this->get_table_slot_count()*2/3+1);
				this->broadcast_slots[1].channel=3;
				this->broadcast_slots[1].timeslot=(slot_number_t)(this->get_table_slot_count()/3+1);
				this->broadcast_slots[0].channel=0;
				this->broadcast_slots[0].timeslot=0;
				break;
			case 2:
				this->broadcast_slots[1].channel=3;
				this->broadcast_slots[1].timeslot=(slot_number_t)(this->get_table_slot_count()/2+1);
				this->broadcast_slots[0].channel=0;
				this->broadcast_slots[0].timeslot=0;
				break;
			case 1:
				this->broadcast_slots[0].channel=0;
				this->broadcast_slots[0].timeslot=0;
				break;
			}

			break;
	}

	for (uint8_t i = 0; i< broadcast_slots_count; i++){
		status = add_slot(this->broadcast_slots[i].channel, this->broadcast_slots[i].timeslot, TDMA_slot::CONTROL_BROADCAST);
		if(status!=SUCCESS)
			return status;
	}

	return status;
}

void TDMA_table::print_table_info()
{
		NS_LOG_INFO("Printing current superframe state!");
	//Logger::log_info<TDMA_table>(" ");
    for(int8_t i = table_channel_count - 1; i >= 0; i--)
	{
		for(uint8_t j = 0; j < table_slot_count; j++)
		{
			table[std::make_pair(i, j)]->print_slot_info();
		}
		std::cout << std::endl;
	//Logger::log_info<TDMA_table>(" ");

	}
    std::cout << "END OF TABLE" << std::endl;
}

#ifdef USE_QUEUES
void TDMA_table::print_queues_info()
{
	std::map<mac_address_t, Ptr<TDMA_queue>>::iterator it;
    for (it = table_queues.begin(); it != table_queues.end(); ++it)
    {
    	it->second->print_queue_info();
		printf("\n\n");
    }
}
#endif

TDMA_table::table_operation_status TDMA_table::check_if_divisible(uint16_t first_number, uint16_t second_number)
{
	if(first_number % second_number != 0)
		return PARAMETERS_NOT_DIVISIBLE;

	return SUCCESS;
}

inline TDMA_table::table_operation_status TDMA_table::check_channel_and_slot_number(channel_t channel, slot_number_t slot_number)
{
	if(channel >= table_channel_count)
		return CHANNEL_OUT_OF_BOUNDS;

	if(slot_number >= table_slot_count)
		return SLOT_OUT_OF_BOUNDS;

	return SUCCESS;
}

TDMA_table::table_operation_status TDMA_table::check_data_size(uint16_t data_size, std::vector<uint16_t> &slot_free_sizes, std::vector<Ptr<TDMA_slot>> slots, bool check_type)
{
	uint16_t free_size, free_size_sum = 0;
	uint8_t i;
	if(check_type)
	{
		for(i = 0; i < slots.size() - 1; i++)
		{
			free_size = slots[i]->get_free_size();
			slot_free_sizes.push_back(free_size);
			free_size_sum += free_size;
		}

		free_size = slots[i]->get_slot_sent_index();
		slot_free_sizes.push_back(free_size);
		free_size_sum += free_size;
	}
	else
	{
		for(i = 0; i < slots.size(); i++)
		{
			free_size = slots[i]->get_free_size();
			slot_free_sizes.push_back(free_size);
			free_size_sum += free_size;
		}
	}

	if(free_size_sum >= data_size)
		return SUCCESS;
	else
		return NOT_ENOUGH_SPACE;
}

TDMA_table::table_operation_status TDMA_table::check_data_size_high_priority(uint16_t data_size, std::vector<uint16_t> &slot_free_sizes, std::vector<Ptr<TDMA_slot>> slots, bool check_type)
{
	uint16_t slot_size = slots[0]->get_slot_size();

	table_operation_status status = check_data_size(data_size, slot_free_sizes, slots, check_type);

	uint8_t j(check_type == true ? 1 : 0);

	if((slots.size() - j) * slot_size < data_size)
		return NOT_ENOUGH_SPACE;

	if(status == SUCCESS)
		return status;
	else
	{
		uint16_t free_size_sum = 0, extra_data_size;
		uint8_t drop_from_slot, drop_from_slot_left;

		for(uint8_t i = 0; i < slots.size(); i++)
		{
			free_size_sum += slot_free_sizes[i];
		}

		extra_data_size = data_size - free_size_sum;
		drop_from_slot = extra_data_size / (slots.size() - j);
		drop_from_slot_left = extra_data_size % (slots.size() - j);

		if(j)
		{
			if((slot_size - slot_free_sizes[0] - slot_free_sizes[slots.size() - 1]) >= drop_from_slot)
			{
				slot_free_sizes[0] += drop_from_slot;
				extra_data_size -= drop_from_slot;
			}
			else
			{
				extra_data_size -= slot_size - slot_free_sizes[0] - slot_free_sizes[slots.size() - 1];
				slot_free_sizes[0] = slot_size - slot_free_sizes[slots.size() - 1];
			}
			drop_from_slot = extra_data_size / (slots.size() - j - 1);
			drop_from_slot_left += extra_data_size % (slots.size() - j - 1);
		}

		for(uint8_t i = j; i < slots.size() - j; i++)
		{
			if((slot_size - slot_free_sizes[i]) >= drop_from_slot)
			{
				slot_free_sizes[i] += drop_from_slot;
				extra_data_size -= drop_from_slot;
			}
			else
			{
				extra_data_size -= slot_size - slot_free_sizes[i];
				slot_free_sizes[i] = slot_size;
			}
			if(i != (slots.size() - 1 - j))
			{
				drop_from_slot = extra_data_size / (slots.size() - j - i - 1);
				drop_from_slot_left += extra_data_size % (slots.size() - j - i - 1);
			}
			else
				drop_from_slot_left += extra_data_size;
		}

		for(uint8_t i = 0; i < slots.size() - j; i++)
		{
			if((slot_size - slot_free_sizes[i]) >= drop_from_slot_left)
			{
				slot_free_sizes[i] += drop_from_slot_left;
				break;
			}
		}

		return SUCCESS;
	}
}

inline uint16_t TDMA_table::table_get_ms_in_sframe()
{
	return (clock_get_time_ms() + 1 + TX_TIMER_OFFSET_MS) % table_sframe_duration;
}

inline uint16_t TDMA_table::table_get_ms_in_sframe_unicast()
{
	return (clock_get_time_ms() + 1 + TX_TIMER_OFFSET_MS + TDMA_ACK_QUEUE_SIZE) % table_sframe_duration;
}

inline uint16_t TDMA_table::table_get_ms_in_slot()
{
	return table_get_ms_in_sframe() % (table_sframe_duration / table_slot_count);
}

inline uint8_t TDMA_table::table_get_slot_number()
{
	return table_get_ms_in_sframe() * table_slot_count / table_sframe_duration;
}

inline uint8_t TDMA_table::table_get_slot_number_unicast()
{
	return table_get_ms_in_sframe_unicast() * table_slot_count / table_sframe_duration;
}

inline uint16_t TDMA_table::table_get_tx_ms_in_sframe()
{
	return (clock_get_time_ms() + ROUND_TX_MS_TO_HIGHER_VALUE + TX_OFFSET_MS) % table_sframe_duration;
}

inline uint16_t TDMA_table::table_get_tx_ms_in_slot()
{
	return table_get_tx_ms_in_sframe() % (table_sframe_duration / table_slot_count);
}

inline uint8_t TDMA_table::table_get_tx_slot_number()
{
	return table_get_tx_ms_in_sframe() * table_slot_count / table_sframe_duration;
}


TDMA_table::table_operation_status TDMA_table::table_get_slots(std::vector<Ptr<TDMA_slot>> &slots, 	std::vector<std::pair<channel_t, slot_number_t>> &slots_info, TDMA_slot::tdma_slot_type slot_type, bool &current_slot, mac_address_t mac_address)
{
	table_operation_status status = NO_SLOTS_AVAILABLE;
	current_slot = false;
	uint8_t slot_number;

	if(slot_type != TDMA_slot::UNICAST_SEND)
		slot_number = table_get_slot_number();
	else
		slot_number = table_get_slot_number_unicast();

	for(uint8_t i = slot_number; i < table_slot_count; i++)
	{
		for(uint8_t j = 0; j < table_channel_count; j++)
		{
			if(table[std::make_pair(j, i)]->get_slot_type() == slot_type)
			{
				if((slot_type == TDMA_slot::CONTROL_BROADCAST) || ((slot_type == TDMA_slot::UNICAST_SEND || slot_type == TDMA_slot::RECEIVE) && table[std::make_pair(j, i)]->get_remote_mac_address() == mac_address))
				{
					status = SUCCESS;
					slots.push_back(table[std::make_pair(j, i)]);
					slots_info.push_back(std::make_pair(j, i));
				}
			}
		}
	}

	for(uint8_t i = 0; i < slot_number; i++)
	{
		for(uint8_t j = 0; j < table_channel_count; j++)
		{
			if(table[std::make_pair(j, i)]->get_slot_type() == slot_type)
			{
				if((slot_type == TDMA_slot::CONTROL_BROADCAST) || ((slot_type == TDMA_slot::UNICAST_SEND || slot_type == TDMA_slot::RECEIVE) && table[std::make_pair(j, i)]->get_remote_mac_address() == mac_address))
				{
					status = SUCCESS;
					slots.push_back(table[std::make_pair(j, i)]);
					slots_info.push_back(std::make_pair(j, i));
				}
			}
		}
	}

	for(uint8_t j = 0; j < table_channel_count; j++)
	{
		if(table[std::make_pair(j, slot_number)]->get_slot_type() == slot_type)
		{
			if((slot_type == TDMA_slot::CONTROL_BROADCAST) || ((slot_type == TDMA_slot::UNICAST_SEND || slot_type == TDMA_slot::RECEIVE) && table[std::make_pair(j, slot_number)]->get_remote_mac_address() == mac_address))
			{
				status = SUCCESS;
				current_slot = true;
				slots.push_back(table[std::make_pair(j, slot_number)]);
				slots_info.push_back(std::make_pair(j, slot_number));
			}
		}
	}

	return status;
}

inline uint64_t TDMA_table::clock_get_time_ms()
{
    //timespec ts;
    //clock_gettime(CLOCK_REALTIME, &ts);
    //return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000L;
  	return Simulator::Now().GetMilliSeconds();
}

#ifndef USE_SLOTTED_ALOHA
bool TDMA_table::table_randomize_bc_slot(uint16_t &sleep_for, uint16_t ms_in_slot)
{
	static bool sleep_now = true;
	bool specific_value = false;
	uint8_t i;

	for(i = 0; i < table_sframe_duration/(table_slot_count * RANDOMIZED_BC_SLOTS_INTERVAL); i++)
	{
		if(ms_in_slot == specific_values[i])
		{
			specific_value = true;
			break;
		}
	}

	if(specific_value)
	{
		if(i == 0)
			sleep_for = get_random_value(RANDOMIZED_BC_SLOTS_INTERVAL - GUARD_MS_COUNT - 1);
		else
			sleep_for = get_random_value(RANDOMIZED_BC_SLOTS_INTERVAL - 1);

		if(sleep_for == 0)
		{
			sleep_for = 1;
			sleep_now = false;
		}
		else
			sleep_now = true;
	}
	else
	{
		if(sleep_now)
			sleep_for = 1;
		else
			sleep_for = RANDOMIZED_BC_SLOTS_INTERVAL - (ms_in_slot % RANDOMIZED_BC_SLOTS_INTERVAL);

		sleep_now = !sleep_now;
	}

	//printf("Sleep for: %d, sleep_now: %d, ms in slot: %d\r\n", sleep_for, sleep_now, ms_in_slot);
	return !sleep_now;
}

void TDMA_table::table_generate_specific_values()
{
	specific_values.clear();
	specific_values.push_back(GUARD_MS_COUNT);
	for(uint8_t i = 1; i < table_sframe_duration/(table_slot_count * RANDOMIZED_BC_SLOTS_INTERVAL); i++)
		specific_values.push_back(RANDOMIZED_BC_SLOTS_INTERVAL * i);

	/*printf("Specific values: ");
	for(uint8_t i = 0; i < table_sframe_duration/(table_slot_count * RANDOMIZED_BC_SLOTS_INTERVAL); i++)
		printf("%d ", specific_values[i]);
	printf("\r\n");*/
}

void TDMA_table::test_table_randomize_bc_slot()
{
	uint16_t next_wakeup;
	while(1)
	{
		uint16_t ms_in_slot = table_get_tx_ms_in_slot();

		if(table_randomize_bc_slot(next_wakeup, ms_in_slot))
		{
			uint16_t random_number = get_random_value(3) + 1;
			//printf("Packets to send occupies time interval: %d\r\n", random_number);
			usleep(random_number*1000);
		}
		else
		{
			usleep(next_wakeup*1000);
		}
	}
}
#endif

std::pair<uint8_t, slot_reference*> TDMA_table::get_broadcast_slots(){

	return std::make_pair(this->broadcast_slots_count,this->broadcast_slots);
}

bool TDMA_table::table_get_active_slots(std::vector<std::pair<channel_t, slot_number_t>> &slots)
{
	for(uint8_t i = 0; i < table_slot_count; i++)
	{
		for(uint8_t j = 0; j < table_channel_count; j++)
		{
			TDMA_slot::tdma_slot_type type = table[std::make_pair(j, i)]->get_slot_type();
			if(type == TDMA_slot::UNICAST_SEND || type == TDMA_slot::RECEIVE)
				slots.push_back(std::make_pair(j, i));
		}
	}

	if(slots.size() == 0)
		return false;
	else
		return true;
}

#ifdef SLOT_ALLOCATION_EXPOSED_NODE
bool TDMA_table::table_get_active_slots_mode(std::vector<std::pair<channel_t, slot_number_t>> &slots, std::vector<SLOT_MODE> &slots_mode)
{
	for(uint8_t i = 0; i < table_slot_count; i++)
	{
		for(uint8_t j = 0; j < table_channel_count; j++)
		{
			TDMA_slot::tdma_slot_type type = table[std::make_pair(j, i)]->get_slot_type();
			if(type == TDMA_slot::UNICAST_SEND)
			{
				slots.push_back(std::make_pair(j, i));
				slots_mode.push_back(SLOT_MODE::TX);
			}
			if(type == TDMA_slot::RECEIVE)
			{
				slots.push_back(std::make_pair(j, i));
				slots_mode.push_back(SLOT_MODE::RX);
			}
		}
	}

	if(slots.size() == 0)
		return false;
	else
		return true;
}

SLOT_MODE TDMA_table::get_slot_mode(uint8_t channel, uint8_t slot)
{
	TDMA_slot::tdma_slot_type type = table[std::make_pair(channel, slot)]->get_slot_type();
	if(type == TDMA_slot::UNICAST_SEND)
		return SLOT_MODE::TX;
	else if(type == TDMA_slot::RECEIVE)
		return SLOT_MODE::RX;
	else
		return SLOT_MODE::SLOT_MODE_UNKNOWN;
}
#endif

bool TDMA_table::table_get_active_slots_more(std::vector<std::pair<std::pair<channel_t, slot_number_t>, std::pair<TDMA_slot::tdma_slot_type, uint16_t>>> &slots)
{
	for(uint8_t i = 0; i < table_slot_count; i++)
	{
		for(uint8_t j = 0; j < table_channel_count; j++)
		{
			TDMA_slot::tdma_slot_type type = table[std::make_pair(j, i)]->get_slot_type();
			if(type == TDMA_slot::UNICAST_SEND || type == TDMA_slot::RECEIVE || type == TDMA_slot::CONTROL_BROADCAST)
			{
				std::pair<channel_t, slot_number_t> slot = std::make_pair(j, i);
				uint16_t remote_id = table[std::make_pair(j, i)]->get_remote_mac_address() & 0xFFFF;
				slots.push_back(std::make_pair(slot, std::make_pair(type, remote_id)));
			}
		}
	}

	if(slots.size() == 0)
		return false;
	else
		return true;
}

uint8_t TDMA_table::get_active_send_slots_to(mac_address_t dest_mac){

	uint8_t count=0;

	for(uint8_t i = 0; i < table_slot_count; i++)
	{
		for(uint8_t j = 0; j < table_channel_count; j++)
		{
			TDMA_slot::tdma_slot_type type = table[std::make_pair(j, i)]->get_slot_type();
			if(type == TDMA_slot::UNICAST_SEND && table[std::make_pair(j, i)]->get_remote_mac_address()== dest_mac)
				count++;
		}
	}

	return count;
}

}
