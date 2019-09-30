//
// Created by Maxim Claeys on 15/11/2017.
//

#include "MFTDMAExternalSlotGrid.h"
#include "MFTDMASlotGrid.h"
#include <string>
#include <iostream>
#include <sstream>
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MFTDMAExternalSlotGrid");
NS_OBJECT_ENSURE_REGISTERED (MFTDMAExternalSlotGrid);

TypeId
MFTDMAExternalSlotGrid::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MFTDMAExternalSlotGrid")
    .SetParent<Object> ()
    .SetGroupName("Darpa")
    .AddConstructor<MFTDMAExternalSlotGrid> ()
  ;
  return tid;
}

MFTDMAExternalSlotGrid::MFTDMAExternalSlotGrid()
    : m_num_timeslots(0), m_num_frequencyslots(0)
{

}

void MFTDMAExternalSlotGrid::MFTDMAExternalSlotGridInit(uint8_t num_timeslots, uint8_t num_frequencyslots)
{
    m_num_timeslots = num_timeslots;
    m_num_frequencyslots = num_frequencyslots;

    m_slot_info = new ExternalSlotInfo*[m_num_timeslots];
    for(uint8_t i=0; i<m_num_timeslots; i++){
        m_slot_info[i]=new ExternalSlotInfo[m_num_frequencyslots];
    }
}

MFTDMAExternalSlotGrid::~MFTDMAExternalSlotGrid()
{
    for(uint8_t i=0; i<m_num_timeslots; i++){
        delete[] m_slot_info[i];
    }
    delete[] m_slot_info;
}

uint8_t MFTDMAExternalSlotGrid::getNumTimeslots() const
{
    return m_num_timeslots;
}

uint8_t MFTDMAExternalSlotGrid::getNumFrequencyslots() const
{
    return m_num_frequencyslots;
}

bool MFTDMAExternalSlotGrid::isSlotFree(uint8_t timeslot, uint8_t frequencyslot) const
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    return m_slot_info[timeslot][frequencyslot].state == ExternalSlotState::FREE;
}

bool MFTDMAExternalSlotGrid::isSlotFreeTx(uint8_t timeslot, uint8_t frequencyslot) const
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    return m_slot_info[timeslot][frequencyslot].state == ExternalSlotState::USED_RX;
}

bool MFTDMAExternalSlotGrid::isSlotFreeRx(uint8_t timeslot, uint8_t frequencyslot) const
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    return m_slot_info[timeslot][frequencyslot].state == ExternalSlotState::USED_TX;
}

bool MFTDMAExternalSlotGrid::setExternalSlotFree(uint8_t timeslot, uint8_t frequencyslot)
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    //There will be a change when the slot is used right now
    bool change = (m_slot_info[timeslot][frequencyslot].state == ExternalSlotState::USED);
    m_slot_info[timeslot][frequencyslot].state = ExternalSlotState::FREE;
    m_slot_info[timeslot][frequencyslot].last_announcement = 0;
    return change;
}

bool MFTDMAExternalSlotGrid::setExternalSlotFreeTx(uint8_t timeslot, uint8_t frequencyslot)
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    //There will be a change when the slot is used right now
    bool change = false;
    if(m_slot_info[timeslot][frequencyslot].state == ExternalSlotState::USED)
  	{
    	change = true;
    	m_slot_info[timeslot][frequencyslot].state = ExternalSlotState::USED_RX;
  	}
    else if(m_slot_info[timeslot][frequencyslot].state == ExternalSlotState::USED_TX)
  	{
    	change = true;
    	m_slot_info[timeslot][frequencyslot].state = ExternalSlotState::FREE;
  	}

    m_slot_info[timeslot][frequencyslot].last_announcement_tx = 0;
    return change;
}

bool MFTDMAExternalSlotGrid::setExternalSlotFreeRx(uint8_t timeslot, uint8_t frequencyslot)
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    //There will be a change when the slot is used right now
    bool change = false;
    if(m_slot_info[timeslot][frequencyslot].state == ExternalSlotState::USED)
  	{
    	change = true;
    	m_slot_info[timeslot][frequencyslot].state = ExternalSlotState::USED_TX;
  	}
    else if(m_slot_info[timeslot][frequencyslot].state == ExternalSlotState::USED_RX)
  	{
    	change = true;
    	m_slot_info[timeslot][frequencyslot].state = ExternalSlotState::FREE;
  	}

    m_slot_info[timeslot][frequencyslot].last_announcement_rx = 0;
    return change;
}

bool MFTDMAExternalSlotGrid::setExternalSlotUsed(uint8_t timeslot, uint8_t frequencyslot)
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    //There will be a change when the slot is free right now
    bool change = (m_slot_info[timeslot][frequencyslot].state == ExternalSlotState::FREE);
    m_slot_info[timeslot][frequencyslot].state = ExternalSlotState::USED;
    m_slot_info[timeslot][frequencyslot].last_announcement = clock_get_time_ns();
    return change;
}

bool MFTDMAExternalSlotGrid::setExternalSlotUsedTx(uint8_t timeslot, uint8_t frequencyslot)
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    //There will be a change when the slot is free right now
    bool change = false;

    if(m_slot_info[timeslot][frequencyslot].state == ExternalSlotState::FREE)
  	{
    	change = true;
    	m_slot_info[timeslot][frequencyslot].state = ExternalSlotState::USED_TX;
  	}
    else if(m_slot_info[timeslot][frequencyslot].state == ExternalSlotState::USED_RX)
  	{
    	change = true;
    	m_slot_info[timeslot][frequencyslot].state = ExternalSlotState::USED;
  	}

    m_slot_info[timeslot][frequencyslot].last_announcement_tx = clock_get_time_ns();
    return change;
}

bool MFTDMAExternalSlotGrid::setExternalSlotUsedRx(uint8_t timeslot, uint8_t frequencyslot)
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    //There will be a change when the slot is free right now
    bool change = false;

    if(m_slot_info[timeslot][frequencyslot].state == ExternalSlotState::FREE)
  	{
    	change = true;
    	m_slot_info[timeslot][frequencyslot].state = ExternalSlotState::USED_RX;
  	}
    else if(m_slot_info[timeslot][frequencyslot].state == ExternalSlotState::USED_TX)
  	{
    	change = true;
    	m_slot_info[timeslot][frequencyslot].state = ExternalSlotState::USED;
  	}

    m_slot_info[timeslot][frequencyslot].last_announcement_rx = clock_get_time_ns();
    return change;
}

std::vector<Slot> MFTDMAExternalSlotGrid::getIdleSlots(uint32_t timeout_ms) const
{
    std::vector<Slot> idleslots;
    uint64_t now = clock_get_time_ns();
    //uint64_t threshold = clock_get_time_ns() - timeout_ms*1000000L;
    for(uint8_t t=0; t<m_num_timeslots; t++){
        for(uint8_t f=0; f<m_num_frequencyslots; f++){
            //Only check used slots
            if(!isSlotFree(t, f)){
                const ExternalSlotInfo &info = m_slot_info[t][f];
                //if(info.last_announcement < threshold){
                if((now - info.last_announcement) >= timeout_ms*1000000L){
                    //Last announcement was too long ago, will consider the slot free
                    Slot slot(t, f);
                    idleslots.push_back(slot);
                }
            }
        }
    }
    return idleslots;
}

std::vector<Slot> MFTDMAExternalSlotGrid::getIdleSlotsTx(uint32_t timeout_ms) const
{
    std::vector<Slot> idleslots;
    uint64_t now = clock_get_time_ns();
    //uint64_t threshold = clock_get_time_ns() - timeout_ms*1000000L;
    for(uint8_t t=0; t<m_num_timeslots; t++){
        for(uint8_t f=0; f<m_num_frequencyslots; f++){
            //Only check used slots
            if(!isSlotFreeTx(t, f)){
                const ExternalSlotInfo &info = m_slot_info[t][f];
                //if(info.last_announcement < threshold){
                if((now - info.last_announcement_tx) >= timeout_ms*1000000L){
                    //Last announcement was too long ago, will consider the slot free
                    Slot slot(t, f);
                    idleslots.push_back(slot);
                }
            }
        }
    }
    return idleslots;
}

std::vector<Slot> MFTDMAExternalSlotGrid::getIdleSlotsRx(uint32_t timeout_ms) const
{
    std::vector<Slot> idleslots;
    uint64_t now = clock_get_time_ns();
    //uint64_t threshold = clock_get_time_ns() - timeout_ms*1000000L;
    for(uint8_t t=0; t<m_num_timeslots; t++){
        for(uint8_t f=0; f<m_num_frequencyslots; f++){
            //Only check used slots
            if(!isSlotFreeRx(t, f)){
                const ExternalSlotInfo &info = m_slot_info[t][f];
                //if(info.last_announcement < threshold){
                if((now - info.last_announcement_rx) >= timeout_ms*1000000L){
                    //Last announcement was too long ago, will consider the slot free
                    Slot slot(t, f);
                    idleslots.push_back(slot);
                }
            }
        }
    }
    return idleslots;
}

void MFTDMAExternalSlotGrid::clear_table()
{
  for(uint8_t t=0; t<m_num_timeslots; t++){
      for(uint8_t f=0; f<m_num_frequencyslots; f++){
      	if(!isSlotFree(t, f))
      		setExternalSlotFree(t, f);
      }
  }
}

void MFTDMAExternalSlotGrid::logTableState() const
{
    /*printUsage();
    NS_LOG_DEBUG("Will print the current external table state");
    for(uint8_t f=m_num_frequencyslots; f>=1; f--){
        std::stringstream ss;
        ss << "Frequency slot " << std::to_string(f-1) << " - ";
        for(uint8_t t=0; t<m_num_timeslots; t++){
            ss << std::to_string(t) << ": ";
            ExternalSlotInfo info = m_slot_info[t][f-1];
            if(info.state == ExternalSlotState::FREE){
                ss << "F";
            }else{
                ss << "U";
            }
            if(t<m_num_timeslots-1){
                ss << ", ";
            }
        }
        NS_LOG_DEBUG(ss.str());
    }*/
}

void MFTDMAExternalSlotGrid::logTableStateTxRx() const
{
    /*printUsage();
    NS_LOG_DEBUG("Will print the current external table state");
    for(uint8_t f=m_num_frequencyslots; f>=1; f--){
        std::stringstream ss;
        ss << "Frequency slot " << std::to_string(f-1) << " - ";
        for(uint8_t t=0; t<m_num_timeslots; t++){
            ss << std::to_string(t) << ": ";
            ExternalSlotInfo info = m_slot_info[t][f-1];
            if(info.state == ExternalSlotState::FREE){
                ss << "F";
            }else if(info.state == ExternalSlotState::USED_TX){
            		ss << "UT";
            }else if(info.state == ExternalSlotState::USED_RX){
            		ss << "UR";
            }else{
                ss << "U";
            }
            if(t<m_num_timeslots-1){
                ss << ", ";
            }
        }
        NS_LOG_DEBUG(ss.str());
    }*/
}

void MFTDMAExternalSlotGrid::printUsage() const
{
    uint32_t used_count=0;
    for (uint8_t t = 0; t < m_num_timeslots; t++) {
        for(uint8_t f=0; f < m_num_frequencyslots; f++) {
            switch(m_slot_info[t][f].state){
                case ExternalSlotState::USED:
                    used_count++;
                    break;
                default:
                    break;
            }
        }
    }
    NS_LOG_INFO("[ExternalSlotusage] Slots in use: " << used_count);
}

void MFTDMAExternalSlotGrid::printUsageTxRx() const
{
    uint32_t used_count=0;
    uint32_t used_count_tx=0;
    uint32_t used_count_rx=0;
    for (uint8_t t = 0; t < m_num_timeslots; t++) {
        for(uint8_t f=0; f < m_num_frequencyslots; f++) {
            switch(m_slot_info[t][f].state){
                case ExternalSlotState::USED:
                    used_count++;
                    break;
                case ExternalSlotState::USED_TX:
                    used_count_tx++;
                    break;
                case ExternalSlotState::USED_RX:
                    used_count_rx++;
                    break;
                default:
                    break;
            }
        }
    }
    NS_LOG_INFO("[ExternalSlotusage] Slots in use: " << used_count << ", Tx slots in use" << used_count_tx << ", Rx slots in use" << used_count_rx);
}

}
