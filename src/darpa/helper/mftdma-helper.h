/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef MFTDMA_HELPER_H
#define MFTDMA_HELPER_H

#include <string>
#include "ns3/attribute.h"
#include "ns3/object-factory.h"
#include "ns3/net-device-container.h"
#include "ns3/node-container.h"
#include "ns3/darpa-wireless-channel.h"

namespace ns3 {

/**
 * \brief build a set of MFTDMAMacNetDevice objects
 */
class MFTDMAHelper
{
public:
  /**
   * Construct a MFTDMAHelper.
   */
	MFTDMAHelper ();
	virtual ~MFTDMAHelper () {}

  /**
   * Each net device must have a queue to pass packets through.
   * This method allows one to set the type of the queue that is automatically
   * created when the device is created and attached to a node.
   *
   * \param type the type of queue
   * \param n1 the name of the attribute to set on the queue
   * \param v1 the value of the attribute to set on the queue
   * \param n2 the name of the attribute to set on the queue
   * \param v2 the value of the attribute to set on the queue
   * \param n3 the name of the attribute to set on the queue
   * \param v3 the value of the attribute to set on the queue
   * \param n4 the name of the attribute to set on the queue
   * \param v4 the value of the attribute to set on the queue
   *
   * Set the type of queue to create and associated to each
   * MFTDMAMacNetDevice created through MFTDMAHelper::Install.
   */
  void SetQueue (std::string type,
                 std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                 std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                 std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                 std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue ());

  /**
   * Each net device must have a channel to pass packets through.
   * This method allows one to set the type of the channel that is automatically
   * created when the device is created and attached to a node.
   *
   * \param type the type of queue
   * \param n1 the name of the attribute to set on the queue
   * \param v1 the value of the attribute to set on the queue
   * \param n2 the name of the attribute to set on the queue
   * \param v2 the value of the attribute to set on the queue
   * \param n3 the name of the attribute to set on the queue
   * \param v3 the value of the attribute to set on the queue
   * \param n4 the name of the attribute to set on the queue
   * \param v4 the value of the attribute to set on the queue
   *
   * Set the type of channel to create and associated to each
   * MFTDMAMacNetDevice created through MFTDMAHelper::Install.
   */
  void SetChannel (std::string type,
                   std::string n1 = "", const AttributeValue &v1 = EmptyAttributeValue (),
                   std::string n2 = "", const AttributeValue &v2 = EmptyAttributeValue (),
                   std::string n3 = "", const AttributeValue &v3 = EmptyAttributeValue (),
                   std::string n4 = "", const AttributeValue &v4 = EmptyAttributeValue ());


  /**
   * \param n1 the name of the attribute to set
   * \param v1 the value of the attribute to set
   *
   * Set these attributes on each ns3::MFTDMAMacNetDevice created
   * by MFTDMAHelper::Install
   */
  void SetDeviceAttribute (std::string n1, const AttributeValue &v1);

  /**
   * \param n1 the name of the attribute to set
   * \param v1 the value of the attribute to set
   *
   * Set these attributes on each ns3::DarpaWirelessChannel created
   * by MFTDMAHelper::Install
   */
  void SetChannelAttribute (std::string n1, const AttributeValue &v1);

  /**
   * MFTDMAMacNetDevice is Broadcast capable and ARP needing. This function
   * limits the number of MFTDMAMacNetDevice on one channel to two, disables
   * Broadcast and ARP and enables PointToPoint mode.
   *
   * \warning It must be used before installing a NetDevice on a node.
   *
   * \param pointToPointMode True for PointToPoint MFTDMAMacNetDevice
   */
  void SetNetDevicePointToPointMode (bool pointToPointMode);

  /**
   * This method creates an ns3::DarpaWirelessChannel with the attributes configured by
   * MFTDMAHelper::SetChannelAttribute, an ns3::MFTDMAMacNetDevice with the attributes
   * configured by MFTDMAHelper::SetDeviceAttribute and then adds the device
   * to the node and attaches the channel to the device.
   *
   * \param node The node to install the device in
   * \returns A container holding the added net device.
   */
  NetDeviceContainer Install (Ptr<Node> node, uint32_t max_band, uint32_t channel_step, uint32_t init_tx_gain, uint32_t slots_count, uint8_t cb_slots_count, uint8_t index, Ptr<OutputStreamWrapper> log) const;

  /**
   * This method creates an ns3::MFTDMAMacNetDevice with the attributes configured by
   * MFTDMAHelper::SetDeviceAttribute and then adds the device to the node and
   * attaches the provided channel to the device.
   *
   * \param node The node to install the device in
   * \param channel The channel to attach to the device.
   * \returns A container holding the added net device.
   */
  NetDeviceContainer Install (Ptr<Node> node, Ptr<DarpaWirelessChannel> channel, uint32_t max_band, uint32_t channel_step, uint32_t init_tx_gain, uint32_t slots_count, uint8_t cb_slots_count, uint8_t index, Ptr<OutputStreamWrapper> log) const;

  /**
   * This method creates an ns3::DarpaWirelessChannel with the attributes configured by
   * MFTDMAHelper::SetChannelAttribute.  For each Ptr<node> in the provided
   * container: it creates an ns3::MFTDMAMacNetDevice (with the attributes
   * configured by MFTDMAHelper::SetDeviceAttribute); adds the device to the
   * node; and attaches the channel to the device.
   *
   * \param c The NodeContainer holding the nodes to be changed.
   * \returns A container holding the added net devices.
   */
  NetDeviceContainer Install (const NodeContainer &c, uint32_t max_band, uint32_t channel_step, uint32_t init_tx_gain, uint32_t slots_count, uint8_t cb_slots_count, Ptr<OutputStreamWrapper> log) const;

  /**
   * For each Ptr<node> in the provided container, this method creates an
   * ns3::MFTDMAMacNetDevice (with the attributes configured by
   * MFTDMAHelper::SetDeviceAttribute); adds the device to the node; and attaches
   * the provided channel to the device.
   *
   * \param c The NodeContainer holding the nodes to be changed.
   * \param channel The channel to attach to the devices.
   * \returns A container holding the added net devices.
   */
  NetDeviceContainer Install (const NodeContainer &c, Ptr<DarpaWirelessChannel> channel, uint32_t max_band, uint32_t channel_step, uint32_t init_tx_gain, uint32_t slots_count, uint8_t cb_slots_count, Ptr<OutputStreamWrapper> log) const;

private:
  /**
   * This method creates an ns3::MFTDMAMacNetDevice with the attributes configured by
   * MFTDMAHelper::SetDeviceAttribute and then adds the device to the node and
   * attaches the provided channel to the device.
   *
   * \param node The node to install the device in
   * \param channel The channel to attach to the device.
   * \returns The new net device.
   */
  Ptr<NetDevice> InstallPriv (Ptr<Node> node, Ptr<DarpaWirelessChannel> channel, uint32_t max_band, uint32_t channel_step, uint32_t init_tx_gain, uint32_t slots_count, uint8_t cb_slots_count, uint16_t index, Ptr<OutputStreamWrapper> log) const;

  void SetAddressManual(Ptr<MFTDMAMacNetDevice> device, uint16_t index) const;

  ObjectFactory m_queueFactory; //!< Queue factory
  ObjectFactory m_deviceFactory; //!< NetDevice factory
  ObjectFactory m_channelFactory; //!< Channel factory
  bool m_pointToPointMode; //!< Install PointToPoint MFTDMAMacNetDevice or Broadcast ones

};

}

#endif /* MFTDMA_HELPER_H */

