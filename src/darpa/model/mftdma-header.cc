/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "mftdma-header.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MFTDMAHeader");

NS_OBJECT_ENSURE_REGISTERED (MFTDMAHeader);

TypeId
MFTDMAHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MFTDMAHeader")
    .SetParent<Header> ()
    .SetGroupName("Darpa")
    .AddConstructor<MFTDMAHeader> ()
  ;
  return tid;
}

MFTDMAHeader::MFTDMAHeader()
{
	NS_LOG_FUNCTION (this);
}

TypeId
MFTDMAHeader::GetInstanceTypeId (void) const
{
  return GetTypeId();
}

uint32_t
MFTDMAHeader::GetSerializedSize (void) const
{
  return sizeof(mac_header);
}

void
MFTDMAHeader::Serialize (ns3::Buffer::Iterator start) const
{
  ns3::Buffer::Iterator i = start;

  i.WriteU64(m_header.destination_mac_address);
  i.WriteU64(m_header.source_mac_address);
  i.WriteU16(m_header.mac_index);
  i.WriteU16(m_header.num_pad_bytes);
  i.WriteU16(m_header.mac_crc);
  i.WriteU8(m_header.need_ack);
  i.WriteU8(m_header.mes_type);
  i.WriteU8(m_header.fragment_index);
  i.WriteU8(m_header.fragment_index_max);
}

uint32_t
MFTDMAHeader::Deserialize (ns3::Buffer::Iterator start)
{
	m_header.destination_mac_address = start.ReadU64();
	m_header.source_mac_address = start.ReadU64();
	m_header.mac_index = start.ReadU16();
	m_header.num_pad_bytes = start.ReadU16();
	m_header.mac_crc = start.ReadU16();
	m_header.need_ack = start.ReadU8();
	m_header.mes_type = (enum type_of_message)start.ReadU8();
	m_header.fragment_index = start.ReadU8();
	m_header.fragment_index_max = start.ReadU8();

  return sizeof(mac_header);
}

void
MFTDMAHeader::Print (std::ostream &os) const
{
  /*os << "dest_mac_addr: " << m_header.destination_mac_address
     << ", src_mac_addr: " << m_header.source_mac_address
     << ", mac_index: " << m_header.mac_index
     << ", num_pad_bytes: " << m_header.num_pad_bytes
     << ", mac_crc: " << m_header.mac_crc
     << ", need_ack: " << m_header.need_ack
     << ", mes_type: " << m_header.mes_type
     << ", fragment_index: " << (uint16_t)m_header.fragment_index
     << ", fragment_index_max: " << (uint16_t)m_header.fragment_index_max << "\r\n";*/
}

void MFTDMAHeader::MFTDMASetHeader(mac_header header)
{
	NS_LOG_FUNCTION (this);
	m_header = header;
}

mac_header MFTDMAHeader::MFTDMAGetHeader(void)
{
	NS_LOG_FUNCTION (this);
	return m_header;
}

}
