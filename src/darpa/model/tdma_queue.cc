/*
 * tdma_queue.cpp
 *
 *  Created on: Oct 29, 2017
 *      Author: Irfan Jabandzic
 */

#include "tdma_queue.h"
#include "ns3/packet.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TDMA_queue");

NS_OBJECT_ENSURE_REGISTERED (TDMA_queue);

TypeId
TDMA_queue::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TDMA_queue")
    .SetParent<Object> ()
    .SetGroupName("Darpa")
    .AddConstructor<TDMA_queue> ()
  ;
  return tid;
}

TDMA_queue::TDMA_queue() : queue_type(UNICAST), own_mac_address(0),
													 remote_mac_address(0), queue_timeout(0),
													 queue_first_index(0), queue_last_index(0)
{
  NS_LOG_FUNCTION (this);
}

void TDMA_queue::TDMA_queue_init(tdma_queue_type queue_type, uint64_t queue_timeout, mac_address_t own_mac_address, mac_address_t remote_mac_address)
{
  NS_LOG_FUNCTION (this);

  queue_data.reserve((queue_timeout/1000000) * 2);

	this->queue_type = queue_type;
	this->own_mac_address = own_mac_address;
	this->remote_mac_address = remote_mac_address;
	this->queue_timeout = queue_timeout;
}

uint16_t TDMA_queue::get_queue_retransmission_size() const
{
	uint16_t retr_count = 0;

	for(uint16_t i = 0; i < queue_first_index; i++)
	{
		if(queue_data[i].packet_type == RETRANSMISSION)
			retr_count++;
	}

	return retr_count;
}

uint16_t TDMA_queue::get_queue_high_priority_data_size() const
{
	uint16_t data_count = 0;

	for(uint16_t i = 0; i < queue_first_index; i++)
	{
		if(queue_data[i].packet_type == APP_HIGH_PRIORITY)
			data_count++;
	}

	return data_count;
}

uint16_t TDMA_queue::get_queue_broadcast_size() const
{
	uint16_t broadcast_count = 0;

	for(uint16_t i = 0; i < queue_first_index; i++)
	{
		if(queue_data[i].packet_type == BROADCAST)
			broadcast_count++;
	}

	return broadcast_count;
}

uint16_t TDMA_queue::get_queue_control_size() const
{
	uint16_t control_count = 0;

	for(uint16_t i = 0; i < queue_first_index; i++)
	{
		if(queue_data[i].packet_type == CONTROL)
			control_count++;
	}

	return control_count;
}

TDMA_queue::queue_operation_status TDMA_queue::push_back_high_priority(Ptr<Packet> data, TDMA_queue::tdma_queue_packet_type packet_type, mcs_t mcs, uint32_t tx_gain, udp_port_t port, uint32_t payload_size, timestamp_t timestamp)
{
	queue_container temp_container;

	temp_container.packet_type = packet_type;
	temp_container.data = data;
	temp_container.queue_mcs_gain = std::make_pair(mcs, tx_gain);
	temp_container.port = port;
	temp_container.payload_size = payload_size;
	temp_container.queue_timestamp = timestamp;

	//std::lock_guard<std::mutex> lock_queue{tdma_queue_mutex};

	queue_data.insert(queue_data.begin() + queue_first_index, temp_container);

	queue_first_index++;
	queue_last_index++;

	return SUCCESS;
}

TDMA_queue::queue_operation_status TDMA_queue::push_front_high_priority(Ptr<Packet> data, TDMA_queue::tdma_queue_packet_type packet_type, mcs_t mcs, uint32_t tx_gain, udp_port_t port, uint32_t payload_size, timestamp_t timestamp)
{
	queue_container temp_container;

	temp_container.packet_type = packet_type;
	temp_container.data = data;
	temp_container.queue_mcs_gain = std::make_pair(mcs, tx_gain);
	temp_container.port = port;
	temp_container.payload_size = payload_size;
	temp_container.queue_timestamp = timestamp;

	//std::lock_guard<std::mutex> lock_queue{tdma_queue_mutex};

	if(packet_type == RETRANSMISSION || packet_type == BROADCAST || packet_type == CONTROL)
		queue_data.insert(queue_data.begin(), temp_container);
	else
	{
		uint16_t i = 0;
		for(; i < queue_first_index; i++)
		{
			if(queue_data[i].packet_type != RETRANSMISSION)
				break;
		}
		queue_data.insert(queue_data.begin() + i, temp_container);
	}


	queue_first_index++;
	queue_last_index++;

	return SUCCESS;
}

TDMA_queue::queue_operation_status TDMA_queue::push_back_low_priority(Ptr<Packet> data, TDMA_queue::tdma_queue_packet_type packet_type, mcs_t mcs, uint32_t tx_gain, udp_port_t port, uint32_t payload_size, timestamp_t timestamp)
{
	queue_container temp_container;

	temp_container.packet_type = packet_type;
	temp_container.data = data;
	temp_container.queue_mcs_gain = std::make_pair(mcs, tx_gain);
	temp_container.port = port;
	temp_container.payload_size = payload_size;
	temp_container.queue_timestamp = timestamp;

	//std::lock_guard<std::mutex> lock_queue{tdma_queue_mutex};

	queue_data.insert(queue_data.begin() + queue_last_index, temp_container);

	queue_last_index++;

	return SUCCESS;
}

TDMA_queue::queue_operation_status TDMA_queue::push_front_low_priority(Ptr<Packet> data, TDMA_queue::tdma_queue_packet_type packet_type, mcs_t mcs, uint32_t tx_gain, udp_port_t port, uint32_t payload_size, timestamp_t timestamp)
{
	queue_container temp_container;

	temp_container.packet_type = packet_type;
	temp_container.data = data;
	temp_container.queue_mcs_gain = std::make_pair(mcs, tx_gain);
	temp_container.port = port;
	temp_container.payload_size = payload_size;
	temp_container.queue_timestamp = timestamp;

	//std::lock_guard<std::mutex> lock_queue{tdma_queue_mutex};

	queue_data.insert(queue_data.begin() + queue_first_index, temp_container);

	queue_last_index++;

	return SUCCESS;
}

TDMA_queue::queue_operation_status TDMA_queue::push_to_position_low_priority(Ptr<Packet> data, TDMA_queue::tdma_queue_packet_type packet_type, mcs_t mcs, uint32_t tx_gain, udp_port_t port, uint32_t payload_size, timestamp_t timestamp, uint16_t position)
{
	queue_container temp_container;

	temp_container.packet_type = packet_type;
	temp_container.data = data;
	temp_container.queue_mcs_gain = std::make_pair(mcs, tx_gain);
	temp_container.port = port;
	temp_container.payload_size = payload_size;
	temp_container.queue_timestamp = timestamp;

	//std::lock_guard<std::mutex> lock_queue{tdma_queue_mutex};

	if((queue_first_index + position) > queue_data.size())
		queue_data.push_back(temp_container);
	else
		queue_data.insert(queue_data.begin() + queue_first_index + position, temp_container);

	queue_last_index++;

	return SUCCESS;
}

TDMA_queue::queue_operation_status TDMA_queue::queue_get_data(std::vector<Ptr<Packet>> &data, std::vector<std::pair<mcs_t, uint32_t>> &mcs_gain, std::vector<udp_port_t> &port, std::vector<uint32_t> &payload_size, uint16_t &last_index)
{
	//std::lock_guard<std::mutex> lock_queue{tdma_queue_mutex};

	if(is_empty())
		return QUEUE_EMPTY;
	if(last_index >= queue_last_index)
		last_index = queue_last_index;

	for(uint16_t i = 0; i < last_index; i++, queue_last_index--)
	{
		data.push_back(queue_data[0].data);
		mcs_gain.push_back(queue_data[0].queue_mcs_gain);
		port.push_back(queue_data[0].port);
		payload_size.push_back(queue_data[0].payload_size);
		queue_data.erase(queue_data.begin(), queue_data.begin() + 1);
		if(queue_first_index != 0)
			queue_first_index--;
	}

	return SUCCESS;
}

TDMA_queue::queue_operation_status TDMA_queue::queue_get_data_mcs_gain(std::vector<Ptr<Packet>> &data, std::pair<mcs_t, uint32_t> &mcs_gain, std::vector<udp_port_t> &port, std::vector<uint32_t> &payload_size, uint16_t &packet_count, uint16_t &reduce_value)
{
	//std::lock_guard<std::mutex> lock_queue{tdma_queue_mutex};

	if(is_empty())
		return QUEUE_EMPTY;
	if(packet_count >= queue_last_index)
		packet_count = queue_last_index;

	uint16_t k;
	mcs_gain = queue_data[0].queue_mcs_gain;

	for(k = 1; k < packet_count; k++)
	{
		if(mcs_gain.first != queue_data[k].queue_mcs_gain.first || mcs_gain.second != queue_data[k].queue_mcs_gain.second)
		{
			packet_count = k;
			break;
		}
	}

	for(uint16_t i = 0; i < k; i++, queue_last_index--)
	{
		data.push_back(queue_data[0].data);
		port.push_back(queue_data[0].port);
		payload_size.push_back(queue_data[0].payload_size);
		queue_data.erase(queue_data.begin(), queue_data.begin() + 1);
		if(queue_first_index != 0)
			queue_first_index--;
		else
			reduce_value++;
	}
	return SUCCESS;
}

uint16_t TDMA_queue::queue_check_timeout(timestamp_t timestamp)
{
	uint16_t packets_dropped=0;

	//std::lock_guard<std::mutex> lock_queue{tdma_queue_mutex};

#if DATA_ORDER == OLDER_DATA_LAST
	int16_t i = queue_last_index - 1;
	for(; i >= (int16_t)queue_first_index; i--)
	{
        if((queue_data[i].queue_timestamp + queue_timeout) < timestamp)
        {
        	queue_data.erase(queue_data.begin() + i);
        	queue_last_index--;
        	packets_dropped++;
        }
        else
        	break;
	}

#ifdef FILTER_HIGH_PRIORITY_APP_DATA
	for(i = (int16_t)(queue_first_index - 1); i >= 0; i--)
	{
		if(queue_data[i].packet_type == APP_HIGH_PRIORITY)
		{
	        if((queue_data[i].queue_timestamp + queue_timeout) < timestamp)
	        {
	        	queue_data.erase(queue_data.begin() + i);
	        	queue_last_index--;
	        	queue_first_index--;
	        	packets_dropped++;
	        }
	        else
	        	break;
		}
		else
			break;
	}
#endif
#elif DATA_ORDER == OLDER_DATA_FIRST
	int16_t i;
	for(i = (int16_t)queue_first_index; i < (int16_t)queue_last_index; i++)
	{
        if((queue_data[i].queue_timestamp + queue_timeout) < timestamp)
        {
        	queue_data.erase(queue_data.begin() + i);
        	i--;
        	queue_last_index--;
        	packets_dropped++;
        }
        else
        	break;
	}

#ifdef FILTER_HIGH_PRIORITY_APP_DATA
	for(i = 0; i < (int16_t)queue_first_index; i++)
	{
		if(queue_data[i].packet_type == APP_HIGH_PRIORITY)
		{
	        if((queue_data[i].queue_timestamp + queue_timeout) < timestamp)
	        {
	        	queue_data.erase(queue_data.begin() + i);
	        	i--;
	        	queue_last_index--;
	        	queue_first_index--;
	        	packets_dropped++;
	        }
	        else
	        	break;
		}
	}
#endif
#endif

	return packets_dropped;
}

TDMA_queue::queue_operation_status TDMA_queue::test_data()
{
	queue_operation_status status = SUCCESS;

	return status;
}

void TDMA_queue::clear_data()
{
	//std::lock_guard<std::mutex> lock_queue{tdma_queue_mutex};

	queue_data.erase(queue_data.begin(), queue_data.end());
}

void TDMA_queue::print_queue_info()
{
	printf("Queue initialized with own mac address: %lu and remote mac address: %lu\r\n", own_mac_address, remote_mac_address);

	if(queue_type == UNICAST)
		printf("Queue initialized with type UNICAST.\r\n");

	else if(queue_type == BROADCAST_CONTROL)
		printf("Queue initialized with type BROADCAST_CONTROL.\r\n");

	printf("Total size of queue: %d\r\n", get_queue_size());

	if(queue_type == UNICAST)
		printf("Retransmission packets: %d || High priority data packets: %d || Low priority data packets: %d\r\n", get_queue_retransmission_size(),
																													get_queue_high_priority_data_size(),
																													get_queue_low_priority_size());

	else if(queue_type == BROADCAST_CONTROL)
		printf("Broadcast packets: %d || Control packets: %d\r\n", get_queue_broadcast_size(), get_queue_control_size());
}

}
