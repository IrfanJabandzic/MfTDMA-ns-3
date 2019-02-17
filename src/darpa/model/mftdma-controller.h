/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef MFTDMA_CONTROLLER_H
#define MFTDMA_CONTROLLER_H

#include "ns3/object.h"
#include "tdma_table.h"
#include "ns3/output-stream-wrapper.h"

namespace ns3 {

#define START_INTERFERENCE 250
#define STOP_INTERFERENCE 500
//#define ENABLE_MOBILITY 250
//#define DISABLE_MOBILITY 500

//#define ENABLE_IMPROVED_NEIGHBOR_REPORT

class MFTDMAMac;
class DarpaWirelessChannel;
class RandomSlotSelection;

class MFTDMAController : public Object
{
public:
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);
	MFTDMAController();
	//~MFTDMAController();

	void MFTDMAControllerInit(uint64_t process_other_mac_frequency, uint64_t process_other_slot_selection_frequency, Ptr<OutputStreamWrapper> log);

	void MFTDMAControllerStart(uint64_t tx_start_time, uint64_t rx_start_time, uint64_t other_mac_start_time, uint64_t other_slot_selection_start_time);

	Ptr<MFTDMAMac> GetMac (void) const;
	void SetMac (Ptr<MFTDMAMac> node);
	Ptr<DarpaWirelessChannel> GetChannel (void) const;
	void SetChannel (Ptr<DarpaWirelessChannel> channel);
	Ptr<RandomSlotSelection> GetSlotSelection (void) const;
	void SetSlotSelection (Ptr<RandomSlotSelection> slot_selection);

  void process_other_MAC(void);
  void process_other_slot_selection(void);
  void tx_MAC(void);
  void rx_MAC_start(void);
  void rx_MAC_first_event(TDMA_table::rx_timer_container rx_container);
  void rx_MAC_second_event(TDMA_table::rx_timer_container rx_container);

  void start_interference_event(void);
  void stop_interference_event(void);

  void start_mobility_event(void);
  void stop_mobility_event(void);
  void MFTDMAControllerSetNeighbors(uint16_t neighbors);

  void print_table_event(void);
  void print_interference_event(void);
  void get_slots(uint8_t &count, std::vector<std::pair<channel_t, slot_number_t>> &slots, TDMA_slot::tdma_slot_type type);

private:
	Ptr<DarpaWirelessChannel> m_channel;
	Ptr<MFTDMAMac> m_mac;
  Ptr<RandomSlotSelection> m_slot_selection;
  Ptr<OutputStreamWrapper> log;

  uint16_t m_neighbors;
	uint64_t process_other_mac_frequency;
	uint64_t process_other_slot_selection_frequency;

};

}

#endif /* MFTDMA_CONTROLLER_H */
