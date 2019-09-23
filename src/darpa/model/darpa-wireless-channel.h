/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef DARPA_WIRELESS_CHANNEL_H
#define DARPA_WIRELESS_CHANNEL_H

#include "ns3/channel.h"
#include "ns3/nstime.h"
#include "ns3/mac48-address.h"
#include "mac_types.h"
#include <vector>
#include <map>
//#include <mutex>
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/output-stream-wrapper.h"

namespace ns3 {

//#define MULTIHOP
//#define SIMULATE_EXPOSED_NODE

class Packet;
class MFTDMAMacNetDevice;
class MFTDMAMac;

class DarpaWirelessChannel : public Channel
{
public:

	typedef struct {
		Ptr<Packet> p;
		uint16_t protocol;
		Mac48Address to;
		Mac48Address from;
		channel_t tx_channel;
		timestamp_t phy_timestamp;
		uint8_t slot_type;
		slot_number_t slot_number;
		Ptr<MFTDMAMac> sender;
	} phy_container;

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  DarpaWirelessChannel ();

  void DarpaWirelessChannelInit (uint32_t nodes_count);
  /**
   * A packet is sent by a net device.  A receive event will be
   * scheduled for all net device connected to the channel other
   * than the net device who sent the packet
   *
   * \param p packet to be sent
   * \param protocol protocol number
   * \param to address to send packet to
   * \param from address the packet is coming from
   * \param sender netdevice who sent the packet
   *
   */
  virtual void Send (Ptr<Packet> p, uint16_t protocol, Mac48Address to, Mac48Address from, channel_t tx_channel,
                     Ptr<MFTDMAMac> sender);

  virtual void AddPacket (Ptr<Packet> p, uint16_t protocol, Mac48Address to, Mac48Address from,
  												channel_t tx_channel, timestamp_t phy_timestamp, uint8_t slot_type, slot_number_t slot_number,
                     	 	 	Ptr<MFTDMAMac> sender);

  void ScheduleNext (void);

  void SendScheduleNext (void);

  bool CheckCollisionDropPackets (uint32_t &packets_dropped);

  int16_t GetNodeID (Mac48Address node);

  bool IsNodeInRange (Mac48Address node1, Mac48Address node2);

  void AddInterferenceSlot(channel_t channel, slot_number_t slot_number);
  void RemoveInterferenceSlot(channel_t channel, slot_number_t slot_number);
  void RemoveInterferenceSlots(void);
  void PrintInterferenceSlots(Ptr<OutputStreamWrapper> log);

  void ChangeMobility(bool);

  uint16_t GetNeighbors(void);

  /**
   * Attached a net device to the channel.
   *
   * \param device the device to attach to the channel
   */
  virtual void Add (Ptr<MFTDMAMacNetDevice> device);

  // inherited from ns3::Channel
  virtual uint32_t GetNDevices (void) const;
  virtual Ptr<NetDevice> GetDevice (uint32_t i) const;

  virtual uint32_t GetNMac (void) const;
  virtual Ptr<MFTDMAMac> GetMac (uint32_t i) const;

private:
  Time m_delay; //!< The assigned speed-of-light delay of the channel
  uint16_t m_neighbors;
  std::vector<Ptr<MFTDMAMacNetDevice> > m_devices; //!< devices connected by the channel
  std::vector<Ptr<MFTDMAMac> > m_mac; //!< devices connected by the channel
  std::vector<phy_container> m_queue;

  std::vector<std::pair<channel_t, slot_number_t>> m_interference_slots;

  //std::mutex channel_mutex;

  bool mobility_active;
  TracedValue<uint32_t> added_packets;
	TracedValue<uint32_t> drop_packets_control;
	TracedValue<uint32_t> drop_packets_data;
	TracedValue<uint32_t> drop_packets_control_interference;
	TracedValue<uint32_t> drop_packets_data_interference;
	TracedValue<uint32_t> send_packets;
};

}

#endif /* DARPA_WIRELESS_CHANNEL_H */
