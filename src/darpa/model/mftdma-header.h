/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef MFTDMA_HEADER_H
#define MFTDMA_HEADER_H

#include "ns3/header.h"
#include "mac_types.h"

namespace ns3 {

class MFTDMAHeader : public Header
{
public:
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);
	MFTDMAHeader();
	//~MFTDMAHeader();

  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

	void MFTDMASetHeader(mac_header header);
	mac_header MFTDMAGetHeader(void);

private:
	mac_header m_header;
};

}

#endif /* MFTDMA_HEADER_H */

