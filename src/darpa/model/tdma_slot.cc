/*
 * tdma_slot.cpp
 *
 *  Created on: Sep 20, 2017
 *      Author: Irfan Jabandzic
 */

#include "tdma_slot.h"
#include "ns3/log.h"
#include "ns3/packet.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TDMA_slot");

NS_OBJECT_ENSURE_REGISTERED (TDMA_slot);

TypeId
TDMA_slot::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TDMA_slot")
    .SetParent<Object> ()
    .SetGroupName("Darpa")
    .AddConstructor<TDMA_slot> ()
  ;
  return tid;
}

TDMA_slot::TDMA_slot() :
		slot_type(UNUSED), own_mac_address(0), remote_mac_address(0),
												   slot_size(0), slot_last_index(0), slot_sent_index(0), min_mcs(0)
{
  NS_LOG_FUNCTION (this);

	max_slot_size = 0;

	slot_data.reserve(8);
	slot_mcs_gain.reserve(8);
}

void TDMA_slot::TDMA_slot_init(uint16_t slot_size, uint8_t def_min_mcs)
{
  NS_LOG_FUNCTION (this);

	this->slot_size = slot_size;
	max_slot_size = slot_size;
	this->min_mcs = def_min_mcs;

	NS_LOG_DEBUG("Slot created with slot size " << this->slot_size << " and min_mcs "<< (uint16_t)this->min_mcs << "!" );
}

TDMA_slot::tdma_slot_type TDMA_slot::get_slot_type()
{
	//std::lock_guard<std::mutex> lock{slot_mutex};
	return slot_type;
}

mac_address_t TDMA_slot::get_remote_mac_address()
{
	//std::lock_guard<std::mutex> lock{slot_mutex};
	return remote_mac_address;
}

uint16_t TDMA_slot::get_slot_size()
{
	//std::lock_guard<std::mutex> lock{slot_mutex};
	return slot_size;
}

void TDMA_slot::get_slot_info(tdma_slot_type &type, uint16_t &size, mac_address_t &remote_address)
{
	//std::lock_guard<std::mutex> lock{slot_mutex};

	type = this->slot_type;
	size = this->slot_size;
	remote_address = this->remote_mac_address;
}

void TDMA_slot::block_slot()
{
	//std::lock_guard<std::mutex> lock{slot_mutex};

	slot_type = BLOCKED;
	own_mac_address = 0;
	remote_mac_address = 0;
	clear_slot();
}

void TDMA_slot::remove_slot()
{

	//std::lock_guard<std::mutex> lock{slot_mutex};

	slot_size = max_slot_size;
	slot_type = UNUSED;
	own_mac_address = 0;
	remote_mac_address = 0;
	clear_slot();
}

void TDMA_slot::clear_slot()
{
	slot_last_index = 0;
	slot_sent_index = 0;
	clear_data();
}

void TDMA_slot::set_control_broadcast_slot(mac_address_t own_mac_address)
{
	//std::lock_guard<std::mutex> lock{slot_mutex};

	slot_type = CONTROL_BROADCAST;
	this->own_mac_address = own_mac_address;
}

void TDMA_slot::set_receive_slot(mac_address_t own_mac_address, mac_address_t remote_mac_address)
{
	//std::lock_guard<std::mutex> lock{slot_mutex};

	slot_type = RECEIVE;
	slot_size = TDMA_ACK_QUEUE_SIZE - TDMA_ACK_SLOT_GUARD_SPACE;
	this->own_mac_address = own_mac_address;
	this->remote_mac_address = remote_mac_address;
}

void TDMA_slot::set_used_slot(mac_address_t remote_1, mac_address_t remote_2)
{
	//std::lock_guard<std::mutex> lock{slot_mutex};

	slot_type = USED;
	slot_size = TDMA_ACK_QUEUE_SIZE - TDMA_ACK_SLOT_GUARD_SPACE;
	this->own_mac_address = remote_1;
	this->remote_mac_address = remote_2;
}

void TDMA_slot::set_unicast_send_slot(mac_address_t own_mac_address, mac_address_t remote_mac_address)
{
	//std::lock_guard<std::mutex> lock{slot_mutex};

	slot_type = UNICAST_SEND;
	slot_size -= TDMA_ACK_QUEUE_SIZE;
	this->own_mac_address = own_mac_address;
	this->remote_mac_address = remote_mac_address;

}

void TDMA_slot::set_slot_size(uint16_t slot_size)
{
	this->slot_size = slot_size;
	max_slot_size = slot_size;
}

TDMA_slot::queue_operation_status TDMA_slot::push_back(Ptr<Packet> data, mcs_t mcs, uint32_t tx_gain)
{
	//std::lock_guard<std::mutex> lock{slot_mutex};

	if(is_full())
		return QUEUE_FULL;

	push(data, slot_last_index, mcs, tx_gain);

	return SUCCESS;
}

TDMA_slot::queue_operation_status TDMA_slot::push_front(Ptr<Packet> data, mcs_t mcs, uint32_t tx_gain)
{
	//std::lock_guard<std::mutex> lock{slot_mutex};

	push_with_erase(data, 0, mcs, tx_gain);

	return SUCCESS;
}

TDMA_slot::queue_operation_status TDMA_slot::get_data(std::vector<Ptr<Packet>> &data, std::vector<std::pair<mcs_t, uint32_t>> &mcs_gain, uint16_t &last_index)
{
	//std::lock_guard<std::mutex> lock{slot_mutex};

	if(is_empty())
		return QUEUE_EMPTY;
	if(last_index >= slot_last_index)
		last_index = slot_last_index;

	for(uint16_t i = 0; i < last_index; i++, slot_last_index--, slot_sent_index++)
	{
		data.push_back(slot_data[0]);
		mcs_gain.push_back(slot_mcs_gain[0]);
		slot_data.erase(slot_data.begin(), slot_data.begin() + 1);
		slot_mcs_gain.erase(slot_mcs_gain.begin(), slot_mcs_gain.begin() + 1);
	}

	return SUCCESS;
}

TDMA_slot::queue_operation_status TDMA_slot::get_data_mcs_gain(std::vector<Ptr<Packet>> &data, std::pair<mcs_t, uint32_t> &mcs_gain, uint16_t &packet_count)
{
	//std::lock_guard<std::mutex> lock{slot_mutex};

	if(is_empty())
		return QUEUE_EMPTY;
	if(packet_count >= slot_last_index)
		packet_count = slot_last_index;

	uint16_t k;
	mcs_gain = slot_mcs_gain[0];

	for(k = 1; k < packet_count; k++)
	{
		if(mcs_gain.first != slot_mcs_gain[k].first || mcs_gain.second != slot_mcs_gain[k].second)
		{
			packet_count = k;
			break;
		}
	}

	for(uint16_t i = 0; i < k; i++, slot_last_index--, slot_sent_index++)
	{
		data.push_back(slot_data[0]);
		slot_data.erase(slot_data.begin(), slot_data.begin() + 1);
		slot_mcs_gain.erase(slot_mcs_gain.begin(), slot_mcs_gain.begin() + 1);
	}
	return SUCCESS;
}

void TDMA_slot::push(Ptr<Packet> data, uint16_t index, mcs_t mcs, uint32_t tx_gain)
{
	slot_data.insert(slot_data.begin() + index, data);
	slot_mcs_gain.insert(slot_mcs_gain.begin() + index, std::make_pair(mcs, tx_gain));
	if(get_free_size() == 0)
		slot_sent_index--;
	slot_last_index++;
}

void TDMA_slot::push_with_erase(Ptr<Packet> data, uint16_t index, mcs_t mcs, uint32_t tx_gain)
{
	if(is_full())
	{
		slot_data.erase(slot_data.end() - 1);
		slot_mcs_gain.erase(slot_mcs_gain.end() - 1);
		slot_last_index--;
	}
	else
	{
		if(get_free_size() == 0)
			slot_sent_index--;
	}

	slot_data.insert(slot_data.begin() + index, data);
	slot_mcs_gain.insert(slot_mcs_gain.begin() + index, std::make_pair(mcs, tx_gain));
	slot_last_index++;
}

void TDMA_slot::clear_data()
{
	if(slot_data.size() != 0)
		slot_data.erase(slot_data.begin(), slot_data.end());
	if(slot_mcs_gain.size() != 0)
		slot_mcs_gain.erase(slot_mcs_gain.begin(), slot_mcs_gain.end());
}

void TDMA_slot::print_slot_info()
{
	if(slot_type == UNUSED)
	{
		printf("UN ");
		return;
	}
	else if(slot_type == BLOCKED)
	{
		printf("BL|");
	}
	else if(slot_type == CONTROL_BROADCAST)
	{
		printf("CB|");
	}
	else if(slot_type == RECEIVE)
	{
		printf("RE|");
	}
	else if(slot_type == UNICAST_SEND)
	{
		printf("US|");
	}
	printf("P:%d|", slot_last_index);
	printf("M:%lu ", remote_mac_address);
}

}
