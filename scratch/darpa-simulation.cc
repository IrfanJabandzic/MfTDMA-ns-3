/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/darpa-module.h"
#include "ns3/applications-module.h"
#include "ns3/mpi-interface.h"
//#include <mpi.h>

using namespace ns3;

static uint64_t ip_tx = 0, ip_rx = 0, ip_rx_one = 0, ip_drop = 0;
static uint64_t mac_send_control_count = 0, mac_send_control_no_ack_count = 0, mac_send_control_no_ack_advertise_count = 0, mac_send_data_count = 0, mac_ack_control_count = 0, mac_ack_data_count = 0;
static uint64_t	mac_drop_count = 0, mac_timeout_drop_count = 0, mac_entered_count = 0, mac_retr_control_count = 0, mac_retr_data_count = 0;
static uint64_t channel_send_count = 0, channel_drop_control_count = 0, channel_drop_data_count = 0, channel_interference_control_count = 0, channel_interference_data_count = 0, channel_added_count = 0;
static uint64_t table_tx_slots_count = 0, table_rx_slots_count = 0, max_slots_count = 0, slot_allocation_tried_count = 0, slot_allocation_count = 0, slot_allocation_durations = 0, slot_allocation_duration_min = 0, slot_allocation_duration_max = 0, slot_removal_count= 0;

#define NUMBER_OF_NODES 2
#define NUMBER_OF_PACKETS 200*800
#define REMOTE_UDP_PORT 9
#define PACKET_PERIOD 2500.0 //in microseconds 2000000.0
#define PACKET_SIZE 12 //1024
#define SIMULATION_DURATION 820.0 //in seconds

//Throughput measurement
#define MEASURE_THROUGHPUT_ALL
#define MEASURE_THROUGHPUT_ONE

#if defined(MEASURE_THROUGHPUT_ALL) || defined(MEASURE_THROUGHPUT_ONE)
#define RX_TIMESTAMP_PERIOD 1000
static uint64_t first_tx_timestamp = 0;
#endif
#ifdef MEASURE_THROUGHPUT_ALL
static uint64_t last_rx_timestamp = 0, last_ip_rx = 0;
#endif
#ifdef MEASURE_THROUGHPUT_ONE
static uint64_t last_rx_timestamp_one = 0, last_ip_rx_one = 0;
#endif

//#define SIMULATE_MOBILITY
//#define SIMULATE_EXPOSED_NODE
//#define SIMULATE_NODE_ENTERS_NETWORK

NS_LOG_COMPONENT_DEFINE ("DarpaSimulation");

static void
Ipv4L3ProtocolTxSink (
  Ptr<const Packet> packet,
  Ptr<Ipv4> ipv4,
  uint32_t interface)
{
	ip_tx++;

#if defined(MEASURE_THROUGHPUT_ALL) || defined(MEASURE_THROUGHPUT_ONE)
	if(first_tx_timestamp == 0)
	{
		first_tx_timestamp = Simulator::Now ().GetMilliSeconds ();
		last_rx_timestamp = first_tx_timestamp;
#ifdef MEASURE_THROUGHPUT_ONE
		last_rx_timestamp_one = first_tx_timestamp;
#endif
	}
#endif
}

static void
Ipv4L3ProtocolRxSink (
	Ptr<OutputStreamWrapper> stream1,
	Ptr<OutputStreamWrapper> stream2,
  Ptr<const Packet> packet,
  Ptr<Ipv4> ipv4,
  uint32_t interface)
{
	ip_rx++;

#ifdef MEASURE_THROUGHPUT_ALL
	uint64_t time = Simulator::Now ().GetMilliSeconds ();
	if(last_rx_timestamp + RX_TIMESTAMP_PERIOD <= time)
	{
		*stream1->GetStream () << time << "\t" << ip_rx*1000/(time - first_tx_timestamp) << std::endl;
		*stream2->GetStream () << time << "\t" << (ip_rx - last_ip_rx)*1000/(time - last_rx_timestamp) << std::endl;
		last_rx_timestamp = time;
		last_ip_rx = ip_rx;
	}
#endif
}

static void
Ipv4L3ProtocolRxSinkOne (
	Ptr<OutputStreamWrapper> stream1,
	Ptr<OutputStreamWrapper> stream2,
  Ptr<const Packet> packet,
  Ptr<Ipv4> ipv4,
  uint32_t interface)
{
	ip_rx_one++;

#ifdef MEASURE_THROUGHPUT_ONE
	uint64_t time = Simulator::Now ().GetMilliSeconds ();
	if(last_rx_timestamp_one + RX_TIMESTAMP_PERIOD <= time)
	{
		*stream1->GetStream () << time << "\t" << ip_rx_one*1000/(time - first_tx_timestamp) << std::endl;
		*stream2->GetStream () << time << "\t" << (ip_rx_one - last_ip_rx_one)*1000/(time - last_rx_timestamp_one) << std::endl;
		last_rx_timestamp_one = time;
		last_ip_rx_one = ip_rx_one;
	}
#endif
}

static void
Ipv4L3ProtocolDropSink (
  Ipv4Header const &header,
  Ptr<const Packet> packet,
  Ipv4L3Protocol::DropReason reason,
  Ptr<Ipv4> ipv4,
  uint32_t interface)
{
	ip_drop++;
  //NS_LOG_UNCOND("Dropped " << drop << "Drop reason " << reason);
}

//Stats
void
SendControlCountTrace (uint32_t oldValue, uint32_t newValue)
{
	mac_send_control_count++;
}

void
SendControlNoAckCountTrace (uint32_t oldValue, uint32_t newValue)
{
	mac_send_control_no_ack_count++;
}

void
SendControlNoAckAdvertiseCountTrace (uint32_t oldValue, uint32_t newValue)
{
	mac_send_control_no_ack_advertise_count++;
}

void
SendDataCountTrace (uint32_t oldValue, uint32_t newValue)
{
	mac_send_data_count++;
}

void
AckControlCountTrace (uint32_t oldValue, uint32_t newValue)
{
	mac_ack_control_count += newValue - oldValue;
}

void
AckDataCountTrace (uint32_t oldValue, uint32_t newValue)
{
	mac_ack_data_count += newValue - oldValue;
}

void
DropCountTrace (uint32_t oldValue, uint32_t newValue)
{
	mac_drop_count++;
}

void
TimeoutDropCountTrace (uint32_t oldValue, uint32_t newValue)
{
	mac_timeout_drop_count += newValue - oldValue;
}

void
EnteredCountTrace (uint32_t oldValue, uint32_t newValue)
{
	mac_entered_count++;
}

void
RetrControlCountTrace (uint32_t oldValue, uint32_t newValue)
{
	mac_retr_control_count++;
}


void
RetrDataCountTrace (uint32_t oldValue, uint32_t newValue)
{
	mac_retr_data_count++;
}


void
ChannelSendTrace (uint32_t oldValue, uint32_t newValue)
{
	channel_send_count++;
}

void
ChannelDropControlTrace (uint32_t oldValue, uint32_t newValue)
{
	channel_drop_control_count += newValue - oldValue;
}

void
ChannelDropDataTrace (uint32_t oldValue, uint32_t newValue)
{
	channel_drop_data_count += newValue - oldValue;
}

void
ChannelInterferenceControlTrace (uint32_t oldValue, uint32_t newValue)
{
	channel_interference_control_count += newValue - oldValue;
}

void
ChannelInterferenceDataTrace (uint32_t oldValue, uint32_t newValue)
{
	channel_interference_data_count += newValue - oldValue;
}

void
ChannelAddedTrace (uint32_t oldValue, uint32_t newValue)
{
	channel_added_count++;
}

void
TableTxSlotsCount (Ptr<OutputStreamWrapper> stream, Ptr<OutputStreamWrapper> slots_stream, uint8_t oldValue, uint8_t newValue)
{
	table_tx_slots_count += newValue - oldValue;

	if(Simulator::Now().GetSeconds() < (SIMULATION_DURATION - 20)  && table_tx_slots_count == table_rx_slots_count)
	{
		max_slots_count = table_tx_slots_count + table_rx_slots_count;
		NS_LOG_UNCOND(Simulator::Now().GetSeconds() << ": Max slots registered=" << max_slots_count);
		NS_LOG_UNCOND("Slot allocation count=" << slot_allocation_count);
		NS_LOG_UNCOND("Slot allocation tried count=" << slot_allocation_tried_count);
		NS_LOG_UNCOND("Slot removal count=" << slot_removal_count);

		*stream->GetStream () << Simulator::Now().GetSeconds() << ": Max slots registered=" << max_slots_count << std::endl;
		*stream->GetStream () << "Slot allocation count=" << slot_allocation_count << std::endl;
		*stream->GetStream () << "Slot allocation tried count=" << slot_allocation_tried_count << std::endl;
		*stream->GetStream () << "Slot removal count=" << slot_removal_count << std::endl;
	}

	if(Simulator::Now().GetSeconds() < (SIMULATION_DURATION - 20))
	{
		//*slots_stream->GetStream () << Simulator::Now().GetSeconds() << ": Tx slots registered=" << table_tx_slots_count << " Rx slots registered=" << table_rx_slots_count << std::endl;
		//*slots_stream->GetStream () << "Slot allocation count=" << slot_allocation_count << " Slot removal count=" << slot_removal_count << std::endl;

		*slots_stream->GetStream () << Simulator::Now().GetSeconds() << ": Tx=" << table_tx_slots_count << " Rx=" << table_rx_slots_count << std::endl << "A=" << slot_allocation_count << " AT=" << slot_allocation_tried_count << " R=" << slot_removal_count << std::endl;
	}

  //NS_LOG_UNCOND(Simulator::Now().GetSeconds() << ": Tx slots " << table_tx_slots_count);
}

void
TableRxSlotsCount (Ptr<OutputStreamWrapper> stream, Ptr<OutputStreamWrapper> slots_stream, uint8_t oldValue, uint8_t newValue)
{
	table_rx_slots_count += newValue - oldValue;

	if(Simulator::Now().GetSeconds() < (SIMULATION_DURATION - 20) && table_tx_slots_count == table_rx_slots_count)
	{
		max_slots_count = table_tx_slots_count + table_rx_slots_count;
		NS_LOG_UNCOND(Simulator::Now().GetSeconds() << ": Max slots registered=" << max_slots_count);
		NS_LOG_UNCOND("Slot allocation count=" << slot_allocation_count);
		NS_LOG_UNCOND("Slot allocation tried count=" << slot_allocation_tried_count);
		NS_LOG_UNCOND("Slot removal count=" << slot_removal_count);

		*stream->GetStream () << Simulator::Now().GetSeconds() << ": Max slots registered=" << max_slots_count << std::endl;
		*stream->GetStream () << "Slot allocation count=" << slot_allocation_count << std::endl;
		*stream->GetStream () << "Slot allocation tried count=" << slot_allocation_tried_count << std::endl;
		*stream->GetStream () << "Slot removal count=" << slot_removal_count << std::endl;
	}

	if(Simulator::Now().GetSeconds() < (SIMULATION_DURATION - 20))
	{
		//*slots_stream->GetStream () << Simulator::Now().GetSeconds() << ": Tx slots registered=" << table_tx_slots_count << " Rx slots registered=" << table_rx_slots_count << std::endl;
		//*slots_stream->GetStream () << "Slot allocation count=" << slot_allocation_count << " Slot removal count=" << slot_removal_count << std::endl;

		*slots_stream->GetStream () << Simulator::Now().GetSeconds() << ": Tx=" << table_tx_slots_count << " Rx=" << table_rx_slots_count << std::endl << "A=" << slot_allocation_count << " AT=" << slot_allocation_tried_count << " R=" << slot_removal_count << std::endl;
	}

  //NS_LOG_UNCOND(Simulator::Now().GetSeconds() << ": Rx slots " << table_rx_slots_count);
}

void
SlotAllocationCount (uint32_t oldValue, uint32_t newValue)
{
	slot_allocation_count++;
}

void
SlotAllocationTriedCount (uint32_t oldValue, uint32_t newValue)
{
	slot_allocation_tried_count++;
}

void
SlotAllocationDurations (uint32_t oldValue, uint32_t newValue)
{
	slot_allocation_durations = newValue;
}

void
SlotAllocationDurationMin (uint32_t oldValue, uint32_t newValue)
{
	slot_allocation_duration_min = newValue;
}

void
SlotAllocationDurationMax (uint32_t oldValue, uint32_t newValue)
{
	slot_allocation_duration_max = newValue;
}

void
SlotRemovalCount (uint32_t oldValue, uint32_t newValue)
{
	slot_removal_count++;
}

typedef std::pair<Ptr<Packet>, Ipv4Header> Ipv4PayloadHeaderPair;

static void PopulateArpCache()
{
  Ptr<Packet> dummy = Create<Packet> ();
  Ptr<ArpCache> arp = CreateObject<ArpCache> ();
  arp->SetAliveTimeout (Seconds (3600 * 24 * 365));

  for (NodeList::Iterator i = NodeList::Begin (); i != NodeList::End (); ++i)
    {
      Ptr<Ipv4L3Protocol> ip = (*i)->GetObject<Ipv4L3Protocol> ();
      NS_ASSERT (ip != 0);
      ObjectVectorValue interfaces;
      ip->GetAttribute ("InterfaceList", interfaces);
      for (ObjectVectorValue::Iterator j = interfaces.Begin (); j != interfaces.End (); j++)
        {
          Ptr<Ipv4Interface> ipIface = j->second->GetObject<Ipv4Interface> ();
          NS_ASSERT (ipIface != 0);
          Ptr<NetDevice> device = ipIface->GetDevice ();
          NS_ASSERT (device != 0);
          Mac48Address addr = Mac48Address::ConvertFrom (device->GetAddress ());
          for (uint32_t k = 0; k < ipIface->GetNAddresses (); k ++)
            {
              Ipv4Address ipAddr = ipIface->GetAddress (k).GetLocal ();
             if (ipAddr == Ipv4Address::GetLoopback ())
                {
                  continue;
                }
              Ipv4Header ipHeader;
              ArpCache::Entry *entry = arp->Add (ipAddr);
              entry->MarkWaitReply (Ipv4PayloadHeaderPair(dummy,ipHeader));
              entry->MarkAlive (addr);
              entry->ClearPendingPacket();
              entry->MarkPermanent ();
            }
        }
    }

  for (NodeList::Iterator i = NodeList::Begin (); i != NodeList::End (); ++i)
    {
      Ptr<Ipv4L3Protocol> ip = (*i)->GetObject<Ipv4L3Protocol> ();
      NS_ASSERT (ip != 0);
      ObjectVectorValue interfaces;
      ip->GetAttribute ("InterfaceList", interfaces);
      for (ObjectVectorValue::Iterator j = interfaces.Begin (); j != interfaces.End (); j ++)
        {
          Ptr<Ipv4Interface> ipIface = j->second->GetObject<Ipv4Interface> ();
          ipIface->SetAttribute ("ArpCache", PointerValue (arp));
        }
    }
}

int
main (int argc, char *argv[])
{
  uint32_t max_band = DEFAULT_MAX_BANDWIDTH;
  uint32_t channel_step = DEFAULT_CHANNEL_STEP;
	uint32_t init_tx_gain = DEFAULT_TX_GAIN;
  uint32_t slots_count = DEFAULT_SLOT_COUNT;
  uint32_t cb_slots_count = DEFAULT_CB_SLOT_COUNT;
  uint16_t simulation_id = 1;
  bool use_mpi = false;
  uint32_t number_of_nodes = NUMBER_OF_NODES;
  uint32_t number_of_neighbors = NUMBER_OF_NODES;
  uint32_t number_of_packets = NUMBER_OF_PACKETS;
  uint32_t packet_period = PACKET_PERIOD;

  CommandLine cmd;
  cmd.AddValue ("max_band", "Maximum bandwith.", max_band);
  cmd.AddValue ("channel_step", "Channel bandwith.", channel_step);
  cmd.AddValue ("tx_gain", "Initial transmit gain.", init_tx_gain);
  cmd.AddValue ("slots_count", "Number of slots in superframe.", slots_count);
  cmd.AddValue ("cb_slots_count", "Number of control/broadcast slots in superframe.", cb_slots_count);
  cmd.AddValue ("simulation_id", "Simulation ID number.", simulation_id);
  cmd.AddValue ("use_mpi", "Enable the use of MPI.", use_mpi);
  cmd.AddValue ("number_of_nodes", "Number of nodes.", number_of_nodes);
  cmd.AddValue ("number_of_neighbors", "Number of neighbors.", number_of_neighbors);
  cmd.AddValue ("number_of_packets", "Number of APP packets per node.", number_of_packets);
  cmd.AddValue ("packet_period", "Time interval between two packets.", packet_period);
  cmd.Parse (argc, argv);

	uint32_t cores = 1;
  uint32_t systemId = 0;

  NodeContainer nodes;

  if(use_mpi)
	{

  /*GlobalValue::Bind ("SimulatorImplementationType",
											 StringValue ("ns3::NullMessageSimulatorImpl"));*/
		GlobalValue::Bind ("SimulatorImplementationType",
											 StringValue ("ns3::DistributedSimulatorImpl"));

		MpiInterface::Enable (&argc, &argv);

		cores = MpiInterface::GetSize ();
	  systemId = MpiInterface::GetSystemId ();
		NS_LOG_UNCOND("MPI cores used " << cores << " System ID of this core " << systemId);

		uint32_t i;
		for(i = 0; i < cores - 1; i++)
		{
			nodes.Create (number_of_nodes/cores, i);
			NS_LOG_UNCOND("Created nodes " << number_of_nodes/cores << " with core ID " << i);
		}
		nodes.Create (number_of_nodes/cores + number_of_nodes%cores, i);
		NS_LOG_UNCOND("Created nodes " << number_of_nodes/cores + number_of_nodes%cores << " with core ID " << i);
	}
  else
	{
  	nodes.Create (number_of_nodes);
	}

  //GlobalValue::Bind ("SimulatorImplementationType", StringValue ("ns3::RealtimeSimulatorImpl"));

  Time::SetResolution (Time::NS);
  //LogComponentEnable ("UdpClient", LOG_LEVEL_INFO);
  //LogComponentEnable ("UdpServer", LOG_LEVEL_INFO);
  //LogComponentEnable ("MFTDMAMac", LOG_LEVEL_ERROR);
  //LogComponentEnable ("TDMA_table", LOG_LEVEL_ERROR);
  //LogComponentEnable ("MFTDMAMacNetDevice", LOG_LEVEL_ERROR);
  //LogComponentEnable ("Acknowledgment", LOG_LEVEL_ERROR);
  //LogComponentEnable ("MFTDMAController", LOG_LEVEL_ALL);
  //LogComponentEnable ("TDMA_slot", LOG_LEVEL_ERROR);
  //LogComponentEnable ("MFTDMASlotGrid", LOG_LEVEL_ERROR);
  //LogComponentEnable ("RandomSlotSelection", LOG_LEVEL_ERROR);
  //LogComponentEnable ("SlotSelectionAIModule", LOG_LEVEL_ERROR);
  //LogComponentEnable ("DarpaWirelessChannel", LOG_LEVEL_ERROR);

  //NodeContainer nodes;
  //nodes.Create (number_of_nodes);

  MFTDMAHelper MFTDMANet;
  MFTDMANet.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  MFTDMANet.SetChannelAttribute ("Delay", StringValue ("0ms"));
  MFTDMANet.SetChannelAttribute ("Neighbors", UintegerValue (number_of_neighbors));
  Config::SetDefault("ns3::neighbour_table::Neighbors", UintegerValue((uint16_t)number_of_neighbors));

  AsciiTraceHelper asciiTraceHelper;
  Ptr<OutputStreamWrapper> mac_log = asciiTraceHelper.CreateFileStream ("mac_log" + std::to_string(simulation_id) + ".txt");
  Ptr<OutputStreamWrapper> slots_log = asciiTraceHelper.CreateFileStream ("slots_log" + std::to_string(simulation_id) + ".txt");

  NetDeviceContainer devices;
  devices = MFTDMANet.Install (nodes, max_band, channel_step, init_tx_gain, slots_count, cb_slots_count, mac_log);

  InternetStackHelper stack;
  stack.Install (nodes);

  Ipv4AddressHelper address;
  //address.SetBase ("192.168.2.0", "255.255.255.0");
  address.SetBase ("192.168.0.0", "255.255.240.0");

  Ipv4InterfaceContainer interfaces = address.Assign (devices);

  PopulateArpCache();

  UdpServerHelper udpServer (REMOTE_UDP_PORT);
	UdpClientHelper udpClient[number_of_nodes];
	ApplicationContainer clientApps[number_of_nodes];

	//RngSeedManager::SetSeed(simulation_id);
	RngSeedManager::SetSeed(5);
	RngSeedManager::SetRun(simulation_id);
	Ptr<UniformRandomVariable> random_client_app_start = CreateObject<UniformRandomVariable> ();
	double client_app_start;

  if(use_mpi)
	{
		ApplicationContainer serverApps[number_of_nodes];

		for(uint16_t node_count = 0; node_count < number_of_nodes; node_count++)
		{
			if(node_count >= (systemId * (number_of_nodes/cores)) && node_count < ((systemId + 1) * (number_of_nodes/cores)))
			{
#if !defined(SIMULATE_MOBILITY) && !defined(SIMULATE_EXPOSED_NODE)
				if(node_count == (number_of_nodes - 1))
					udpClient[node_count].SetAttribute ("RemoteAddress", AddressValue(interfaces.GetAddress (0)));
				else
					udpClient[node_count].SetAttribute ("RemoteAddress", AddressValue(interfaces.GetAddress (node_count + 1)));
#else
#ifdef SIMULATE_MOBILITY
				if(node_count == (number_of_nodes/2 - 1))
					udpClient[node_count].SetAttribute ("RemoteAddress", AddressValue(interfaces.GetAddress (0)));
				else if(node_count == (number_of_nodes - 1))
					udpClient[node_count].SetAttribute ("RemoteAddress", AddressValue(interfaces.GetAddress (number_of_nodes/2)));
				else
					udpClient[node_count].SetAttribute ("RemoteAddress", AddressValue(interfaces.GetAddress (node_count + 1)));
#else
				if((node_count < (number_of_nodes/4)) || (node_count >= (number_of_nodes/2) && node_count < (3*number_of_nodes/4)))
					udpClient[node_count].SetAttribute ("RemoteAddress", AddressValue(interfaces.GetAddress (node_count + number_of_nodes/4)));
				else
					udpClient[node_count].SetAttribute ("RemoteAddress", AddressValue(interfaces.GetAddress (node_count - number_of_nodes/4)));
#endif
#endif

				udpClient[node_count].SetAttribute ("RemotePort", UintegerValue (REMOTE_UDP_PORT));
				udpClient[node_count].SetAttribute ("MaxPackets", UintegerValue (number_of_packets));
				udpClient[node_count].SetAttribute ("Interval", TimeValue (MicroSeconds (packet_period)));
				udpClient[node_count].SetAttribute ("PacketSize", UintegerValue (PACKET_SIZE));

#ifdef SIMULATE_EXPOSED_NODE
				//if(node_count >= (number_of_nodes/4) && node_count < (3*number_of_nodes/4))
				if(node_count < (number_of_nodes/4) || node_count >= (3*number_of_nodes/4))
				{
					clientApps[node_count] = udpClient[node_count].Install (nodes.Get(node_count));
					client_app_start = random_client_app_start->GetValue (2, 5);
					//clientApps[node_count].Start(Seconds (2.0));
#ifndef SIMULATE_NODE_ENTERS_NETWORK
					clientApps[node_count].Start(Seconds (client_app_start));
#else
					//if(node_count == (number_of_nodes/4))
					if(node_count == 0)
						clientApps[node_count].Start(Seconds (200.0));
					else
						clientApps[node_count].Start(Seconds (client_app_start));
#endif
					clientApps[node_count].Stop(Seconds (SIMULATION_DURATION));
				}
#else
				clientApps[node_count] = udpClient[node_count].Install (nodes.Get(node_count));
				client_app_start = random_client_app_start->GetValue (2, 5);
				//clientApps[node_count].Start(Seconds (2.0));
#ifndef SIMULATE_NODE_ENTERS_NETWORK
				clientApps[node_count].Start(Seconds (client_app_start));
#else
				if(node_count == 0)
					clientApps[node_count].Start(Seconds (200.0));
				else
					clientApps[node_count].Start(Seconds (client_app_start));
#endif
				clientApps[node_count].Stop(Seconds (SIMULATION_DURATION));
#endif

				serverApps[node_count] = udpServer.Install (nodes.Get(node_count));
				serverApps[node_count].Start (Seconds (1.0));
				serverApps[node_count].Stop (Seconds (SIMULATION_DURATION));

				NS_LOG_UNCOND("Node ID " << (uint16_t)node_count << " Core ID " << systemId);
			}
		}
	}
  else
	{
		ApplicationContainer serverApps = udpServer.Install (nodes);
		serverApps.Start (Seconds (1.0));
		serverApps.Stop (Seconds (SIMULATION_DURATION));

		for(uint16_t node_count = 0; node_count < number_of_nodes; node_count++)
		{
#if !defined(SIMULATE_MOBILITY) && !defined(SIMULATE_EXPOSED_NODE)
				if(node_count == (number_of_nodes - 1))
					udpClient[node_count].SetAttribute ("RemoteAddress", AddressValue(interfaces.GetAddress (0)));
				else
					udpClient[node_count].SetAttribute ("RemoteAddress", AddressValue(interfaces.GetAddress (node_count + 1)));
#else
#ifdef SIMULATE_MOBILITY
				if(node_count == (number_of_nodes/2 - 1))
					udpClient[node_count].SetAttribute ("RemoteAddress", AddressValue(interfaces.GetAddress (0)));
				else if(node_count == (number_of_nodes - 1))
					udpClient[node_count].SetAttribute ("RemoteAddress", AddressValue(interfaces.GetAddress (number_of_nodes/2)));
				else
					udpClient[node_count].SetAttribute ("RemoteAddress", AddressValue(interfaces.GetAddress (node_count + 1)));
#else
				if((node_count < (number_of_nodes/4)) || (node_count >= (number_of_nodes/2) && node_count < (3*number_of_nodes/4)))
					udpClient[node_count].SetAttribute ("RemoteAddress", AddressValue(interfaces.GetAddress (node_count + number_of_nodes/4)));
				else
					udpClient[node_count].SetAttribute ("RemoteAddress", AddressValue(interfaces.GetAddress (node_count - number_of_nodes/4)));
#endif
#endif
				udpClient[node_count].SetAttribute ("RemotePort", UintegerValue (REMOTE_UDP_PORT));
				udpClient[node_count].SetAttribute ("MaxPackets", UintegerValue (number_of_packets));
				udpClient[node_count].SetAttribute ("Interval", TimeValue (MicroSeconds (packet_period)));
				udpClient[node_count].SetAttribute ("PacketSize", UintegerValue (PACKET_SIZE));

#ifdef SIMULATE_EXPOSED_NODE
				//if(node_count >= (number_of_nodes/4) && node_count < (3*number_of_nodes/4))
				if(node_count < (number_of_nodes/4) || node_count >= (3*number_of_nodes/4))
				{
					clientApps[node_count] = udpClient[node_count].Install (nodes.Get(node_count));
					client_app_start = random_client_app_start->GetValue (2, 5);
					//clientApps[node_count].Start(Seconds (2.0));
#ifndef SIMULATE_NODE_ENTERS_NETWORK
					clientApps[node_count].Start(Seconds (client_app_start));
#else
					//if(node_count == (number_of_nodes/4))
					if(node_count == 0)
						clientApps[node_count].Start(Seconds (200.0));
					else
						clientApps[node_count].Start(Seconds (client_app_start));
#endif
					clientApps[node_count].Stop(Seconds (SIMULATION_DURATION));
				}
#else
				clientApps[node_count] = udpClient[node_count].Install (nodes.Get(node_count));
				client_app_start = random_client_app_start->GetValue (2, 5);
				//clientApps[node_count].Start(Seconds (2.0));
#ifndef SIMULATE_NODE_ENTERS_NETWORK
				clientApps[node_count].Start(Seconds (client_app_start));
#else
				if(node_count == 0)
					clientApps[node_count].Start(Seconds (200.0));
				else
					clientApps[node_count].Start(Seconds (client_app_start));
#endif
				clientApps[node_count].Stop(Seconds (SIMULATION_DURATION));
#endif
		}
	}

  //pointToPoint.EnablePcapAll ("myfirst"); //SimpleNetDeviceHelper not providing this functionality

  //IP stats
  Config::ConnectWithoutContext ("/NodeList/*/$ns3::Ipv4L3Protocol/Drop", MakeCallback (&Ipv4L3ProtocolDropSink));
  Config::ConnectWithoutContext ("/NodeList/*/$ns3::Ipv4L3Protocol/Tx", MakeCallback (&Ipv4L3ProtocolTxSink));

  Ptr<OutputStreamWrapper> stream1 = asciiTraceHelper.CreateFileStream ("rxI" + std::to_string(simulation_id) + ".data");
  Ptr<OutputStreamWrapper> stream2 = asciiTraceHelper.CreateFileStream ("rxII" + std::to_string(simulation_id) + ".data");
  Config::ConnectWithoutContext ("/NodeList/*/$ns3::Ipv4L3Protocol/Rx", MakeBoundCallback (&Ipv4L3ProtocolRxSink, stream1, stream2));
#ifdef MEASURE_THROUGHPUT_ONE
  Ptr<OutputStreamWrapper> stream_one1 = asciiTraceHelper.CreateFileStream ("rxIone" + std::to_string(simulation_id) + ".data");
  Ptr<OutputStreamWrapper> stream_one2 = asciiTraceHelper.CreateFileStream ("rxIIone" + std::to_string(simulation_id) +  ".data");
  Config::ConnectWithoutContext ("/NodeList/1/$ns3::Ipv4L3Protocol/Rx", MakeBoundCallback (&Ipv4L3ProtocolRxSinkOne, stream_one1, stream_one2));
#endif

  //Stats

  /*for(uint8_t node_count = 0; node_count < NUMBER_OF_NODES; node_count++)
  {
		Ptr<NetDevice> net = nodes.Get(node_count)->GetDevice(0);
		Ptr<MFTDMAMacNetDevice> mftdma_net = net->GetObject<MFTDMAMacNetDevice> ();
		Ptr<MFTDMAMac> mftdma_mac = mftdma_net->GetMac();

		mftdma_mac->TraceConnectWithoutContext("SendCount", MakeCallback (&SendCountTrace));
		mftdma_mac->TraceConnectWithoutContext("AckCount", MakeCallback (&AckCountTrace));
		mftdma_mac->TraceConnectWithoutContext("DropCount", MakeCallback (&DropCountTrace));
		mftdma_mac->TraceConnectWithoutContext("EnteredCount", MakeCallback (&EnteredCountTrace));
  }*/

  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/Mac/$ns3::MFTDMAMac/SendControlCount", MakeCallback (&SendControlCountTrace));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/Mac/$ns3::MFTDMAMac/SendControlNoAckCount", MakeCallback (&SendControlNoAckCountTrace));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/Mac/$ns3::MFTDMAMac/SendControlNoAckAdvertiseCount", MakeCallback (&SendControlNoAckAdvertiseCountTrace));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/Mac/$ns3::MFTDMAMac/SendDataCount", MakeCallback (&SendDataCountTrace));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/Mac/$ns3::MFTDMAMac/AckControlCount", MakeCallback (&AckControlCountTrace));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/Mac/$ns3::MFTDMAMac/AckDataCount", MakeCallback (&AckDataCountTrace));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/Mac/$ns3::MFTDMAMac/DropCount", MakeCallback (&DropCountTrace));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/Mac/$ns3::MFTDMAMac/Table/$ns3::TDMA_table/TimeoutDropCount", MakeCallback (&TimeoutDropCountTrace));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/Mac/$ns3::MFTDMAMac/EnteredCount", MakeCallback (&EnteredCountTrace));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/Mac/$ns3::MFTDMAMac/RetrControlCount", MakeCallback (&RetrControlCountTrace));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/Mac/$ns3::MFTDMAMac/RetrDataCount", MakeCallback (&RetrDataCountTrace));

  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/Mac/$ns3::MFTDMAMac/Table/$ns3::TDMA_table/TxSlotsCount", MakeBoundCallback (&TableTxSlotsCount, mac_log, slots_log));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/Mac/$ns3::MFTDMAMac/Table/$ns3::TDMA_table/RxSlotsCount", MakeBoundCallback (&TableRxSlotsCount, mac_log, slots_log));

  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/SlotSelection/$ns3::RandomSlotSelection/SlotAllocationCount", MakeCallback (&SlotAllocationCount));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/SlotSelection/$ns3::RandomSlotSelection/SlotAllocationTriedCount", MakeCallback (&SlotAllocationTriedCount));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/SlotSelection/$ns3::RandomSlotSelection/SlotAllocationDurations", MakeCallback (&SlotAllocationDurations));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/SlotSelection/$ns3::RandomSlotSelection/SlotAllocationDurationMin", MakeCallback (&SlotAllocationDurationMin));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/SlotSelection/$ns3::RandomSlotSelection/SlotAllocationDurationMax", MakeCallback (&SlotAllocationDurationMax));
  Config::ConnectWithoutContext ("/NodeList/*/DeviceList/*/$ns3::MFTDMAMacNetDevice/SlotSelection/$ns3::RandomSlotSelection/SlotRemovalCount", MakeCallback (&SlotRemovalCount));

  //Channel stats
	Ptr<NetDevice> net = nodes.Get(0)->GetDevice(0);
	Ptr<MFTDMAMacNetDevice> mftdma_net = net->GetObject<MFTDMAMacNetDevice> ();
	Ptr<Channel> channel = mftdma_net->GetChannel();
	Ptr<DarpaWirelessChannel> darpa_channel = channel->GetObject<DarpaWirelessChannel> ();

	darpa_channel->TraceConnectWithoutContext("SendPackets", MakeCallback (&ChannelSendTrace));
	darpa_channel->TraceConnectWithoutContext("DropPacketsControl", MakeCallback (&ChannelDropControlTrace));
	darpa_channel->TraceConnectWithoutContext("DropPacketsData", MakeCallback (&ChannelDropDataTrace));
#ifdef START_INTERFERENCE
	darpa_channel->TraceConnectWithoutContext("DropPacketsControlInterference", MakeCallback (&ChannelInterferenceControlTrace));
	darpa_channel->TraceConnectWithoutContext("DropPacketsDataInterference", MakeCallback (&ChannelInterferenceDataTrace));
#endif
	darpa_channel->TraceConnectWithoutContext("AddedPackets", MakeCallback (&ChannelAddedTrace));

  Simulator::Stop (Seconds(SIMULATION_DURATION));

  Simulator::Run ();

  NS_LOG_UNCOND("Number of nodes=" << number_of_nodes);
  NS_LOG_UNCOND("Number of neighbors=" << number_of_neighbors);
  NS_LOG_UNCOND("Number of packets=" << number_of_packets);
  NS_LOG_UNCOND("Packet period=" << packet_period);
  NS_LOG_UNCOND("Number of channels=" << max_band/channel_step);
  NS_LOG_UNCOND("Number of bc slots=" << cb_slots_count);

  NS_LOG_UNCOND("IP Tx=" << ip_tx);
  NS_LOG_UNCOND("IP Rx=" << ip_rx);
  NS_LOG_UNCOND("IP Dropped=" << ip_drop);

  //Stats
  NS_LOG_UNCOND("MAC Send control count=" << mac_send_control_count);
  NS_LOG_UNCOND("MAC Send control no ack count=" << mac_send_control_no_ack_count);
  NS_LOG_UNCOND("MAC Send control no ack advertise count=" << mac_send_control_no_ack_advertise_count);
  NS_LOG_UNCOND("MAC Send data count=" << mac_send_data_count);
  NS_LOG_UNCOND("MAC Ack control count=" << mac_ack_control_count);
  NS_LOG_UNCOND("MAC Ack data count=" << mac_ack_data_count);
  NS_LOG_UNCOND("MAC Drop count=" << mac_drop_count);
  NS_LOG_UNCOND("MAC Timeout Drop count=" << mac_timeout_drop_count);
  NS_LOG_UNCOND("MAC Entered count=" << mac_entered_count);
  NS_LOG_UNCOND("MAC Retr. control packets count=" << mac_retr_control_count);
  NS_LOG_UNCOND("MAC Retr. data packets count=" << mac_retr_data_count);
  NS_LOG_UNCOND("Slot allocation duration average=" << slot_allocation_durations*10/slot_allocation_count);
  NS_LOG_UNCOND("Slot allocation duration min=" << slot_allocation_duration_min);
  NS_LOG_UNCOND("Slot allocation duration max=" << slot_allocation_duration_max);

  //Channel stats
  NS_LOG_UNCOND("Channel Send count=" << channel_send_count);
  NS_LOG_UNCOND("Channel Drop control count=" << channel_drop_control_count);
  NS_LOG_UNCOND("Channel Drop data count=" << channel_drop_data_count);
  NS_LOG_UNCOND("Channel Interference control count=" << channel_interference_control_count);
  NS_LOG_UNCOND("Channel Interference data count=" << channel_interference_data_count);
  NS_LOG_UNCOND("Channel Added count=" << channel_added_count);

	*mac_log->GetStream () << "Number of nodes=" << number_of_nodes << std::endl;
	*mac_log->GetStream () << "Number of neighbors=" << number_of_neighbors << std::endl;
	*mac_log->GetStream () << "Number of packets=" << number_of_packets << std::endl;
	*mac_log->GetStream () << "Packet period=" << packet_period << std::endl;
	*mac_log->GetStream () << "Number of channels=" << max_band/channel_step << std::endl;
	*mac_log->GetStream () << "Number of bc slots=" << cb_slots_count << std::endl;

	*mac_log->GetStream () << "IP Tx=" << ip_tx << std::endl;
	*mac_log->GetStream () << "IP Rx=" << ip_rx << std::endl;
	*mac_log->GetStream () << "IP Dropped=" << ip_drop << std::endl;

	*mac_log->GetStream () << "MAC Send control count=" << mac_send_control_count << std::endl;
	*mac_log->GetStream () << "MAC Send control no ack count=" << mac_send_control_no_ack_count << std::endl;
	*mac_log->GetStream () << "MAC Send control no ack advertise count=" << mac_send_control_no_ack_advertise_count << std::endl;
	*mac_log->GetStream () << "MAC Send data count=" << mac_send_data_count << std::endl;
	*mac_log->GetStream () << "MAC Ack control count=" << mac_ack_control_count << std::endl;
	*mac_log->GetStream () << "MAC Ack data count=" << mac_ack_data_count << std::endl;
	*mac_log->GetStream () << "MAC Drop count=" << mac_drop_count << std::endl;
	*mac_log->GetStream () << "MAC Timeout Drop count=" << mac_timeout_drop_count << std::endl;
	*mac_log->GetStream () << "MAC Entered count=" << mac_entered_count << std::endl;
	*mac_log->GetStream () << "MAC Retr. control packets count=" << mac_retr_control_count << std::endl;
	*mac_log->GetStream () << "MAC Retr. data packets count=" << mac_retr_data_count << std::endl;
	*mac_log->GetStream () << "Slot allocation duration average=" << slot_allocation_durations*10/slot_allocation_count << std::endl;
	*mac_log->GetStream () << "Slot allocation duration min=" << slot_allocation_duration_min << std::endl;
	*mac_log->GetStream () << "Slot allocation duration max=" << slot_allocation_duration_max << std::endl;

	*mac_log->GetStream () << "Channel Send count=" << channel_send_count << std::endl;
	*mac_log->GetStream () << "Channel Drop control count=" << channel_drop_control_count << std::endl;
	*mac_log->GetStream () << "Channel Drop data count=" << channel_drop_data_count << std::endl;
	*mac_log->GetStream () << "Channel Interference control count=" << channel_interference_control_count << std::endl;
	*mac_log->GetStream () << "Channel Interference data count=" << channel_interference_data_count << std::endl;
	*mac_log->GetStream () << "Channel Added count=" << channel_added_count << std::endl;

  Simulator::Destroy ();

  if(use_mpi)
  	MpiInterface::Disable ();

  return 0;
}
