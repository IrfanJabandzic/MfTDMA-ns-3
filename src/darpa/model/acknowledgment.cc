/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "acknowledgment.h"
#include "mftdma-mac.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include <iostream>
//#include "helpers.h"
#include "tdma_slot.h"

using namespace std;

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("Acknowledgment");

NS_OBJECT_ENSURE_REGISTERED (Acknowledgment);

TypeId
Acknowledgment::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Acknowledgment")
    .SetParent<Object> ()
    .SetGroupName("Darpa")
    .AddConstructor<Acknowledgment> ()
  ;
  return tid;
}

Acknowledgment::Acknowledgment() :
	m_mac(0),
	ack_max_timeout(0),
	ack_control_max_timeout(0),
	active_bw_index(0) //	 active_bw_index = communicator::BW_INDEX::BW_INDEX_UNKNOWN;
{
	NS_LOG_FUNCTION (this);
}

Ptr<MFTDMAMac>
Acknowledgment::GetMac (void) const
{
  NS_LOG_FUNCTION (this);
  return m_mac;
}

void
Acknowledgment::SetMac (Ptr<MFTDMAMac> mac)
{
  NS_LOG_FUNCTION (this << mac);
  m_mac = mac;
}

/**
* Getters
*/
std::list<Acknowledgment::ack_buffer_data>* Acknowledgment::get_ack_buffer_data(mac_address_t destination_mac_address){
    std::list<Acknowledgment::ack_buffer_data>* ack_buffer_list_p = &ack_buffer_map[destination_mac_address];

    return ack_buffer_list_p;
}

bool Acknowledgment::get_ack_needed_info(mac_index_t mac_index, fragment_index_t fragment_index, Acknowledgment::ack_needed_info &ack_needed_info){
	//std::lock_guard<std::mutex> lock_ack_needed{ack_needed_mutex};

	if(ack_needed_map.find(std::make_pair(mac_index, fragment_index)) == ack_needed_map.end())
		return false;
	else
	{
		ack_needed_info = ack_needed_map[std::make_pair(mac_index, fragment_index)];
		return true;
	}
}

std::list<Acknowledgment::ack_statistics>* Acknowledgment::get_ack_stats_list(){
    std::list<Acknowledgment::ack_statistics>* ack_stats_list_p = &ack_stats_list;
    return ack_stats_list_p;
}

/**
* Statistics Setters
*/
void Acknowledgment::increment_number_of_received_acks(mac_address_t addr, packet_counter_t val, timestamp_t current_timestamp){
		return;
		std::lock_guard<std::mutex> aggregated_ack_stats_needed{aggregated_ack_stats_mutex};

    aggregated_ack_stats_map[addr].number_of_received_acks += val;
    aggregated_ack_stats_map[addr].last_updated_timestamp = current_timestamp;
}

void Acknowledgment::increment_number_of_sent_packets(mac_address_t addr, packet_counter_t val, timestamp_t current_timestamp) {
		return;
		std::lock_guard<std::mutex> aggregated_ack_stats_needed{aggregated_ack_stats_mutex};

    aggregated_ack_stats_map[addr].number_of_sent_packets += val;
    aggregated_ack_stats_map[addr].last_updated_timestamp = current_timestamp;
}

void Acknowledgment::increment_ack_timeout_counter(mac_address_t addr, packet_counter_t val, timestamp_t current_timestamp) {
		return;
		aggregated_ack_stats_map[addr].ack_timeout_counter += val;
    aggregated_ack_stats_map[addr].last_updated_timestamp = current_timestamp;
}

void Acknowledgment::clear_ack_stats_list(){
    ack_stats_list.clear();
}

/**
* Moving sample functions
*/
void Acknowledgment::add_ack_moving_sample(mac_address_t addr,
                                           packet_counter_t number_of_sent_packets,
                                           packet_counter_t number_of_received_acks,
                                           packet_counter_t ack_timeout_counters,
                                           timestamp_t current_timestamp){
		return;
		//Pushing to the ack_moving_sample class
    aggregated_ack_stats_map[addr].acks_moving_samples.push(number_of_sent_packets,
                                                 number_of_received_acks,
                                                 ack_timeout_counters,
                                                 current_timestamp);

    //Re-set counters
    aggregated_ack_stats_map[addr].number_of_sent_packets = 0;
    aggregated_ack_stats_map[addr].number_of_received_acks = 0;
    aggregated_ack_stats_map[addr].ack_timeout_counter = 0;
}

/**
* This function adds the ACKs stats into the ACK moving sample buffer
 * @param current_timestamp
 * @return void
*/
void Acknowledgment::update_ack_moving_sample(timestamp_t current_timestamp){
	return;
		//Iterating over ACK statistics
	std::lock_guard<std::mutex> aggregated_ack_stats_needed{aggregated_ack_stats_mutex};

    map<mac_address_t, Acknowledgment::ack_aggregated_statistics>::iterator it;
    for (it=aggregated_ack_stats_map.begin(); it!=aggregated_ack_stats_map.end(); ++it) {

        //Calculating and adding percentage of successful ACK transmissions to the MovingAverage class
        if (it->second.number_of_sent_packets != 0 || it->second.number_of_received_acks != 0 ||
            it->second.ack_timeout_counter != 0) {
            add_ack_moving_sample(it->first,
                                  it->second.number_of_sent_packets,
                                  it->second.number_of_received_acks,
                                  it->second.ack_timeout_counter,
                                  current_timestamp);
        }

        //Printing ACK aggregated statistics
        print_ack_aggregated_statistics();
    }
}


/**
* This function extracts if the flag that indicate if ACK is needed or not
 * @param string data
 * @return boolean ack_needed
*/
bool Acknowledgment::get_ack_needed(std::string* data) {
    const struct mac_header* mac_header = (struct mac_header*) data->c_str();
    return mac_header->need_ack;
}

/**
* This function removes the ACK entry on ack_needed_map
 * @param mac_index
 * @param fragment_index
 * @return boolean
*/
bool Acknowledgment::remove_ack_needed_from_map(mac_index_t mac_index,
                                                fragment_index_t fragment_index) {
		//std::lock_guard<std::mutex> lock_ack_needed{ack_needed_mutex};

    uint64_t size_before = ack_needed_map.size();
    ack_needed_map.erase(std::make_pair(mac_index, fragment_index));
    uint64_t size_after = ack_needed_map.size();
    if (size_after >= size_before){
        NS_LOG_ERROR("Error removing mac_index: " << mac_index << ", fragment_index: " << (uint16_t)fragment_index << " from acks_map when receiving ACK! Entry removed by time out?"); //And do not update stats
        return false;
    }
    NS_LOG_INFO("mac_index: " << mac_index << ", fragment_index: " << (uint16_t)fragment_index << "  removed successfully from acks_map when receiving ACK!");
    return true;
}

/**
* This function adds the ACK entry on ack_needed_map
 * @param mac_index
 * @param fragment_index
 * @param destination_mac_address
 * @param MCS
 * @param GAIN
 * @param CHANNEL
 * @param current_timestamp
 * @return boolean
*/
bool Acknowledgment::add_ack_needed_in_map(mac_index_t mac_index,
                                           fragment_index_t fragment_index,
                                           mac_address_t destination_mac_address,
                                           mcs_t mcs,
                                           gain_t tx_gain,
																					 udp_port_t port,
																					 uint32_t payload_size,
                                           channel_t tx_channel,
																					 slot_number_t slot_index,
																					 uint8_t slot_type,
																					 uint8_t priority,
                                           timestamp_t current_timestamp,
																					 uint8_t retry_counter_max,
																					 Ptr<Packet> packet){

	//Need to check we don't save erroneous ACKs with sender address ==0.
	if(destination_mac_address==0){
		NS_LOG_ERROR("ERROR: Tried to add ACK with destination address: " << destination_mac_address << " in acks_needed_map!");
		return false;
	}

	//std::lock_guard<std::mutex> lock_ack_needed{ack_needed_mutex};

	if(ack_needed_map.find(std::make_pair(mac_index, fragment_index)) != ack_needed_map.end())
	{
        NS_LOG_LOGIC("mac_index: " << mac_index << " already in acks_needed_map!"); //And do not update stats
		return false;
	}

    ack_needed_map[std::make_pair(mac_index, fragment_index)].destination_mac_address = destination_mac_address;
    ack_needed_map[std::make_pair(mac_index, fragment_index)].mcs = mcs;
    ack_needed_map[std::make_pair(mac_index, fragment_index)].tx_gain = tx_gain;
    ack_needed_map[std::make_pair(mac_index, fragment_index)].port = port;
    ack_needed_map[std::make_pair(mac_index, fragment_index)].payload_size = payload_size;
    ack_needed_map[std::make_pair(mac_index, fragment_index)].tx_channel = tx_channel;
#ifdef TDMA_USE_ACK
    ack_needed_map[std::make_pair(mac_index, fragment_index)].slot_index = slot_index;
    ack_needed_map[std::make_pair(mac_index, fragment_index)].slot_type = slot_type;
    ack_needed_map[std::make_pair(mac_index, fragment_index)].priority = priority;
    ack_needed_map[std::make_pair(mac_index, fragment_index)].send_ack_stats = true;
#endif
    ack_needed_map[std::make_pair(mac_index, fragment_index)].timestamp = current_timestamp;
#ifdef RETRANSMISSIONS
    ack_needed_map[std::make_pair(mac_index, fragment_index)].packet = packet;
    ack_needed_map[std::make_pair(mac_index, fragment_index)].max_retry_counter = retry_counter_max;
    ack_needed_map[std::make_pair(mac_index, fragment_index)].retry_counter = retry_counter_max;
//    std::cout << ack_needed_map[std::make_pair(mac_index, fragment_index)].packet << std::endl;
#endif

    NS_LOG_INFO("mac_index: " << mac_index << ", fragment_index: " << (int)fragment_index << ", dst: " << destination_mac_address << ", MCS: " << (uint16_t)mcs << ", GAIN: " << tx_gain << ", and Channel: " << (uint16_t)tx_channel << " added successfully to acks_needed_map!");
    return true;
}

bool Acknowledgment::update_ack_needed_in_map(mac_index_t mac_index,
                                              fragment_index_t fragment_index,
											  bool update_status,
											  timestamp_t current_timestamp,
                                              channel_t tx_channel,
										      slot_number_t slot_index){
	//std::lock_guard<std::mutex> lock_ack_needed{ack_needed_mutex};

	if(ack_needed_map.find(std::make_pair(mac_index, fragment_index)) == ack_needed_map.end())
		return false;

	if(update_status)
	{
		ack_needed_map[std::make_pair(mac_index, fragment_index)].tx_channel = tx_channel;
		ack_needed_map[std::make_pair(mac_index, fragment_index)].slot_index = slot_index;
	}
	ack_needed_map[std::make_pair(mac_index, fragment_index)].send_ack_stats = update_status;
	ack_needed_map[std::make_pair(mac_index, fragment_index)].timestamp = current_timestamp;

	NS_LOG_DEBUG("mac_index: " << mac_index << ", fragment_index: " << (int)fragment_index << ", channel " << (uint16_t)tx_channel << " and slot_index " << (uint16_t)slot_index << ": updated successfully in acks_needed_map!");

	return true;
}

/**
* This function adds the ACK entry on the ack_buffer_map
 * @param destination_address
 * @param mac_packet_index
 * @param fragment_index
 * @param fragment_index_max
 * @param cqi
 * @param rssi
 * @param current_timestamp
 * @return boolean
*/
bool Acknowledgment::add_ack_data_into_buffer(mac_address_t destination_mac_address,
                                              mac_index_t mac_packet_index,
                                              fragment_index_t fragment_index,
                                              fragment_index_max_t fragment_index_max,
                                              cqi_t cqi,
                                              rssi_t rssi,
                                              timestamp_t current_timestamp){
		NS_LOG_DEBUG("To be added on the ACKs buffer " << destination_mac_address << " " << mac_packet_index << " " << (uint16_t)fragment_index << " " << (uint16_t)fragment_index_max << " " << (uint16_t)cqi << " " << (uint16_t)rssi << " " << current_timestamp);

    //Create ACK data structure and add it to the ACK buffer map
    Acknowledgment::ack_buffer_data ack_data;
    ack_data.destination_address = destination_mac_address;
    ack_data.mac_packet_index = mac_packet_index;
    ack_data.fragment_index = fragment_index;
    ack_data.fragment_index_max = fragment_index_max;
    ack_data.cqi = cqi;
    ack_data.rssi = rssi;
    ack_data.timestamp = current_timestamp;

    uint64_t size_before = ack_buffer_map[destination_mac_address].size();
    ack_buffer_map[destination_mac_address].push_back(ack_data);
    uint64_t size_after = ack_buffer_map[destination_mac_address].size();
    if (size_after <= size_before){
    		NS_LOG_ERROR("Error adding ACK data into buffer!");
        return false;
    }
    NS_LOG_DEBUG("ACK data successfully added into ack_buffer_map!");
    return true;
}

/**
* This function checks the ACK buffer data exceeded the limit
 * @param destination_address
 * @return bool
*/
bool Acknowledgment::check_exceeded_ack_buffer_data(mac_address_t destination_mac_address){
    //Check if ACK buffer size exceeded the limit of ACK_BUFFER_SIZE
    if(ack_buffer_map[destination_mac_address].size() >= ACK_BUFFER_SIZE){
    		NS_LOG_DEBUG("ACK Buffer data size exceeded the limit of: " << ACK_BUFFER_SIZE << " with: " << ack_buffer_map[destination_mac_address].size());
        return true;
    } else {
        return false;
    }
};

/**
* This function checks if the ACK entries has expired
 * @param current_timestamp
 * @return list of iterators pointing to the ack_needed_info
*/
#ifndef TDMA_USE_ACK
std::list<communicator::Message>* Acknowledgment::check_ack_timeout(timestamp_t current_timestamp) {
#else
std::list<Acknowledgment::retransmission_info>* Acknowledgment::check_ack_timeout(timestamp_t current_timestamp) {
#endif
   	uint64_t ack_max_timeout_to_use = ack_max_timeout;
#ifndef TDMA_USE_ACK
    //List of packets to be retransmitted
	std::list<communicator::Message>* packets_resent_list_p = new std::list<communicator::Message>();
#else
	std::list<retransmission_info>* retr_info_list_p = new std::list<retransmission_info>();
#endif

	//std::lock_guard<std::mutex> lock_ack_needed{ack_needed_mutex};

	map<std::pair<mac_index_t, fragment_index_t>, ack_needed_info>::iterator it;
    for (it=ack_needed_map.begin(); it!=ack_needed_map.end();) {

    	//Check if a fragment has timedout, lower the counter and retransmit the packet. If the counter is 0, drop the packet
#ifndef TDMA_USE_ACK
    	if((it->second.timestamp + ACK_MAX_TIMEOUT) < current_timestamp){
#else

    	if(it->second.slot_type == TDMA_slot::CONTROL_BROADCAST)
    		ack_max_timeout_to_use = ack_control_max_timeout;
    	else
    		ack_max_timeout_to_use = ack_max_timeout;

        if((it->second.timestamp + ack_max_timeout_to_use) < current_timestamp){
#endif

#ifdef RETRANSMISSIONS
    		//Adjust the counter and check if it reached zero

			if(it->second.send_ack_stats == true)
				it->second.retry_counter--;
#ifndef TDMA_USE_ACK
				NS_LOG_INFO("ACK timeout of dst: " << it->second.destination_mac_address << ", mac_index: " << std::get<0>(it->first) << ", fragment_index: " << std::get<1>(it->first) << ", packet: " << it->second.packet.message->transaction_index() << ", decreasing counter to: " << it->second.retry_counter);
    		std::cout << it->second.packet << std::endl;
#else
    		NS_LOG_DEBUG("ACK timeout of dst: " << it->second.destination_mac_address << ", mac_index: " << std::get<0>(it->first) << ", fragment_index: " << (uint16_t)std::get<1>(it->first) << ", decreasing counter to: " << (uint16_t)it->second.retry_counter);
#endif


    		if(it->second.retry_counter){ //Add them to the return list of packets needing retransmission and refresh their timestamp
#ifndef TDMA_USE_ACK
    			packets_resent_list_p->push_back(it->second.packet);
#else
    			retransmission_info retr_k;
    			retr_k.destination_mac_address = it->second.destination_mac_address;
    			retr_k.mac_index = std::get<0>(it->first);
    			retr_k.fragment_index = std::get<1>(it->first);
    			retr_k.mcs = it->second.mcs;
    			retr_k.tx_gain = it->second.tx_gain;
    			retr_k.port = it->second.port;
    			retr_k.payload_size = it->second.payload_size;
    			retr_k.slot_type = it->second.slot_type;
    			retr_k.priority = it->second.priority;
    			retr_k.packet = it->second.packet;
    			retr_info_list_p->push_back(retr_k);
#endif
    			it->second.timestamp = 0xFFFFFFFFFFFFFFFFUL - ack_max_timeout_to_use - TDMA_ACK_GUARD_TIME;

    			if(it->second.send_ack_stats == true)
    			{
    		    	if(it->second.slot_type != TDMA_slot::CONTROL_BROADCAST)
    		    	{
						//Individual ACK stats
						ack_statistics ack_stats;

						//Pupulating the structure
						ack_stats.destination_address = it->second.destination_mac_address;
						ack_stats.status = FAILED;
						ack_stats.mcs = it->second.mcs;
						ack_stats.tx_gain = it->second.tx_gain;
						ack_stats.port = it->second.port;
						ack_stats.payload_size = it->second.payload_size;
						ack_stats.tx_channel = it->second.tx_channel;
#ifdef TDMA_USE_ACK
						ack_stats.slot_index = it->second.slot_index;
#endif
						ack_stats.max_retry_counter = it->second.max_retry_counter;
						ack_stats.retry_counter = it->second.max_retry_counter - it->second.retry_counter;
						//ack_stats.bytes_transmitted = get_bytes_in_slot(this->active_bw_index, it->second.mcs); //TODO fix this
						//ack_stats.receiver_stats = Do not set this stats because it FAILED;

						ack_stats_list.push_back(ack_stats);
    		    	}
						//Updating Aggregated ACK stats
					increment_ack_timeout_counter(it->second.destination_mac_address, 1, current_timestamp);
    			}

    			++it;
    		}
    		else { //DROP PACKET
#endif

#ifndef TDMA_USE_ACK
    			Logger::log_info<Acknowledgment>("ACK counter zeroed, dropping packet " << it->second.packet.message->transaction_index());
#else
    			NS_LOG_INFO("ACK counter zeroed, dropping packet mac_index: " << std::get<0>(it->first) << ", fragment_index: " << (uint16_t)std::get<1>(it->first) << ".");
#endif

		    	if(it->second.slot_type != TDMA_slot::CONTROL_BROADCAST)
				{
					//Individual ACK stats
					ack_statistics ack_stats;

					//Pupulating the structure
					ack_stats.destination_address = it->second.destination_mac_address;
					ack_stats.status = FAILED;
					ack_stats.mcs = it->second.mcs;
					ack_stats.tx_gain = it->second.tx_gain;
					ack_stats.port = it->second.port;
					ack_stats.payload_size = it->second.payload_size;
					ack_stats.tx_channel = it->second.tx_channel;
	#ifdef TDMA_USE_ACK
					ack_stats.slot_index = it->second.slot_index;
	#endif
					ack_stats.max_retry_counter = it->second.max_retry_counter;
					ack_stats.retry_counter = it->second.max_retry_counter - it->second.retry_counter;
					//ack_stats.bytes_transmitted = get_bytes_in_slot(this->active_bw_index, it->second.mcs); //TODO fix this
					//ack_stats.receiver_stats = Do not set this stats because it FAILED;

					ack_stats_list.push_back(ack_stats);
		    	}
				//Updating Aggregated ACK stats
				increment_ack_timeout_counter(it->second.destination_mac_address, 1, current_timestamp);

				//Removing from ack_needed_map
				uint64_t size_before = ack_needed_map.size();
				it = ack_needed_map.erase(it);
				uint64_t size_after = ack_needed_map.size();

				if (size_after >= size_before){
					NS_LOG_ERROR("Error on timeout removing ACK needed entry in ack_needed_map!");
					return false;
				} else {
					NS_LOG_LOGIC("ACK timeout removed successfully!");
				}
#ifdef RETRANSMISSIONS
    		}
#endif
    	}
//#ifndef RETRANSMISSIONS
    	else ++it;
//#endif
    }
#ifndef TDMA_USE_ACK
    return packets_resent_list_p;
#else
    return retr_info_list_p;
#endif
}

/**
* This function checks if the ACK buffer entries has expired
 * @param current_timestamp
 * @return list of destinations where ACKs should be sent due to timeout
*/
std::list<mac_address_t>* Acknowledgment::get_ack_buffer_destinations_timeout_list(timestamp_t current_timestamp) {

    //List to be returned
    std::list<mac_address_t>* ack_buffer_destinations_timeout_list_p = new std::list<mac_address_t>();

    map<mac_address_t, list<Acknowledgment::ack_buffer_data>>::iterator it;
    for (it=ack_buffer_map.begin(); it!=ack_buffer_map.end(); ++it) {

        for (std::list<Acknowledgment::ack_buffer_data>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
            //Check if ACK timestamp expired by the limit of ACK_BUFFER_TIMEOUT
            if((it2->timestamp + ACK_BUFFER_TIMEOUT) < current_timestamp){
            		NS_LOG_LOGIC("ACKs being sent to " << it2->destination_address << ", due ACK buffer timed out: " << it2->timestamp << " + " << ACK_BUFFER_TIMEOUT << " <= " << current_timestamp);
                ack_buffer_destinations_timeout_list_p->push_back(it2->destination_address);
                break;
            }
        }
    }
    return ack_buffer_destinations_timeout_list_p;
}

/**
* This function checks if the ACK stats has expired
 * @param current_timestamp
 * @return void
*/
void Acknowledgment::check_ack_moving_samples_timeout(timestamp_t current_timestamp) {
	return;
	std::lock_guard<std::mutex> aggregated_ack_stats_needed{aggregated_ack_stats_mutex};

	map<mac_address_t, ack_aggregated_statistics>::iterator it;
    for (it=aggregated_ack_stats_map.begin(); it!=aggregated_ack_stats_map.end(); ++it) {
        aggregated_ack_stats_map[it->first].acks_moving_samples.checkTimeouts(ACK_MOVING_SAMPLES_TIMEOUT, current_timestamp);
    }
}

/**
* Print Statistics
*/
void Acknowledgment::print_ack_aggregated_statistics(){
		NS_LOG_DEBUG("---------------------------- ACKs AGGREGATED STATISTICS ----------------------------");

    map<mac_address_t, ack_aggregated_statistics>::iterator it;
    for (it=aggregated_ack_stats_map.begin(); it!=aggregated_ack_stats_map.end(); ++it) {
    		NS_LOG_DEBUG("Destination address -> " << it->first);
    		NS_LOG_DEBUG("Number of sent packets -> " << it->second.acks_moving_samples.get_aggregated_sent_packets());
    		NS_LOG_DEBUG("Number of received ACKs -> " << it->second.acks_moving_samples.get_aggregated_received_acks());
    		NS_LOG_DEBUG("ACK timeout counter -> " << it->second.acks_moving_samples.get_aggregated_ack_timeout());
    		NS_LOG_DEBUG("**** ACKs Success -> " << it->second.acks_moving_samples.get_percentage_mean() << "% with stdev: " << it->second.acks_moving_samples.get_percentage_stdev());
    		NS_LOG_DEBUG("Last updated timestamp: " << it->second.last_updated_timestamp);
    }
    NS_LOG_DEBUG("--------------------------------- Done ----------------------------------");
}

}

