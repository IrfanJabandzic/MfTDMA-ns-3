/*
 * neighbour_table.h
 *
 *  Created on: Feb 5, 2018
 *      Author: Irfan Jabandzic
 */

#ifndef MAC_NEIGHBOUR_TABLE_H_
#define MAC_NEIGHBOUR_TABLE_H_

#include <vector>
#include "mac_types.h"
#include "ns3/object.h"
#include "ns3/log.h"

using namespace std;

namespace ns3 {

#define FIXED_NEIGHBOUR_TABLE_UPDATE 5*1000000000UL
#define NEIGHBOUR_TABLE_UPDATE_TIMEOUT FIXED_NEIGHBOUR_TABLE_UPDATE*5

class neighbour_table : public Object {
public:

	static TypeId GetTypeId (void);
	neighbour_table();
	void neighbour_table_init();
	virtual ~neighbour_table();

	void neighbour_table_timeout_update();
	void neighbour_table_manual_update(mac_index_t mac);
	uint16_t neighbour_table_get_size() const { return table_size; }
	//uint16_t neighbour_table_get_size() const { return m_neighbors - 1; }

private:

	uint16_t table_size;
  uint16_t m_neighbors;
	std::vector<std::pair<mac_index_t, bool>> table;
};

}

#endif /* MAC_NEIGHBOUR_TABLE_H_ */
