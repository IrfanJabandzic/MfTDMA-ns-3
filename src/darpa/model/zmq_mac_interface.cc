/*
 * zmq_mac_interface.cpp
 *
 *  Created on: Oct 23, 2017
 *      Author: Irfan Jabandzic
 */

#include "zmq_mac_interface.h"

namespace ns3 {

void mac_container_populate_ctrl_info(mac_container &container, Basic_ctrl_TRX trx_flag, uint32_t tx_channel,
									  									timestamp_t timestamp, uint32_t mcs, uint32_t tx_gain,
																			uint32_t data_size)
{
	container.ctrl_info.trx_flag = trx_flag;
	container.ctrl_info.tx_channel = tx_channel;
	container.ctrl_info.timestamp = timestamp;
	container.ctrl_info.mcs = mcs;
	container.ctrl_info.tx_gain = tx_gain;
	container.ctrl_info.data_size = data_size;
}

void mac_container_populate_slot_info(mac_container &container, channel_t channel, slot_number_t slot_number,
									  									uint8_t slot_type, uint8_t priority)
{
	container.slot_info.channel = channel;
	container.slot_info.priority = priority;
	container.slot_info.slot_number = slot_number;
	container.slot_info.slot_type = slot_type;
}

void mac_container_populate_port_payload_size(mac_container *container, std::vector<udp_port_t> port, std::vector<uint32_t> payload_size)
{
	container->port = port;
	container->payload_size = payload_size;
}

}
