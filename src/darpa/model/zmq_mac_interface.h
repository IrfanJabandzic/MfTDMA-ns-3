/*
 * zmq_mac_interface.h
 *
 *  Created on: Oct 23, 2017
 *      Author: Irfan Jabandzic
 */

#ifndef ZMQ_MAC_INTERFACE_H_
#define ZMQ_MAC_INTERFACE_H_

#include "tdma_slot.h"
#include "mac_types.h"
#include "ns3/packet.h"

namespace ns3 {

enum zmq_message_type:uint8_t {
	SEND = 1,
	CHANGE_RX_CHANNEL = 2,
	RECEIVED = 3
};

enum Basic_ctrl_TRX {
	TRX_UNKNOWN	= 0,
	RX					= 1,				// Radio needs to Listen on this (slot,ch)
	TX					= 2				// Radio can transmit a message on this (slot, ch)
	//MAY HAVE SOME MORE STATUS IN THE FUTURE
};

typedef struct {
	Basic_ctrl_TRX trx_flag;
	uint32_t tx_channel;
	timestamp_t timestamp;
	uint32_t mcs;
	int32_t tx_gain;
	uint32_t data_size;
} ctrl_container;

typedef struct {
	channel_t channel;
	slot_number_t slot_number;
	uint8_t slot_type;
	uint8_t priority;
} slot_information;

typedef struct {
	uint8_t packets_in_data;
	std::vector<Ptr<Packet>> data;
	std::vector<udp_port_t> port;
	std::vector<uint32_t> payload_size;
	ctrl_container ctrl_info;
	slot_information slot_info;
} mac_container;

void mac_container_populate_ctrl_info(mac_container &container, Basic_ctrl_TRX trx_flag, uint32_t tx_channel,
									  									timestamp_t timestamp, uint32_t mcs, uint32_t tx_gain,
																			uint32_t data_size);

void mac_container_populate_slot_info(mac_container &container, channel_t channel, slot_number_t slot_number,
									  uint8_t slot_type, uint8_t priority = 0);
void mac_container_populate_port_payload_size(mac_container *container, std::vector<udp_port_t> port, std::vector<uint32_t> payload_size);

}

#endif /* ZMQ_MAC_INTERFACE_H_ */
