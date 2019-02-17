/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "mftdma-mac-net-device.h"
#include "darpa-wireless-channel.h"
#include "mftdma-mac.h"
#include "mftdma-controller.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/error-model.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/boolean.h"
#include "ns3/string.h"
#include "ns3/tag.h"
#include "ns3/simulator.h"
#include "ns3/net-device-queue-interface.h"
#include "RandomSlotSelection.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MFTDMAMacNetDevice");

/**
 * \brief MFTDMAMacNetDevice tag to store source, destination and protocol of each packet.
 */
class MFTDMATag : public Tag {
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);

  /**
   * Set the source address
   * \param src source address
   */
  void SetSrc (Mac48Address src);
  /**
   * Get the source address
   * \return the source address
   */
  Mac48Address GetSrc (void) const;

  /**
   * Set the destination address
   * \param dst destination address
   */
  void SetDst (Mac48Address dst);
  /**
   * Get the destination address
   * \return the destination address
   */
  Mac48Address GetDst (void) const;

  /**
   * Set the protocol number
   * \param proto protocol number
   */
  void SetProto (uint16_t proto);
  /**
   * Get the protocol number
   * \return the protocol number
   */
  uint16_t GetProto (void) const;

  void Print (std::ostream &os) const;

private:
  Mac48Address m_src; //!< source address
  Mac48Address m_dst; //!< destination address
  uint16_t m_protocolNumber; //!< protocol number
};


NS_OBJECT_ENSURE_REGISTERED (MFTDMATag);

TypeId
MFTDMATag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MFTDMATag")
    .SetParent<Tag> ()
    .SetGroupName("Darpa")
    .AddConstructor<MFTDMATag> ()
  ;
  return tid;
}
TypeId
MFTDMATag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
MFTDMATag::GetSerializedSize (void) const
{
  return 8+8+2;
}
void
MFTDMATag::Serialize (TagBuffer i) const
{
  uint8_t mac[6];
  m_src.CopyTo (mac);
  i.Write (mac, 6);
  m_dst.CopyTo (mac);
  i.Write (mac, 6);
  i.WriteU16 (m_protocolNumber);
}
void
MFTDMATag::Deserialize (TagBuffer i)
{
  uint8_t mac[6];
  i.Read (mac, 6);
  m_src.CopyFrom (mac);
  i.Read (mac, 6);
  m_dst.CopyFrom (mac);
  m_protocolNumber = i.ReadU16 ();
}

void
MFTDMATag::SetSrc (Mac48Address src)
{
  m_src = src;
}

Mac48Address
MFTDMATag::GetSrc (void) const
{
  return m_src;
}

void
MFTDMATag::SetDst (Mac48Address dst)
{
  m_dst = dst;
}

Mac48Address
MFTDMATag::GetDst (void) const
{
  return m_dst;
}

void
MFTDMATag::SetProto (uint16_t proto)
{
  m_protocolNumber = proto;
}

uint16_t
MFTDMATag::GetProto (void) const
{
  return m_protocolNumber;
}

void
MFTDMATag::Print (std::ostream &os) const
{
  os << "src=" << m_src << " dst=" << m_dst << " proto=" << m_protocolNumber;
}



NS_OBJECT_ENSURE_REGISTERED (MFTDMAMacNetDevice);

TypeId
MFTDMAMacNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MFTDMAMacNetDevice")
    .SetParent<NetDevice> ()
    .SetGroupName("Darpa")
    .AddConstructor<MFTDMAMacNetDevice> ()
    .AddAttribute ("Mac", "The MAC layer attached to this device.",
                   PointerValue (),
                   MakePointerAccessor (&MFTDMAMacNetDevice::GetMac,
                                        &MFTDMAMacNetDevice::SetMac),
                   MakePointerChecker<MFTDMAMac> ())
		.AddAttribute ("SlotSelection", "The Slot Selection layer attached to this device.",
									 PointerValue (),
									 MakePointerAccessor (&MFTDMAMacNetDevice::GetSlotSelection,
																				&MFTDMAMacNetDevice::SetSlotSelection),
									 MakePointerChecker<RandomSlotSelection> ())
    .AddAttribute ("ReceiveErrorModel",
                   "The receiver error model used to simulate packet loss",
                   PointerValue (),
                   MakePointerAccessor (&MFTDMAMacNetDevice::m_receiveErrorModel),
                   MakePointerChecker<ErrorModel> ())
    .AddAttribute ("PointToPointMode",
                   "The device is configured in Point to Point mode",
                   BooleanValue (false),
                   MakeBooleanAccessor (&MFTDMAMacNetDevice::m_pointToPointMode),
                   MakeBooleanChecker ())
    .AddAttribute ("TxQueue",
                   "A queue to use as the transmit queue in the device.",
                   StringValue ("ns3::DropTailQueue<Packet>"),
                   MakePointerAccessor (&MFTDMAMacNetDevice::m_queue),
                   MakePointerChecker<Queue<Packet> > ())
    .AddAttribute ("DataRate",
                   "The default data rate for point to point links. Zero means infinite",
                   DataRateValue (DataRate ("0b/s")),
                   MakeDataRateAccessor (&MFTDMAMacNetDevice::m_bps),
                   MakeDataRateChecker ())
    .AddTraceSource ("PhyRxDrop",
                     "Trace source indicating a packet has been dropped "
                     "by the device during reception",
                     MakeTraceSourceAccessor (&MFTDMAMacNetDevice::m_phyRxDropTrace),
                     "ns3::Packet::TracedCallback")
  ;
  return tid;
}

MFTDMAMacNetDevice::MFTDMAMacNetDevice ()
  : m_channel (0),
    m_node (0),
    m_mtu (0xffff),
    m_ifIndex (0),
    m_linkUp (false),
		own_mac_address(0),
		transaction_index(0)
{
  NS_LOG_FUNCTION (this);

  m_mac = CreateObject<MFTDMAMac> ();
  m_controller = CreateObject<MFTDMAController> ();

  m_mac->SetDevice(this);
  m_mac->SetController(m_controller);

  m_controller->SetMac(m_mac);

  m_slot_selection = CreateObject<RandomSlotSelection> ();
  m_slot_selection->SetDevice(this);

  m_controller->SetSlotSelection(m_slot_selection);

  m_mac->SetForwardUpCallback (MakeCallback(&MFTDMAMacNetDevice::Receive, this));
}

void MFTDMAMacNetDevice::MFTDMAMacNetDeviceInit(uint32_t max_band, uint32_t channel_step, uint32_t init_tx_gain, uint32_t slots_count, uint8_t cb_slots_count, Ptr<OutputStreamWrapper> log)
{
	SetMaxBand(max_band);
	SetChannelStep(channel_step);
	SetInitTxGain(init_tx_gain);
	SetSlotsCount(slots_count);

	m_slot_selection->RandomSlotSelectionInit(REMOVAL_THRESHOLD, CONSEC_FAIL_THRESHOLD, slots_count, max_band/channel_step, SUPERFRAME_DURATION/slots_count, TRANSACTION_TIMEOUT_MS, IDLE_TIMEOUT_MS, EXTERNAL_IDLE_TIMEOUT_MS, this->own_mac_address);
	m_mac->MFTDMAMacInit(max_band, channel_step, init_tx_gain, slots_count, cb_slots_count, own_mac_address, log);
}

void
MFTDMAMacNetDevice::DoInitialize (void)
{
  if (m_queueInterface)
    {
      NS_ASSERT_MSG (m_queue != 0, "A Queue object has not been attached to the device");

      // connect the traced callbacks of m_queue to the static methods provided by
      // the NetDeviceQueue class to support flow control and dynamic queue limits.
      // This could not be done in NotifyNewAggregate because at that time we are
      // not guaranteed that a queue has been attached to the netdevice
      m_queueInterface->ConnectQueueTraces (m_queue, 0);
    }

  NetDevice::DoInitialize ();
}

void
MFTDMAMacNetDevice::NotifyNewAggregate (void)
{
  NS_LOG_FUNCTION (this);
  if (m_queueInterface == 0)
    {
      Ptr<NetDeviceQueueInterface> ndqi = this->GetObject<NetDeviceQueueInterface> ();
      //verify that it's a valid netdevice queue interface and that
      //the netdevice queue interface was not set before
      if (ndqi != 0)
        {
          m_queueInterface = ndqi;
        }
    }
  NetDevice::NotifyNewAggregate ();
}

void
MFTDMAMacNetDevice::Receive (Ptr<Packet> packet, uint16_t protocol,
                          Mac48Address to, Mac48Address from)
{
  NS_LOG_FUNCTION (this << packet << protocol << to << from);
  NetDevice::PacketType packetType;

  if (m_receiveErrorModel && m_receiveErrorModel->IsCorrupt (packet) )
    {
      m_phyRxDropTrace (packet);
      return;
    }

  if (to == m_address)
    {
      packetType = NetDevice::PACKET_HOST;
    }
  else if (to.IsBroadcast ())
    {
      packetType = NetDevice::PACKET_BROADCAST;
    }
  else if (to.IsGroup ())
    {
      packetType = NetDevice::PACKET_MULTICAST;
    }
  else
    {
      packetType = NetDevice::PACKET_OTHERHOST;
    }

  if (packetType != NetDevice::PACKET_OTHERHOST)
    {
      m_rxCallback (this, packet, protocol, from);
    }

  if (!m_promiscCallback.IsNull ())
    {
      m_promiscCallback (this, packet, protocol, from, to, packetType);
    }
}

void
MFTDMAMacNetDevice::SetChannel (Ptr<DarpaWirelessChannel> channel)
{
  NS_LOG_FUNCTION (this << channel);
  m_channel = channel;
  m_mac->SetChannel(channel);
  m_controller->SetChannel(channel);
  m_channel->Add (this);
  m_linkUp = true;
  m_linkChangeCallbacks ();
}

Ptr<Queue<Packet> >
MFTDMAMacNetDevice::GetQueue () const
{
  NS_LOG_FUNCTION (this);
  return m_queue;
}

Ptr<MFTDMAMac>
MFTDMAMacNetDevice::GetMac (void) const
{
  NS_LOG_FUNCTION (this);
  return m_mac;
}

void
MFTDMAMacNetDevice::SetMac (Ptr<MFTDMAMac> mac)
{
  NS_LOG_FUNCTION (this << mac);
  m_mac = mac;
}

Ptr<RandomSlotSelection>
MFTDMAMacNetDevice::GetSlotSelection (void) const
{
  NS_LOG_FUNCTION (this);
  return m_slot_selection;
}

void
MFTDMAMacNetDevice::SetSlotSelection (Ptr<RandomSlotSelection> slot_selection)
{
  NS_LOG_FUNCTION (this << slot_selection);
  m_slot_selection = slot_selection;
}

void
MFTDMAMacNetDevice::SetMaxBand (const uint32_t max_band)
{
	this->max_band = max_band;
}

void
MFTDMAMacNetDevice::SetChannelStep (const uint32_t channel_step)
{
	this->channel_step = channel_step;
}

void
MFTDMAMacNetDevice::SetInitTxGain (const uint32_t init_tx_gain)
{
	this->init_tx_gain = init_tx_gain;
}

void
MFTDMAMacNetDevice::SetSlotsCount (const uint32_t slots_count)
{
	this->slots_count = slots_count;
}

uint64_t MFTDMAMacNetDevice::MacAIMessageIndexUpdate (uint64_t transaction_index)
{
	if(transaction_index == UINT64_MAX)
	{
		this->transaction_index++;
		return this->transaction_index - 1;
	}
	else
		return transaction_index;
}

void MFTDMAMacNetDevice::SendMacAIMessage (Internal &message)
{
	if(message.module == MODULE_MAC)
	{
		//m_mac->process_AI_message(message);
	  Simulator::ScheduleNow(&MFTDMAMac::process_AI_message, m_mac, message);
	}
	else if(message.module == MODULE_AI)
	{
		//m_slot_selection->process_message(message);
		Simulator::ScheduleNow(&RandomSlotSelection::process_message, m_slot_selection, message);
	}
}

void
MFTDMAMacNetDevice::SetQueue (Ptr<Queue<Packet> > q)
{
  NS_LOG_FUNCTION (this << q);
  m_queue = q;
}

void
MFTDMAMacNetDevice::SetReceiveErrorModel (Ptr<ErrorModel> em)
{
  NS_LOG_FUNCTION (this << em);
  m_receiveErrorModel = em;
}

void
MFTDMAMacNetDevice::SetIfIndex (const uint32_t index)
{
  NS_LOG_FUNCTION (this << index);
  m_ifIndex = index;
}
uint32_t
MFTDMAMacNetDevice::GetIfIndex (void) const
{
  NS_LOG_FUNCTION (this);
  return m_ifIndex;
}
Ptr<Channel>
MFTDMAMacNetDevice::GetChannel (void) const
{
  NS_LOG_FUNCTION (this);
  return m_channel;
}
void
MFTDMAMacNetDevice::SetAddress (Address address)
{
	NS_LOG_FUNCTION (this << address);
  m_address = Mac48Address::ConvertFrom (address);
  uint8_t temp_mac[6];
  address.CopyTo(temp_mac);
  own_mac_address = get_mac_address_from_array(temp_mac);
  NS_LOG_DEBUG (this << " own_mac_address: " << own_mac_address);
}
Address
MFTDMAMacNetDevice::GetAddress (void) const
{
  //
  // Implicit conversion from Mac48Address to Address
  //
  NS_LOG_FUNCTION (this);
  return m_address;
}
bool
MFTDMAMacNetDevice::SetMtu (const uint16_t mtu)
{
  NS_LOG_FUNCTION (this << mtu);
  m_mtu = mtu;
  return true;
}
uint16_t
MFTDMAMacNetDevice::GetMtu (void) const
{
  NS_LOG_FUNCTION (this);
  return m_mtu;
}
bool
MFTDMAMacNetDevice::IsLinkUp (void) const
{
  NS_LOG_FUNCTION (this);
  return m_linkUp;
}
void
MFTDMAMacNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
 NS_LOG_FUNCTION (this << &callback);
 m_linkChangeCallbacks.ConnectWithoutContext (callback);
}
bool
MFTDMAMacNetDevice::IsBroadcast (void) const
{
  NS_LOG_FUNCTION (this);
  if (m_pointToPointMode)
    {
      return false;
    }
  return true;
}
Address
MFTDMAMacNetDevice::GetBroadcast (void) const
{
  NS_LOG_FUNCTION (this);
  return Mac48Address ("ff:ff:ff:ff:ff:ff");
}
bool
MFTDMAMacNetDevice::IsMulticast (void) const
{
  NS_LOG_FUNCTION (this);
  if (m_pointToPointMode)
    {
      return false;
    }
  return true;
}
Address
MFTDMAMacNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  NS_LOG_FUNCTION (this << multicastGroup);
  return Mac48Address::GetMulticast (multicastGroup);
}

Address MFTDMAMacNetDevice::GetMulticast (Ipv6Address addr) const
{
  NS_LOG_FUNCTION (this << addr);
  return Mac48Address::GetMulticast (addr);
}

bool
MFTDMAMacNetDevice::IsPointToPoint (void) const
{
  NS_LOG_FUNCTION (this);
  if (m_pointToPointMode)
    {
      return true;
    }
  return false;
}

bool
MFTDMAMacNetDevice::IsBridge (void) const
{
  NS_LOG_FUNCTION (this);
  return false;
}

bool
MFTDMAMacNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << packet << dest << protocolNumber);

  return SendFrom (packet, m_address, dest, protocolNumber);
}

bool
MFTDMAMacNetDevice::SendFrom (Ptr<Packet> p, const Address& source, const Address& dest, uint16_t protocolNumber)
{
  NS_LOG_FUNCTION (this << p << source << dest << protocolNumber);
  if (p->GetSize () > GetMtu ())
    {
      return false;
    }

  Ptr<Packet> packet = p->Copy ();

  Mac48Address to = Mac48Address::ConvertFrom (dest);
  Mac48Address from = Mac48Address::ConvertFrom (source);

  MFTDMATag tag;
  tag.SetSrc (from);
  tag.SetDst (to);
  tag.SetProto (protocolNumber);

  p->AddPacketTag (tag);

  if (m_queue->Enqueue (p))
    {
      if (m_queue->GetNPackets () == 1 && !TransmitCompleteEvent.IsRunning ())
        {
          p = m_queue->Dequeue ();
          p->RemovePacketTag (tag);
          Time txTime = Time (0);
          if (m_bps > DataRate (0))
            {
              //txTime = m_bps.CalculateBytesTxTime (packet->GetSize ());
            }
          m_mac->Send (p, protocolNumber, to, from);
          TransmitCompleteEvent = Simulator::Schedule (txTime, &MFTDMAMacNetDevice::TransmitComplete, this);
        }

      return true;
    }


  m_mac->Send (packet, protocolNumber, to, from);
  return true;
}


void
MFTDMAMacNetDevice::TransmitComplete ()
{
  NS_LOG_FUNCTION (this);

  if (m_queue->GetNPackets () == 0)
    {
      return;
    }

  Ptr<Packet> packet = m_queue->Dequeue ();

  MFTDMATag tag;
  packet->RemovePacketTag (tag);

  Mac48Address src = tag.GetSrc ();
  Mac48Address dst = tag.GetDst ();
  uint16_t proto = tag.GetProto ();

  m_mac->Send (packet, proto, dst, src);

  if (m_queue->GetNPackets ())
    {
      Time txTime = Time (0);
      if (m_bps > DataRate (0))
        {
          //txTime = m_bps.CalculateBytesTxTime (packet->GetSize ());
        }
      TransmitCompleteEvent = Simulator::Schedule (txTime, &MFTDMAMacNetDevice::TransmitComplete, this);
    }

  return;
}

Ptr<Node>
MFTDMAMacNetDevice::GetNode (void) const
{
  NS_LOG_FUNCTION (this);
  return m_node;
}
void
MFTDMAMacNetDevice::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}
bool
MFTDMAMacNetDevice::NeedsArp (void) const
{
  NS_LOG_FUNCTION (this);
  if (m_pointToPointMode)
    {
      return false;
    }
  return true;
  //return false;
}
void
MFTDMAMacNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{
  NS_LOG_FUNCTION (this << &cb);
  m_rxCallback = cb;
}

void
MFTDMAMacNetDevice::DoDispose (void)
{
	NS_LOG_FUNCTION (this);
	NS_LOG_ERROR ("Dispose!");
  m_channel = 0;
  m_node = 0;
  m_receiveErrorModel = 0;
  m_queue->Flush ();
  m_queueInterface = 0;
  if (TransmitCompleteEvent.IsRunning ())
    {
      TransmitCompleteEvent.Cancel ();
    }
  NetDevice::DoDispose ();
}


void
MFTDMAMacNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
  NS_LOG_FUNCTION (this << &cb);
  m_promiscCallback = cb;
}

bool
MFTDMAMacNetDevice::SupportsSendFrom (void) const
{
  NS_LOG_FUNCTION (this);
  return true;
}

}

