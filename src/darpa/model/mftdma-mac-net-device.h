/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef MFTDMA_MAC_NET_DEVICE_H
#define MFTDMA_MAC_NET_DEVICE_H

#include <stdint.h>
#include <string>

#include "ns3/traced-callback.h"
#include "ns3/net-device.h"
#include "ns3/data-rate.h"
#include "ns3/event-id.h"
#include "ns3/mac48-address.h"
#include "slot_manage_protocol.h"
#include "ns3/output-stream-wrapper.h"

namespace ns3 {

template <typename Item> class Queue;
class DarpaWirelessChannel;
class MFTDMAMac;
class MFTDMAController;
class Node;
class ErrorModel;
class NetDeviceQueueInterface;
class RandomSlotSelection;

class MFTDMAMacNetDevice : public NetDevice
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  MFTDMAMacNetDevice ();

  void MFTDMAMacNetDeviceInit(uint32_t max_band, uint32_t channel_step, uint32_t init_tx_gain, uint32_t slots_count, uint8_t cb_slots_count, Ptr<OutputStreamWrapper> log);

  /**
   * Receive a packet from a connected SimpleChannel.  The
   * SimpleNetDevice receives packets from its connected channel
   * and then forwards them by calling its rx callback method
   *
   * \param packet Packet received on the channel
   * \param protocol protocol number
   * \param to address packet should be sent to
   * \param from address packet was sent from
   */
  void Receive (Ptr<Packet> packet, uint16_t protocol, Mac48Address to, Mac48Address from);

  /**
   * Attach a channel to this net device.  This will be the
   * channel the net device sends on
   *
   * \param channel channel to assign to this net device
   *
   */
  void SetChannel (Ptr<DarpaWirelessChannel> channel);

  /**
   * Attach a queue to the SimpleNetDevice.
   *
   * \param queue Ptr to the new queue.
   */
  void SetQueue (Ptr<Queue<Packet> > queue);

  /**
   * Get a copy of the attached Queue.
   *
   * \returns Ptr to the queue.
   */
  Ptr<Queue<Packet> > GetQueue (void) const;

  Ptr<MFTDMAMac> GetMac (void) const;
  void SetMac (Ptr<MFTDMAMac> mac);

  Ptr<RandomSlotSelection> GetSlotSelection (void) const;
  void SetSlotSelection (Ptr<RandomSlotSelection> mac);

  void SetMaxBand (const uint32_t max_band);
  void SetChannelStep (const uint32_t channel_step);
  void SetInitTxGain (const uint32_t init_tx_gain);
  void SetSlotsCount (const uint32_t slots_count);

  uint64_t MacAIMessageIndexUpdate(uint64_t	transaction_index = UINT64_MAX);
  void SendMacAIMessage (Internal &message);

  /**
   * Attach a receive ErrorModel to the SimpleNetDevice.
   *
   * The SimpleNetDevice may optionally include an ErrorModel in
   * the packet receive chain.
   *
   * \see ErrorModel
   * \param em Ptr to the ErrorModel.
   */
  void SetReceiveErrorModel (Ptr<ErrorModel> em);

  // inherited from NetDevice base class.
  virtual void SetIfIndex (const uint32_t index);
  virtual uint32_t GetIfIndex (void) const;
  virtual Ptr<Channel> GetChannel (void) const;
  virtual void SetAddress (Address address);
  virtual Address GetAddress (void) const;
  virtual bool SetMtu (const uint16_t mtu);
  virtual uint16_t GetMtu (void) const;
  virtual bool IsLinkUp (void) const;
  virtual void AddLinkChangeCallback (Callback<void> callback);
  virtual bool IsBroadcast (void) const;
  virtual Address GetBroadcast (void) const;
  virtual bool IsMulticast (void) const;
  virtual Address GetMulticast (Ipv4Address multicastGroup) const;
  virtual bool IsPointToPoint (void) const;
  virtual bool IsBridge (void) const;
  virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
  virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);
  virtual Ptr<Node> GetNode (void) const;
  virtual void SetNode (Ptr<Node> node);
  virtual bool NeedsArp (void) const;
  virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);

  virtual Address GetMulticast (Ipv6Address addr) const;

  virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb);
  virtual bool SupportsSendFrom (void) const;

protected:
  virtual void DoDispose (void);
  virtual void DoInitialize (void);
  virtual void NotifyNewAggregate (void);

private:
  Ptr<DarpaWirelessChannel> m_channel; //!< the channel the device is connected to
  Ptr<MFTDMAMac> m_mac; //!< the mac of the device
  Ptr<MFTDMAController> m_controller; //!< the mac controller of the device
  Ptr<RandomSlotSelection> m_slot_selection; //!< the slot_selection part
  NetDevice::ReceiveCallback m_rxCallback; //!< Receive callback
  NetDevice::PromiscReceiveCallback m_promiscCallback; //!< Promiscuous receive callback
  Ptr<Node> m_node; //!< Node this netDevice is associated to
  Ptr<NetDeviceQueueInterface> m_queueInterface;   //!< NetDevice queue interface
  uint16_t m_mtu;   //!< MTU
  uint32_t m_ifIndex; //!< Interface index
  Mac48Address m_address; //!< MAC address
  Ptr<ErrorModel> m_receiveErrorModel; //!< Receive error model.

  uint32_t max_band;
	uint32_t channel_step;
	uint32_t init_tx_gain;
	uint32_t slots_count;
	uint64_t own_mac_address;
	uint64_t transaction_index;

  /**
   * The trace source fired when the phy layer drops a packet it has received
   * due to the error model being active.  Although SimpleNetDevice doesn't
   * really have a Phy model, we choose this trace source name for alignment
   * with other trace sources.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet> > m_phyRxDropTrace;

  /**
   * The TransmitComplete method is used internally to finish the process
   * of sending a packet out on the channel.
   */
  void TransmitComplete (void);

  bool m_linkUp; //!< Flag indicating whether or not the link is up

  /**
   * Flag indicating whether or not the NetDevice is a Point to Point model.
   * Enabling this will disable Broadcast and Arp.
   */
  bool m_pointToPointMode;

  Ptr<Queue<Packet> > m_queue; //!< The Queue for outgoing packets.
  DataRate m_bps; //!< The device nominal Data rate. Zero means infinite
  EventId TransmitCompleteEvent; //!< the Tx Complete event

  /**
   * List of callbacks to fire if the link changes state (up or down).
   */
  TracedCallback<> m_linkChangeCallbacks;
};

}

#endif /* MFTDMA_MAC_NET_DEVICE_H */

