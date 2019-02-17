/*
 * helpers.cpp
 *
 *  Created on: May 12, 2017
 *      Author: spilios
 */

#include "helpers.h"
#include <iostream>
#include <map>
#include <stdio.h>
#include "ns3/simulator.h"

//#define FRAGMENTATION_MAX_TIMEOUT 1000000000ul // (1.0s)
#define FRAGMENTATION_MAX_TIMEOUT 2000000000ul // (2.0s) > RETRY_COUNTER_MAX (6) * ACK_MAX_TIMEOUT (265msec)


using namespace std;

namespace ns3 {

std::vector<uint16_t> mcs_bw_table = {	19,		49,		85,		173,\
										26,		65,		113,	225,\
										32,		81,		137,	277,\
										41,		109,	177,	357,\
										51,		133,	225,	453,\
										63,		165,	277,	549,\
										75,		193,	325,	645,\
										89,		225,	389,	775,\
										101,	261,	437,	871,\
										117,	293,	501,	999,\
										117,	293,	501,	999,\
										129,	333,	549,	1095,\
										149,	373,	621,	1239,\
										169,	421,	717,	1431,\
										193,	485,	807,	1620,\
										217,	533,	903,	1764,\
										225,	573,	967,	1908,\
										225,	573,	967,	1908,\
										241,	621,	999,	2052,\
										269,	669,	1143,	2292,\
										293,	749,	1239,	2481,\
										325,	807,	1335,	2673,\
										349,	871,	1431,	2865,\
										373,	935,	1572,	3182,\
										405,	999,	1692,	3422,\
										437,	1063,	1764,	3542,\
										453,	1143,	1908,	3822,\
										469,	1191,	1980,	3963,\
										549,	1383,	2292,	4587 };

static boost::circular_buffer<std::pair<mac_address_t, mac_index_t>> received_app_packets(RECEIVED_APP_PACKETS_SIZE);
//static boost::circular_buffer<std::pair<mac_address_t, mac_index_t>> received_control_acks(RECEIVED_CONTROL_ACKS_SIZE);
//static boost::circular_buffer<std::pair<mac_address_t, mac_index_t>> control_acks(RECEIVED_CONTROL_ACKS_SIZE);

uint16_t get_bytes_in_slot(BW_INDEX bw_index, mcs_t mcs){

	//std::cout << "BYTESIZE: " << (mcs*4 + (bw_index-1)) << std::endl;

	//Sanity check
	uint16_t return_val = mcs_bw_table[mcs*4 + (bw_index-1)];
	if(return_val <= 4587)
		return return_val;
	else
		std::cout << "MAC:ERROR padded bytes calculated value out of range";

	return 0;
}

mcs_t get_minimum_mcs_to_fit_packet(BW_INDEX bw_index, uint16_t data_size){

	mcs_t temp_mcs = 0;
	uint16_t packet_size = data_size+sizeof(struct mac_header);
	while(get_bytes_in_slot(bw_index,temp_mcs) < packet_size){
	temp_mcs++;
	}
	//Logger::log_debug<MacModule>("Packet was {} and the mcs selected was {} with packet size of {}", packet_size,temp_mcs, get_bytes_in_slot(bw_index,temp_mcs));
	return temp_mcs;
}

timestamp_t clock_get_time_ns()
{
  //timespec ts;
  //clock_gettime(CLOCK_REALTIME, &ts);
  //return (uint64_t)ts.tv_sec * 1000000000LL + (uint64_t)ts.tv_nsec;
	return Simulator::Now().GetNanoSeconds();
}

timestamp_t clock_get_realtime_ns()
{
  timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return (uint64_t)ts.tv_sec * 1000000000LL + (uint64_t)ts.tv_nsec;
}


uint16_t get_random_value(uint16_t max_value)
{
    return rand() % (max_value + 1);
}

bool calculate_probability(uint16_t count)
{
	/*double random_value = rand() * 1.0 / RAND_MAX;
	double probability = 1.0 / count;

	return random_value <= probability ? true : false;*/
	return rand()%count==0;
}

mac_address_t get_mac_address_from_array(uint8_t array[6])
{
	mac_address_t dest_mac = 0;

	for(uint8_t i = 0; i < 6; ++i)
	{
		dest_mac = (dest_mac << 8) | static_cast<uint8_t>(array[i]);
	}

	return dest_mac;
}

void get_array_from_mac_address(mac_address_t mac, uint8_t array[6])
{
	for(uint8_t i = 5; i > 0; i--)
	{
		array[i] = mac & 0xFF;
		mac = mac >> 8;
	}
	array[0] = mac & 0xFF;
}

//CRC16 function
unsigned short crc16(const unsigned char* data_p, unsigned short length){

	unsigned char x;
	unsigned short crc = 0xFFFF;

	while (length--){
		x = crc >> 8 ^ *data_p++;
		x ^= x>>4;
		crc = (crc << 8) ^ ((unsigned short)(x << 12)) ^ ((unsigned short)(x <<5)) ^ ((unsigned short)x);
	}
	return crc;
}

//PRINT PACKET data helper
void print_packet_data(const std::string* data)
{

#ifdef PRINT_PACKET_DATA
	//Print the received application data
	std::cout << "The application data is       : ";
	const unsigned char* print_data_pointer = (unsigned char*) data->c_str();
	for(unsigned int i=0; i< data->size();++i)
	{
		printf("%x, ",print_data_pointer[i]);
		//std::cout << (unsigned int)print_data_pointer[i] << " ";
	}
	std::cout << std::endl;
#endif

}

/**
 * Destination - MCS table and related functions
 */
struct dest_mcs {
	mac_address_t mac_addr_value;
	uint32_t mcs_value;
	uint32_t channel_value;
	uint32_t tx_gain;
};

std::map<mac_address_t, dest_mcs> mac_addr_mcs_map;

void add_dest_mcs_tupple(mac_address_t mac_addr_v, uint32_t mcs_v, uint32_t default_gain){

	dest_mcs tmp_dest_mcs = {mac_addr_v, mcs_v, TX_CHANNEL,default_gain} ;

	std::pair<std::map<mac_address_t, dest_mcs>::iterator,bool> ret;

	ret = mac_addr_mcs_map.insert(std::pair<mac_address_t,dest_mcs>(mac_addr_v, tmp_dest_mcs));

	if (ret.second==false) {
		//Element already exists so go ahead and just update the MCS
		mac_addr_mcs_map[mac_addr_v].mcs_value=mcs_v;
	}

}

void add_dest_txgain_tupple(mac_address_t mac_addr_v, uint32_t tx_gain, uint8_t min_mcs){

	dest_mcs tmp_dest_mcs = {mac_addr_v, min_mcs, TX_CHANNEL, tx_gain} ;

	std::pair<std::map<mac_address_t, dest_mcs>::iterator,bool> ret;

	ret = mac_addr_mcs_map.insert(std::pair<mac_address_t,dest_mcs>(mac_addr_v, tmp_dest_mcs));

	if (ret.second==false) {
		//Element already exists so go ahead and just update the TX gain
		mac_addr_mcs_map[mac_addr_v].tx_gain=tx_gain;
	}

}

void add_dest_channel_tupple(mac_address_t mac_addr_v, uint32_t channel, uint32_t default_gain, uint8_t min_mcs){

	dest_mcs tmp_dest_mcs = {mac_addr_v, min_mcs, channel, default_gain};

	std::pair<std::map<mac_address_t, dest_mcs>::iterator,bool> ret;

	ret = mac_addr_mcs_map.insert(std::pair<mac_address_t,dest_mcs>(mac_addr_v, tmp_dest_mcs));

	if (ret.second==false) {
		//Element already exists so go ahead and just update the RX channel
		mac_addr_mcs_map[mac_addr_v].channel_value = channel;
	}

}

bool remove_dest_mcs_tupple(mac_address_t mac_addr_v){

	if(mac_addr_mcs_map.erase (mac_addr_v) == 1)
		return true;

	return false;
}

uint32_t get_mcs_based_on_destination(mac_address_t mac_addr_v, uint8_t min_mcs){

	std::map<mac_address_t, dest_mcs>::iterator iter;

	iter = mac_addr_mcs_map.find(mac_addr_v);

	if(iter!= mac_addr_mcs_map.end()){
		return iter->second.mcs_value;
	}

	return min_mcs;
}

uint32_t get_channel_based_on_destination(mac_address_t mac_addr_v){

	std::map<mac_address_t, dest_mcs>::iterator iter;

	iter = mac_addr_mcs_map.find(mac_addr_v);

	if(iter!= mac_addr_mcs_map.end()){
		return iter->second.channel_value;
	}

	return TX_CHANNEL;
}

uint32_t get_txgain_based_on_destination(mac_address_t mac_addr_v, uint32_t default_gain){

	std::map<mac_address_t, dest_mcs>::iterator iter;

	iter = mac_addr_mcs_map.find(mac_addr_v);

	if(iter!= mac_addr_mcs_map.end()){
		return iter->second.tx_gain;
	}

	return default_gain;
}

void initialize_dest_mcs_tupple(mac_address_t mac_addr_v, uint32_t default_gain, uint8_t min_mcs){

	dest_mcs tmp_dest_mcs = {mac_addr_v, min_mcs, TX_CHANNEL , default_gain} ;

	std::pair<std::map<mac_address_t, dest_mcs>::iterator,bool> ret;

	ret = mac_addr_mcs_map.insert(std::pair<mac_address_t,dest_mcs>(mac_addr_v, tmp_dest_mcs));

}

void print_dest_mcs_table(){

	//std::cout << mac_addr_mcs_map << std::endl;
	std::cout << "MAC_address 		" << "Channel	" << "MCS " << " TX gain " << "\n";

	for(auto elem : mac_addr_mcs_map)
	{
		std::cout << elem.second.mac_addr_value << "			"  << elem.second.channel_value << "	"<< elem.second.mcs_value << "	 " << elem.second.tx_gain << "\n";
	}

}

uint8_t get_limit_of_active_slots(uint32_t channel_width){

	switch(channel_width){

		case 1400000:
			return 16;
			break;
		case 3000000:
			return 7;
			break;
		case 5000000:
			return 3;
			break;
		default:
			return 3;
			break;
		}

}

//Dummy testing functions
void test_MCS_functions(){

	add_dest_mcs_tupple(281474976710655,8,DEFAULT_TX_GAIN);
	add_dest_mcs_tupple(2000,20,DEFAULT_TX_GAIN);
	add_dest_mcs_tupple(3000,30,DEFAULT_TX_GAIN);
	add_dest_mcs_tupple(4000,40,DEFAULT_TX_GAIN);

	std::cout <<  "MAC 50 uses mcs " << get_mcs_based_on_destination(50,0) << std::endl;
	std::cout <<  "MAC 281474976710655 uses mcs " << get_mcs_based_on_destination(281474976710655,0) << std::endl;
	std::cout <<  "MAC 2000 uses mcs " << get_mcs_based_on_destination(2000,0) << std::endl;

	std::cout << "Remove 500 was " << remove_dest_mcs_tupple(500) << std::endl;
	std::cout << "Remove 2000 was " << remove_dest_mcs_tupple(2000) << std::endl;

	std::cout <<  "MAC 2000 uses mcs " << get_mcs_based_on_destination(2000,0) << std::endl;

	add_dest_mcs_tupple(4000,80,DEFAULT_TX_GAIN);
	std::cout <<  "MAC 4000 uses mcs " << get_mcs_based_on_destination(4000,0) << std::endl;

	print_dest_mcs_table();

}

void test_RX_channel_functions(){


	add_dest_channel_tupple(281474976710655, 2, DEFAULT_TX_GAIN,0);
	add_dest_channel_tupple(2000,3, DEFAULT_TX_GAIN,0);
	add_dest_channel_tupple(3000,4, DEFAULT_TX_GAIN,0);
	add_dest_channel_tupple(4000,5, DEFAULT_TX_GAIN,0);

	std::cout <<  "MAC 50 uses rx_channel " << get_channel_based_on_destination(50) << std::endl;
	std::cout <<  "MAC 281474976710655 uses rx_channel " << get_channel_based_on_destination(281474976710655) << std::endl;
	std::cout <<  "MAC 2000 uses rx_channel " << get_channel_based_on_destination(2000) << std::endl;

	std::cout << "Remove 500 was " << remove_dest_mcs_tupple(500) << std::endl;
	std::cout << "Remove 2000 was " << remove_dest_mcs_tupple(2000) << std::endl;

	std::cout <<  "MAC 2000 uses rx_channel " << get_channel_based_on_destination(2000) << std::endl;

	add_dest_channel_tupple(4000,6, DEFAULT_TX_GAIN,0);
	std::cout <<  "MAC 4000 uses rx_channel " << get_channel_based_on_destination(4000) << std::endl;

	print_dest_mcs_table();

}

/**
 * End of Destination - MCS table and related functions
 */

bool received_app_packet_in_list(mac_address_t source_address, mac_index_t mac_packet_index)
{
    std::pair<mac_address_t, mac_index_t> received_app_packet = std::make_pair(source_address, mac_packet_index);

    for (auto it : received_app_packets) {
    	if(it == received_app_packet)
    		return true;
    }

    received_app_packets.push_back(received_app_packet);
    return false;
}

/*bool control_ack_in_list(mac_address_t source_address, mac_index_t mac_packet_index)
{
    std::pair<mac_address_t, mac_index_t> control_ack = std::make_pair(source_address, mac_packet_index);

    for (auto it : control_acks) {
    	if(it == control_ack)
    		return true;
    }

    control_acks.push_back(control_ack);
    return false;
}

void remove_control_ack_in_list(mac_address_t source_address, mac_index_t mac_packet_index)
{
	std::pair<mac_address_t, mac_index_t> control_ack = std::make_pair(source_address, mac_packet_index);

	uint16_t i = 0;

  for (auto it : control_acks) {
  	if(it == control_ack)
  	{
  		control_acks.erase(control_acks.begin() + i);
  		break;
  	}
  	i++;
  }
}

bool check_received_control_ack(mac_address_t source_address, mac_index_t mac_packet_index)
{
  std::pair<mac_address_t, mac_index_t> received_control_ack = std::make_pair(source_address, mac_packet_index);

  for (auto it : received_control_acks) {
  	if(it == received_control_ack)
  		return true;
  }

  return false;
}

void add_received_control_ack(mac_address_t source_address, mac_index_t mac_packet_index)
{
	std::pair<mac_address_t, mac_index_t> received_control_ack = std::make_pair(source_address, mac_packet_index);

	received_control_acks.push_back(received_control_ack);
}*/

}
