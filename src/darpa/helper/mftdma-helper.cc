/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */



#include "ns3/abort.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/object-factory.h"
#include "ns3/queue.h"
#include "ns3/mftdma-mac-net-device.h"
#include "ns3/darpa-wireless-channel.h"
#include "ns3/config.h"
#include "ns3/packet.h"
#include "ns3/names.h"
#include "ns3/boolean.h"
#include "ns3/trace-helper.h"
#include "mftdma-helper.h"
#include <string>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MFTDMAHelper");

MFTDMAHelper::MFTDMAHelper ()
{
  m_queueFactory.SetTypeId ("ns3::DropTailQueue<Packet>");
  m_deviceFactory.SetTypeId ("ns3::MFTDMAMacNetDevice");
  m_channelFactory.SetTypeId ("ns3::DarpaWirelessChannel");
  m_pointToPointMode = false;
}

void
MFTDMAHelper::SetQueue (std::string type,
                                 std::string n1, const AttributeValue &v1,
                                 std::string n2, const AttributeValue &v2,
                                 std::string n3, const AttributeValue &v3,
                                 std::string n4, const AttributeValue &v4)
{
  QueueBase::AppendItemTypeIfNotPresent (type, "Packet");

  m_queueFactory.SetTypeId (type);
  m_queueFactory.Set (n1, v1);
  m_queueFactory.Set (n2, v2);
  m_queueFactory.Set (n3, v3);
  m_queueFactory.Set (n4, v4);
}

void
MFTDMAHelper::SetChannel (std::string type,
                                   std::string n1, const AttributeValue &v1,
                                   std::string n2, const AttributeValue &v2,
                                   std::string n3, const AttributeValue &v3,
                                   std::string n4, const AttributeValue &v4)
{
  m_channelFactory.SetTypeId (type);
  m_channelFactory.Set (n1, v1);
  m_channelFactory.Set (n2, v2);
  m_channelFactory.Set (n3, v3);
  m_channelFactory.Set (n4, v4);
}

void
MFTDMAHelper::SetDeviceAttribute (std::string n1, const AttributeValue &v1)
{
  m_deviceFactory.Set (n1, v1);
}

void
MFTDMAHelper::SetChannelAttribute (std::string n1, const AttributeValue &v1)
{
  m_channelFactory.Set (n1, v1);
}

void
MFTDMAHelper::SetNetDevicePointToPointMode (bool pointToPointMode)
{
  m_pointToPointMode = pointToPointMode;
}

NetDeviceContainer
MFTDMAHelper::Install (Ptr<Node> node, uint32_t max_band, uint32_t channel_step, uint32_t init_tx_gain, uint32_t slots_count, uint8_t cb_slots_count, uint8_t index, Ptr<OutputStreamWrapper> log) const
{
  Ptr<DarpaWirelessChannel> channel = m_channelFactory.Create<DarpaWirelessChannel> ();
  return Install (node, channel, max_band, channel_step, init_tx_gain, slots_count, cb_slots_count, index, log);
}

NetDeviceContainer
MFTDMAHelper::Install (Ptr<Node> node, Ptr<DarpaWirelessChannel> channel, uint32_t max_band, uint32_t channel_step, uint32_t init_tx_gain, uint32_t slots_count, uint8_t cb_slots_count, uint8_t index, Ptr<OutputStreamWrapper> log) const
{
  return NetDeviceContainer (InstallPriv (node, channel, max_band, channel_step, init_tx_gain, slots_count, cb_slots_count, index, log));
}

NetDeviceContainer
MFTDMAHelper::Install (const NodeContainer &c, uint32_t max_band, uint32_t channel_step, uint32_t init_tx_gain, uint32_t slots_count, uint8_t cb_slots_count, Ptr<OutputStreamWrapper> log) const
{
  Ptr<DarpaWirelessChannel> channel = m_channelFactory.Create<DarpaWirelessChannel> ();

  channel->DarpaWirelessChannelInit(c.GetN());

  return Install (c, channel, max_band, channel_step, init_tx_gain, slots_count, cb_slots_count, log);
}

NetDeviceContainer
MFTDMAHelper::Install (const NodeContainer &c, Ptr<DarpaWirelessChannel> channel, uint32_t max_band, uint32_t channel_step, uint32_t init_tx_gain, uint32_t slots_count, uint8_t cb_slots_count, Ptr<OutputStreamWrapper> log) const
{
  NetDeviceContainer devs;
  uint16_t index = 1;

  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); i++)
    {
      devs.Add (InstallPriv (*i, channel, max_band, channel_step, init_tx_gain, slots_count, cb_slots_count, index, log));
      index++;
    }

  return devs;
}

Ptr<NetDevice>
MFTDMAHelper::InstallPriv (Ptr<Node> node, Ptr<DarpaWirelessChannel> channel, uint32_t max_band, uint32_t channel_step, uint32_t init_tx_gain, uint32_t slots_count, uint8_t cb_slots_count, uint16_t index, Ptr<OutputStreamWrapper> log) const
{
  Ptr<MFTDMAMacNetDevice> device = m_deviceFactory.Create<MFTDMAMacNetDevice> ();
  device->SetAttribute ("PointToPointMode", BooleanValue (m_pointToPointMode));

  //device->SetAddress (Mac48Address::Allocate ());
  SetAddressManual(device, index);

  node->AddDevice (device);
  device->SetChannel (channel);

  device->MFTDMAMacNetDeviceInit(max_band, channel_step, init_tx_gain, slots_count, cb_slots_count, log);

	Ptr<Queue<Packet> > queue = m_queueFactory.Create<Queue<Packet> > ();
  device->SetQueue (queue);
  NS_ASSERT_MSG (!m_pointToPointMode || (channel->GetNDevices () <= 2), "Device set to PointToPoint and more than 2 devices on the channel.");
  return device;
}

void
MFTDMAHelper::SetAddressManual(Ptr<MFTDMAMacNetDevice> device, uint16_t index) const
{
	const uint8_t manual_mac_address_array[6] = {0xea, 0xbe, 0x14, 0xa2, (uint8_t)(index >> 8), (uint8_t)(index & 0xFF)};
	//const uint8_t manual_mac_address_array[6] = {0xea, 0xbe, 0x14, 0xa2, 0xb8, (uint8_t)(index & 0xFF)};
	Mac48Address manual_mac_address;
	manual_mac_address.CopyFrom (manual_mac_address_array);
	device->SetAddress (manual_mac_address);
}

}
