/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "mftdma-controller.h"
#include "mftdma-mac.h"
#include "darpa-wireless-channel.h"
#include "ns3/log.h"
#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "RandomSlotSelection.h"
#include "ns3/config.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MFTDMAController");

NS_OBJECT_ENSURE_REGISTERED (MFTDMAController);

TypeId
MFTDMAController::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MFTDMAController")
    .SetParent<Object> ()
    .SetGroupName("Darpa")
    .AddConstructor<MFTDMAController> ()
  ;
  return tid;
}

MFTDMAController::MFTDMAController() :
	m_channel (0),
	m_mac(0),
	m_slot_selection(0),
	process_other_mac_frequency(0),
	process_other_slot_selection_frequency(0)
{
	NS_LOG_FUNCTION (this);
}

void MFTDMAController::MFTDMAControllerInit(uint64_t process_other_mac_frequency, uint64_t process_other_slot_selection_frequency, Ptr<OutputStreamWrapper> log)
{
	NS_LOG_FUNCTION (this);
	this->process_other_mac_frequency = process_other_mac_frequency * 1000000;
	this->process_other_slot_selection_frequency = process_other_slot_selection_frequency * 1000000;
	this->log = log;
}

void MFTDMAController::MFTDMAControllerStart(uint64_t tx_start_time, uint64_t rx_start_time, uint64_t other_mac_start_time, uint64_t other_slot_selection_start_time)
{
	NS_LOG_FUNCTION (this);
	NS_LOG_DEBUG("Routine for transmitting started.");
  Simulator::Schedule (MilliSeconds (tx_start_time), &MFTDMAController::tx_MAC, this);
  NS_LOG_DEBUG("Routine for changing receive channel started.");
  Simulator::Schedule (MilliSeconds (rx_start_time), &MFTDMAController::rx_MAC_start, this);
  NS_LOG_DEBUG("Routine for running mac timeout checks started.");
  Simulator::Schedule (MilliSeconds (other_mac_start_time), &MFTDMAController::process_other_MAC, this);
  NS_LOG_DEBUG("Routine for running slot_selection timeout checks started.");
  Simulator::Schedule (MilliSeconds (other_slot_selection_start_time), &MFTDMAController::process_other_slot_selection, this);
#ifdef START_INTERFERENCE
  NS_LOG_DEBUG("Routine for running intererence started.");
  Simulator::Schedule (MilliSeconds (START_INTERFERENCE * 1000), &MFTDMAController::start_interference_event, this);
#endif

#ifdef  ENABLE_MOBILITY
  Simulator::Schedule (MilliSeconds (ENABLE_MOBILITY * 1000), &MFTDMAController::start_mobility_event, this);
#endif
#ifdef  DISABLE_MOBILITY
  Simulator::Schedule (MilliSeconds (DISABLE_MOBILITY * 1000), &MFTDMAController::stop_mobility_event, this);
#endif

#ifdef SIMULATE_NODE_ENTERS_NETWORK
  if(GetNodeID(m_mac->GetAddress()) == 1 || GetNodeID(m_mac->GetAddress()) == (m_channel->GetNeighbors()/4 + 1))
	{
  	NS_LOG_UNCOND ("Initiate clear external for node " << GetNodeID(m_mac->GetAddress()));
  	Simulator::Schedule (MilliSeconds (NODE_ENTERS_NETWORK_TIME * 1000), &MFTDMAController::node_enters_system_event, this);
	}
#endif

}

Ptr<MFTDMAMac>
MFTDMAController::GetMac (void) const
{
  NS_LOG_FUNCTION (this);
  return m_mac;
}

void
MFTDMAController::SetMac (Ptr<MFTDMAMac> mac)
{
  NS_LOG_FUNCTION (this << mac);
  m_mac = mac;
}

Ptr<DarpaWirelessChannel>
MFTDMAController::GetChannel (void) const
{
	NS_LOG_FUNCTION (this);
	return m_channel;
}

void
MFTDMAController::SetChannel (Ptr<DarpaWirelessChannel> channel)
{
	NS_LOG_FUNCTION (this << channel);
	m_channel = channel;
}

Ptr<RandomSlotSelection>
MFTDMAController::GetSlotSelection (void) const
{
	NS_LOG_FUNCTION (this);
	return m_slot_selection;
}

void
MFTDMAController::SetSlotSelection (Ptr<RandomSlotSelection> slot_selection)
{
	NS_LOG_FUNCTION (this << slot_selection);
	m_slot_selection = slot_selection;
}

inline int16_t
MFTDMAController::GetNodeID (Mac48Address node)
{
	uint8_t mac_address[6];
	node.CopyTo (mac_address);

	return mac_address[4] << 8 | mac_address[5];
}


inline void
MFTDMAController::process_other_MAC(void)
{
  //NS_LOG_FUNCTION (this << "start at time: " << Simulator::Now().GetNanoSeconds());

	m_mac->process_other_MAC();

  Simulator::Schedule (NanoSeconds (process_other_mac_frequency), &MFTDMAController::process_other_MAC, this);
}

inline void
MFTDMAController::process_other_slot_selection(void)
{
  //NS_LOG_FUNCTION (this << "start at time: " << Simulator::Now().GetNanoSeconds());

	m_slot_selection->interloop_steps();

  Simulator::Schedule (NanoSeconds (process_other_slot_selection_frequency), &MFTDMAController::process_other_slot_selection, this);
}

inline void
MFTDMAController::tx_MAC(void)
{
  //NS_LOG_FUNCTION (this << "start at time: " << Simulator::Now().GetNanoSeconds());

  uint16_t next_tx_wkup = m_mac->tx_MAC();
	//uint16_t next_tx_wkup = 1;

  Simulator::Schedule (NanoSeconds (next_tx_wkup * 1000000), &MFTDMAController::tx_MAC, this);
}

void
MFTDMAController::rx_MAC_start(void)
{
  NS_LOG_FUNCTION (this << "start at time: " << Simulator::Now().GetNanoSeconds());

	channel_t previous_rx_channel = m_mac->GetRxChannel();
  channel_t rx_channel = previous_rx_channel;
	TDMA_table::rx_timer_container rx_container;

	//TODO make it better and test it
	TDMA_table::table_operation_status status = m_mac->GetRxMessage(rx_container, rx_channel);

	if(status == TDMA_table::NO_SLOTS_AVAILABLE)
	{
		NS_LOG_LOGIC("No need to change Rx channel in the superframe!\n");
		Simulator::Schedule (NanoSeconds (50 * 1000000), &MFTDMAController::rx_MAC_start, this);
		//return;
	}
	else
	{
		if(previous_rx_channel != rx_channel)
		{
			//NS_LOG_LOGIC("Changing Rx channel");
			//NS_LOG_LOGIC("Rx timer in start: " << rx_container.next_rx_wkup);
			//NS_LOG_LOGIC("First change: " << rx_container.rx_first_change);
			if(rx_container.rx_first_change > 0)
			{
				if(rx_container.rx_first_change > 1000)
				{
					//TODO these error checks can be moved to tdma_table functions
					NS_LOG_ERROR("ERROR: rx_first_change " << rx_container.rx_first_change);
					rx_container.rx_first_change = 0;
				}
				rx_container.rx_first_change += RX_TIMER_WAIT;

				if(rx_container.rx_number_of_changes == 2)
				{
					rx_container.rx_second_change += RX_TIMER_WAIT;
					rx_container.rx_second_change -= rx_container.rx_first_change;
				}
				Simulator::Schedule (NanoSeconds (rx_container.rx_first_change * 1000000), &MFTDMAController::rx_MAC_first_event, this, rx_container);
				return;
			}
			Simulator::ScheduleNow (&MFTDMAController::rx_MAC_first_event, this, rx_container);
			return;
		}
		//NS_LOG_LOGIC("Rx timer set for: " << rx_container.next_rx_wkup);
		//NS_LOG_LOGIC("Setting timer for next rx_callback!");
		Simulator::Schedule (NanoSeconds (rx_container.next_rx_wkup * 1000000), &MFTDMAController::rx_MAC_start, this);
	}
}

inline void MFTDMAController::rx_MAC_first_event(TDMA_table::rx_timer_container rx_container)
{
	m_mac->SetRxChannel(rx_container.rx_first_container.ctrl_info.tx_channel);
	NS_LOG_DEBUG ("Rx channel: " << (uint16_t)m_mac->GetRxChannel() << " Time changed: " << Simulator::Now().GetNanoSeconds());
	if(rx_container.rx_number_of_changes == 2)
	{
		//NS_LOG_LOGIC("Second change: " << rx_container.rx_second_change);
		if(rx_container.rx_second_change > 1000)
		{
			NS_LOG_ERROR("ERROR: rx_second_change " << rx_container.rx_second_change);
			rx_container.rx_second_change = 0;
		}
		rx_container.next_rx_wkup -= (rx_container.rx_second_change + rx_container.rx_first_change);
		if(rx_container.rx_second_change > 0)
		{
			Simulator::Schedule (NanoSeconds (rx_container.rx_second_change * 1000000), &MFTDMAController::rx_MAC_second_event, this, rx_container);
			return;
		}
		Simulator::ScheduleNow (&MFTDMAController::rx_MAC_second_event, this, rx_container);
		return;
	}

	rx_container.next_rx_wkup -= rx_container.rx_first_change;
	//NS_LOG_LOGIC("Rx timer set for: " << rx_container.next_rx_wkup);
	//NS_LOG_LOGIC("Setting timer for next rx_callback!");
	Simulator::Schedule (NanoSeconds (rx_container.next_rx_wkup * 1000000), &MFTDMAController::rx_MAC_start, this);

}

inline void MFTDMAController::rx_MAC_second_event(TDMA_table::rx_timer_container rx_container)
{
	m_mac->SetRxChannel(rx_container.rx_second_container.ctrl_info.tx_channel);
	NS_LOG_DEBUG ("Rx channel: " << (uint16_t)m_mac->GetRxChannel() << " Time changed: " << Simulator::Now().GetNanoSeconds());
	//NS_LOG_LOGIC("Rx timer set for: " << rx_container.next_rx_wkup);
	//NS_LOG_LOGIC("Setting timer for next rx_callback!");
	Simulator::Schedule (NanoSeconds (rx_container.next_rx_wkup * 1000000), &MFTDMAController::rx_MAC_start, this);
}

void MFTDMAController::start_interference_event(void)
{
	print_table_event();

	/*
	uint8_t count_tx = 5;
	std::vector<std::pair<channel_t, slot_number_t>> slots_tx;

	get_slots(count_tx, slots_tx, TDMA_slot::UNICAST_SEND);

	for(uint8_t i = 0; i < slots_tx.size(); i++)
			m_channel->AddInterferenceSlot(slots_tx[i].first, slots_tx[i].second);

	m_channel->AddInterferenceSlot(0, 0);
	m_channel->AddInterferenceSlot(3, 6);*/

	/*for(uint8_t i = 2; i < 4; i++)
	{
		for(uint8_t j = 0; j < 20; j++)
			m_channel->AddInterferenceSlot(i, j);
	}*/

	Simulator::Schedule (Seconds (1), &MFTDMAController::print_interference_event, this);

#ifdef STOP_INTERFERENCE
	Simulator::Schedule (MilliSeconds (STOP_INTERFERENCE * 1000), &MFTDMAController::stop_interference_event, this);
#endif
}

void MFTDMAController::stop_interference_event(void)
{
	print_table_event();

	//m_channel->RemoveInterferenceSlots();
}

void MFTDMAController::start_mobility_event(void)
{
  Config::SetDefault("ns3::neighbour_table::Neighbors", UintegerValue(m_neighbors/2));
	m_channel->ChangeMobility(true);
}

void MFTDMAController::stop_mobility_event(void)
{
  Config::SetDefault("ns3::neighbour_table::Neighbors", UintegerValue(m_neighbors));
	m_channel->ChangeMobility(false);
}

void MFTDMAController::node_enters_system_event(void)
{
	NS_LOG_UNCOND ("Start clear external at time " << Simulator::Now().GetSeconds());
	m_slot_selection->clear_external_table();
}

void MFTDMAController::MFTDMAControllerSetNeighbors(uint16_t neighbors)
{
	m_neighbors = neighbors;
}

void MFTDMAController::print_table_event(void)
{
	std::vector<std::pair<std::pair<channel_t, slot_number_t>, std::pair<TDMA_slot::tdma_slot_type, uint16_t>>> slots_more;
	m_mac->table_get_active_slots_more(slots_more);

	for(uint16_t i = 0; i < slots_more.size(); i++)
		NS_LOG_UNCOND("Slot:" << (uint16_t)slots_more[i].first.first << "\t" << (uint16_t)slots_more[i].first.second << "\t"
									<< (uint16_t)slots_more[i].second.first << "\t" << (uint16_t)slots_more[i].second.second);

	for(uint16_t i = 0; i < slots_more.size(); i++)
		*log->GetStream () << "Slot:" << (uint16_t)slots_more[i].first.first << "\t" << (uint16_t)slots_more[i].first.second << "\t"
									<< (uint16_t)slots_more[i].second.first << "\t" << (uint16_t)slots_more[i].second.second << std::endl;
}

void MFTDMAController::print_interference_event(void)
{
	m_channel->PrintInterferenceSlots(log);
}

void MFTDMAController::get_slots(uint8_t &count, std::vector<std::pair<channel_t, slot_number_t>> &slots, TDMA_slot::tdma_slot_type type)
{
	std::vector<std::pair<std::pair<channel_t, slot_number_t>, std::pair<TDMA_slot::tdma_slot_type, uint16_t>>> slots_more;
	m_mac->table_get_active_slots_more(slots_more);

	for(uint16_t i = 0; i < slots_more.size(); i++)
	{
		if(slots_more[i].second.first == type)
		{
			slots.push_back(slots_more[i].first);
		}
	}

	if(slots.size() <= count)
	{
		count = slots.size();
	}
	else
	{
		while(slots.size() > count)
		{
				uint8_t pos = rand() % slots.size();
				slots.erase(slots.begin()+pos);
		}
		//Shuffle the remaining proposals
		//std::random_shuffle(slots.begin(), slots.end());
	}
}

}
