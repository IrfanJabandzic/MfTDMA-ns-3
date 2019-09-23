/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "darpa-wireless-channel.h"
#include <algorithm>
#include "mftdma-mac-net-device.h"
#include "mftdma-mac.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("DarpaWirelessChannel");

NS_OBJECT_ENSURE_REGISTERED (DarpaWirelessChannel);

TypeId
DarpaWirelessChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::DarpaWirelessChannel")
    .SetParent<Channel> ()
    .SetGroupName("Darpa")
    .AddConstructor<DarpaWirelessChannel> ()
		.AddTraceSource ("SendPackets",
										 "Send packets count.",
										 MakeTraceSourceAccessor (&DarpaWirelessChannel::send_packets),
										 "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("DropPacketsControl",
										 "Drop control packets count.",
										 MakeTraceSourceAccessor (&DarpaWirelessChannel::drop_packets_control),
										 "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("DropPacketsData",
										 "Drop data packets count.",
										 MakeTraceSourceAccessor (&DarpaWirelessChannel::drop_packets_data),
										 "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("DropPacketsControlInterference",
										 "Drop data packets interference count.",
										 MakeTraceSourceAccessor (&DarpaWirelessChannel::drop_packets_control_interference),
										 "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("DropPacketsDataInterference",
										 "Drop data packets interference count.",
										 MakeTraceSourceAccessor (&DarpaWirelessChannel::drop_packets_data_interference),
										 "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("AddedPackets",
										 "Added packets count.",
										 MakeTraceSourceAccessor (&DarpaWirelessChannel::added_packets),
										 "ns3::TracedValueCallback::Uint32")
    .AddAttribute ("Delay", "Transmission delay through the channel",
                   TimeValue (Seconds (0)),
                   MakeTimeAccessor (&DarpaWirelessChannel::m_delay),
                   MakeTimeChecker ())
		.AddAttribute ("Neighbors", "Number of neighbors",
									 UintegerValue (2),
									 MakeUintegerAccessor (&DarpaWirelessChannel::m_neighbors),
									 MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

DarpaWirelessChannel::DarpaWirelessChannel () :
		mobility_active(false),
		added_packets(0),
		drop_packets_control(0),
		drop_packets_data(0),
		drop_packets_control_interference(0),
		drop_packets_data_interference(0),
		send_packets(0)
{
  NS_LOG_FUNCTION (this);
}

void
DarpaWirelessChannel::DarpaWirelessChannelInit (uint32_t nodes_count)
{
  m_devices.reserve(nodes_count + 5);
  m_mac.reserve(nodes_count + 5);
  m_queue.reserve(nodes_count * 3);
}

void
DarpaWirelessChannel::Send (Ptr<Packet> p, uint16_t protocol,
                     Mac48Address to, Mac48Address from, channel_t tx_channel,
                     Ptr<MFTDMAMac> sender)
{
  NS_LOG_FUNCTION (this << p << protocol << to << from << sender);

  uint32_t packets_dropped = 0;

  if(!CheckCollisionDropPackets(packets_dropped))
	{
		for (std::vector<Ptr<MFTDMAMac> >::const_iterator i = m_mac.begin (); i != m_mac.end (); ++i)
			{
				Ptr<MFTDMAMac> tmp = *i;
				if (tmp == sender)
					{
						send_packets++;
						continue;
					}
				if (tx_channel != tmp->GetRxChannel())
					{
						NS_LOG_INFO("Tx channel " << (uint16_t)tx_channel << "Rx channel " << (uint16_t)tmp->GetRxChannel());
						continue;
					}

				if(!to.IsBroadcast() && tmp->GetAddress() != to)
					{
					 continue;
					}

#ifdef MULTIHOP
				if(to.IsBroadcast() && !IsNodeInRange (from, tmp->GetAddress()))
					{
						//NS_LOG_UNCOND("Skip sending broadcast from " << 	GetNodeID(from) << " to " << GetNodeID(tmp->GetAddress()));
						continue;
					}
#endif

				Simulator::ScheduleWithContext (tmp->GetDevice()->GetNode ()->GetId (), m_delay,
																				&MFTDMAMac::Receive, tmp, p->Copy (), protocol, to, from);
			}
	}
  else
	{
		//NS_LOG_ERROR("Collision occured, number of packets dropped: " << packets_dropped);
	}
  SendScheduleNext();
}

void
DarpaWirelessChannel::AddPacket (Ptr<Packet> p, uint16_t protocol, Mac48Address to, Mac48Address from,
																 channel_t tx_channel, timestamp_t phy_timestamp, uint8_t slot_type, slot_number_t slot_number,
                   	 	 					 Ptr<MFTDMAMac> sender)
{
	added_packets++;

	phy_container container = {p, protocol, to, from, tx_channel, phy_timestamp, slot_type, slot_number, sender};

	//std::lock_guard<std::mutex> lock_channel{channel_mutex};

	m_queue.push_back(container);

	if(m_queue.size() == 1)
		ScheduleNext();
}

inline void
DarpaWirelessChannel::ScheduleNext (void)
{
	Simulator::Schedule (NanoSeconds((m_queue[0].phy_timestamp * 1000000) - Simulator::Now().GetNanoSeconds()),
                     	 &DarpaWirelessChannel::Send, this, m_queue[0].p, m_queue[0].protocol, m_queue[0].to, m_queue[0].from, m_queue[0].tx_channel, m_queue[0].sender);
}

inline void
DarpaWirelessChannel::SendScheduleNext (void)
{
	//std::lock_guard<std::mutex> lock_channel{channel_mutex};

	m_queue.erase(m_queue.begin(), m_queue.begin() + 1);

	if(m_queue.size() != 0)
		ScheduleNext();
}

bool
DarpaWirelessChannel::CheckCollisionDropPackets (uint32_t &packets_dropped)
{
	bool collision = false;
	packets_dropped = 0;

	//std::lock_guard<std::mutex> lock_channel{channel_mutex};

	timestamp_t timestamp = m_queue[0].phy_timestamp;
	channel_t tx_channel = m_queue[0].tx_channel;
	uint8_t slot_type = m_queue[0].slot_type;
	slot_number_t slot_number = m_queue[0].slot_number;
	Mac48Address from = m_queue[0].from;
#ifdef SIMULATE_EXPOSED_NODE
	Mac48Address to = m_queue[0].to;
#endif

	for(uint16_t i = 0; i < m_interference_slots.size(); i++)
	{
		if(tx_channel == m_interference_slots[i].first && slot_number == m_interference_slots[i].second)
		{
			collision = true;
			packets_dropped++;
			break;
		}
	}

	if(!collision)
	{
		for(uint32_t i = 1; i < m_queue.size(); i++)
		{
#ifdef MULTIHOP
#ifndef	SIMULATE_EXPOSED_NODE
			if(IsNodeInRange (from, m_queue[i].from))
#else
			if(IsNodeInRange (from, m_queue[i].to))
#endif
			{
#endif
				if(timestamp == m_queue[i].phy_timestamp)
				{
					if(tx_channel == m_queue[i].tx_channel)
					{
#ifdef	SIMULATE_EXPOSED_NODE
					if(IsNodeInRange (m_queue[i].from, to))
						collision = true;
#else
					collision = true;
#endif
						m_queue.erase(m_queue.begin() + i);
						packets_dropped++;
						i--;
					}
				}
				else
				{
					break;
				}
#ifdef MULTIHOP
			}
#endif
		}

		if(packets_dropped != 0)
		{
			packets_dropped++;

			if(slot_type == TDMA_slot::CONTROL_BROADCAST)
			{
				drop_packets_control += packets_dropped;
			}
			else if(slot_type == TDMA_slot::UNICAST_SEND)
			{
				drop_packets_data += packets_dropped;
				NS_LOG_INFO("Packets dropped type data " << packets_dropped << " Ch-Time (" << (uint16_t)tx_channel << "-" << (uint16_t)slot_number << ")");
			}
			else
				NS_LOG_INFO("Other slot type dropped " << (uint16_t)slot_type);
		}
	}
	else
	{
		if(slot_type == TDMA_slot::CONTROL_BROADCAST)
		{
			drop_packets_control_interference += packets_dropped;
		}
		else if(slot_type == TDMA_slot::UNICAST_SEND)
		{
			drop_packets_data_interference += packets_dropped;
			NS_LOG_INFO("Packets interfered type data " << packets_dropped << " Ch-Time (" << (uint16_t)tx_channel << "-" << (uint16_t)slot_number << ")");
		}
		else
			NS_LOG_INFO("Other slot type interfered " << (uint16_t)slot_type);
	}

	return collision;
}

inline int16_t
DarpaWirelessChannel::GetNodeID (Mac48Address node)
{
	uint8_t mac_address[6];
	node.CopyTo (mac_address);

	return mac_address[4] << 8 | mac_address[5];
}

inline bool
DarpaWirelessChannel::IsNodeInRange (Mac48Address node1, Mac48Address node2)
{
	bool node_in_range = false;
	int16_t node1_id = GetNodeID(node1);
	int16_t node2_id = GetNodeID(node2);
	uint32_t nodes = m_devices.size ();

#ifdef SIMULATE_EXPOSED_NODE
	if((node1_id >= 1 && node1_id <= nodes/2) && (node2_id >= 1 && node2_id <= nodes/2))
		return true;
	if((node1_id > nodes/2 && node1_id <= nodes) && (node2_id > nodes/2 && node2_id <= nodes))
		return true;
	//if((node1_id == 2 && node2_id == 3) || (node1_id == 3 && node2_id == 2))
	if((node1_id > (nodes/4) && node1_id <= (3*nodes/4)) && (node2_id > (nodes/4) && node2_id <= (3*nodes/4)))
		return true;

	return false;
#endif

	if(mobility_active)
	{
		if((node1_id >= 1 && node1_id <= nodes/2) && (node2_id >= 1 && node2_id <= nodes/2))
			return true;
		if((node1_id > nodes/2 && node1_id <= nodes) && (node2_id > nodes/2 && node2_id <= nodes))
			return true;

		return false;
	}

	int16_t lower_limit = node1_id - m_neighbors/2;
	int16_t upper_limit = node1_id + m_neighbors/2 + m_neighbors%2;
	//NS_LOG_UNCOND("ID1 " << node1_id << " ID2 " << node2_id << " Nodes " << nodes << " lower_limit " << lower_limit << " upper_limit " << upper_limit);
	if(lower_limit < 1)
	{
		lower_limit = nodes + lower_limit;
		if((node2_id >= lower_limit && node2_id <= nodes) || (node2_id >= 1 && node2_id <= node1_id))
			node_in_range = true;
	}
	else
	{
		if(node2_id >= lower_limit && node2_id <= node1_id)
			node_in_range = true;
	}

	if(upper_limit > nodes)
	{
		upper_limit = upper_limit % nodes;
		if((node2_id >= 1 && node2_id <= upper_limit) || (node2_id >= node1_id && node2_id <= nodes))
			node_in_range = true;
	}
	else
	{
		if(node2_id >= node1_id && node2_id <= upper_limit)
			node_in_range = true;
	}

	//NS_LOG_UNCOND("node_in_range " << node_in_range << " lower_limit " << lower_limit << " upper_limit " << upper_limit);
	return node_in_range;
}

void
DarpaWirelessChannel::AddInterferenceSlot(channel_t channel, slot_number_t slot_number)
{
	std::pair<channel_t, slot_number_t> add_pair = std::make_pair(channel, slot_number);
	for (std::vector<std::pair<channel_t, slot_number_t>>::iterator it = m_interference_slots.begin(); it != m_interference_slots.end(); ++it)
	{
		if(add_pair == *it)
			return;
	}

	m_interference_slots.push_back(add_pair);
}

void
DarpaWirelessChannel::RemoveInterferenceSlot(channel_t channel, slot_number_t slot_number)
{
	std::pair<channel_t, slot_number_t> remove_pair = std::make_pair(channel, slot_number);
	for (std::vector<std::pair<channel_t, slot_number_t>>::iterator it = m_interference_slots.begin(); it != m_interference_slots.end(); ++it)
	{
		if(remove_pair == *it)
		{
			m_interference_slots.erase(it);
			return;
		}
	}
}

void
DarpaWirelessChannel::RemoveInterferenceSlots(void)
{
	m_interference_slots.clear();
}

static bool interference_printed = false;

void
DarpaWirelessChannel::PrintInterferenceSlots(Ptr<OutputStreamWrapper> log)
{
	if(!interference_printed)
	{
		interference_printed = true;
		for(uint16_t i = 0; i < m_interference_slots.size(); i++)
			NS_LOG_UNCOND("Interference:" << (uint16_t)m_interference_slots[i].first << "\t" << (uint16_t)m_interference_slots[i].second);

		for(uint16_t i = 0; i < m_interference_slots.size(); i++)
			*log->GetStream () << "Interference:" << (uint16_t)m_interference_slots[i].first << "\t" << (uint16_t)m_interference_slots[i].second << std::endl;
	}
}

void
DarpaWirelessChannel::ChangeMobility(bool change_mobility)
{
	mobility_active = change_mobility;
}

uint16_t
DarpaWirelessChannel::GetNeighbors(void)
{
	return m_neighbors;
}

void
DarpaWirelessChannel::Add (Ptr<MFTDMAMacNetDevice> device)
{
  NS_LOG_FUNCTION (this << device);
  m_devices.push_back (device);
  m_mac.push_back (device->GetMac());
}

uint32_t
DarpaWirelessChannel::GetNDevices (void) const
{
  NS_LOG_FUNCTION (this);
  return m_devices.size ();
}

Ptr<NetDevice>
DarpaWirelessChannel::GetDevice (uint32_t i) const
{
  NS_LOG_FUNCTION (this << i);
  return m_devices[i];
}

uint32_t
DarpaWirelessChannel::GetNMac (void) const
{
	NS_LOG_FUNCTION (this);
	return m_mac.size ();
}

Ptr<MFTDMAMac>
DarpaWirelessChannel::GetMac (uint32_t i) const
{
	NS_LOG_FUNCTION (this << i);
	return m_mac[i];
}

}

