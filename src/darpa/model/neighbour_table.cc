/*
 * neighbour_table.cpp
 *
 *  Created on: Feb 5, 2018
 *      Author: Irfan Jabandzic
 */

#include "neighbour_table.h"
#include "ns3/traced-value.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (neighbour_table);

TypeId
neighbour_table::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::neighbour_table")
    .SetParent<Object> ()
    .SetGroupName("Darpa")
    .AddConstructor<neighbour_table> ()
		.AddAttribute ("Neighbors", "Number of neighbors",
									 UintegerValue (2),
									 MakeUintegerAccessor (&neighbour_table::m_neighbors),
									 MakeUintegerChecker<uint16_t> ())
  ;
  return tid;
}

neighbour_table::neighbour_table()
{
}

void neighbour_table::neighbour_table_init()
{
	table_size = 0;
	//m_neighbors = neighbors_count;
  //NS_LOG_UNCOND("Number of neighbours=" << this->m_neighbors);
}

neighbour_table::~neighbour_table()
{

}

void neighbour_table::neighbour_table_timeout_update()
{
#if true
	for(uint16_t i = 0; i < table_size; i++)
	{
		if(table[i].second == false)
		{
			table.erase(table.begin() + i);
			table_size--;
		}
	}
#endif

//TODO this update is important for mobility
#if false
	for(int16_t i = (table_size - 1); i >= 0; i--)
	{
		if(table[i].second == false)
		{
			table.erase(table.begin() + i);
			table_size--;
		}
		else
			table[i].second = false;
	}
#endif

	return;
}

void neighbour_table::neighbour_table_manual_update(mac_index_t mac)
{
	for(uint16_t i = 0; i < table_size; i++)
	{
		if(table[i].first == mac)
		{
			table[i].second = true;
			return;
		}
	}

	table.push_back(std::make_pair(mac, true));
	table_size++;

	return;
}

}
