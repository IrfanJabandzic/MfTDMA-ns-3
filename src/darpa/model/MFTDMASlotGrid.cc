//
// Created by Maxim Claeys on 30/10/2017.
//

#include "MFTDMASlotGrid.h"
#include <string>
#include <iostream>
#include <sstream>
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MFTDMASlotGrid");
NS_OBJECT_ENSURE_REGISTERED (MFTDMASlotGrid);

TypeId
MFTDMASlotGrid::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MFTDMASlotGrid")
    .SetParent<Object> ()
    .SetGroupName("Darpa")
    .AddConstructor<MFTDMASlotGrid> ()
  ;
  return tid;
}

MFTDMASlotGrid::MFTDMASlotGrid()
        : m_num_timeslots(0), m_num_frequencyslots(0)
{

}

void MFTDMASlotGrid::MFTDMASlotGridInit(uint8_t num_timeslots, uint8_t num_frequencyslots)
{
    m_num_timeslots = num_timeslots;
    m_num_frequencyslots = num_frequencyslots;

    m_slot_info = new SlotInfo*[m_num_timeslots];
    for(uint8_t i=0; i<m_num_timeslots; i++){
        m_slot_info[i]=new SlotInfo[m_num_frequencyslots];
    }
    NS_LOG_INFO("Timeslot " << (uint16_t)m_num_timeslots << " Frequency " << (uint16_t)m_num_frequencyslots);
}

MFTDMASlotGrid::~MFTDMASlotGrid()
{
    for(uint8_t i=0; i<m_num_timeslots; i++){
        delete[] m_slot_info[i];
    }
    delete[] m_slot_info;
}

uint8_t MFTDMASlotGrid::getNumTimeslots() const
{
    return m_num_timeslots;
}

uint8_t MFTDMASlotGrid::getNumFrequencyslots() const
{
    return m_num_frequencyslots;
}

const SlotInfo& MFTDMASlotGrid::getSlotInfo(uint8_t timeslot, uint8_t frequencyslot) const
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        static SlotInfo default_info;
        return default_info;
    }
    return m_slot_info[timeslot][frequencyslot];
}

bool MFTDMASlotGrid::isSlotFree(uint8_t timeslot, uint8_t frequencyslot) const
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    return m_slot_info[timeslot][frequencyslot].state == SlotState::FREE;
}

bool MFTDMASlotGrid::isTimeslotAvailableTX(uint8_t timeslot) const
{
    if(timeslot >= m_num_timeslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "! Bounds are " << (uint16_t)m_num_timeslots << ". Undefined behavior now!!");
        return false;
    }
    for(uint8_t f=0; f<m_num_frequencyslots; f++){
        SlotInfo info = m_slot_info[timeslot][f];
#ifndef USE_RX_TX_SAME_TIMESLOT
        if(info.state != SlotState::FREE){
#else
        if(info.state == SlotState::TX || info.state == SlotState::PROPOSED_TX || info.state == SlotState::RESERVED ){
#endif
            return false;
        }
    }
    return true;
}

bool MFTDMASlotGrid::isTimeslotAvailableRX(uint8_t timeslot) const
{
    if(timeslot >= m_num_timeslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "! Bounds are " << (uint16_t)m_num_timeslots << ". Undefined behavior now!!");
        return false;
    }
    for(uint8_t f=0; f<m_num_frequencyslots; f++){
        SlotInfo info = m_slot_info[timeslot][f];
#ifndef USE_RX_TX_SAME_TIMESLOT
        if(info.state != SlotState::FREE){
#else
        if(info.state == SlotState::RX || info.state == SlotState::PROPOSED_RX || info.state == SlotState::RESERVED ){
#endif
            return false;
        }
    }
    return true;
}

bool MFTDMASlotGrid::isSlotAvailableTX(uint8_t timeslot, uint8_t frequencyslot) const
{
    return isTimeslotAvailableTX(timeslot) && isSlotFree(timeslot, frequencyslot);
}

bool MFTDMASlotGrid::isSlotAvailableRX(uint8_t timeslot, uint8_t frequencyslot) const
{
    return isTimeslotAvailableRX(timeslot) && isSlotFree(timeslot, frequencyslot);
}

bool MFTDMASlotGrid::isTimeslotFree(uint8_t timeslot) const
{
    if(timeslot >= m_num_timeslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "! Bounds are " << (uint16_t)m_num_timeslots << ". Undefined behavior now!!");
        return false;
    }
    for(uint8_t f=0; f<m_num_frequencyslots; f++){
        if(!isSlotFree(timeslot, f)){
            return false;
        }
    }
    return true;
}

std::vector<Slot> MFTDMASlotGrid::getIdleSlots(uint32_t idle_timeout_ms, float rx_multiplier) const
{
    std::vector<Slot> idleslots;
    uint64_t now = clock_get_time_ns();
    uint64_t tx_timeout_ns = idle_timeout_ms*1000000L;
    uint64_t rx_timeout_ns = (uint64_t)((double)tx_timeout_ns*(double)rx_multiplier);
    for(uint8_t t=0; t<m_num_timeslots; t++){
        for(uint8_t f=0; f<m_num_frequencyslots; f++){
            //Only allocated slots that are not already pending removal
            if(isSlotAllocated(t, f) && !isRemovePending(t,f)) {
                const SlotInfo &info = m_slot_info[t][f];
                uint64_t timeout_ns = tx_timeout_ns;
                if(info.state == SlotState::RX){
                    timeout_ns = rx_timeout_ns;
                }
                //A slot will be removed if it was idle for a timeout period
                if ((now - info.last_used) >= timeout_ns) {
                    Slot slot(t, f);
                    idleslots.push_back(slot);
                }
            }
        }
    }
    return idleslots;
}

void MFTDMASlotGrid::freeSlot(uint8_t t, uint8_t f)
{
    if(t >= m_num_timeslots || f >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)t << "-" << (uint16_t)f << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return;
    }
    m_slot_info[t][f].state = SlotState::FREE;
    m_slot_info[t][f].target_mac = 0;
    m_slot_info[t][f].last_used = 0;
    m_slot_info[t][f].pending_removal = 0;
}

bool MFTDMASlotGrid::isSetPendingAllowed(uint8_t t, uint8_t f, SlotState state, uint64_t mac) const
{
    if(t >= m_num_timeslots || f >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)t << "-" << (uint16_t)f << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    //Can only propose RX if there are no if this slot is available for RX
    if(state==SlotState::PROPOSED_RX){
        return isSlotAvailableRX(t, f);
    }
    //In proposed TX state, there should be no slots with RESERVED, TX of PROPOSED_TX state for another mac in the entire timeslot
    else if(state==SlotState::PROPOSED_TX){

#ifdef USE_RX_TX_SAME_TIMESLOT
    		if(m_slot_info[t][f].state != SlotState::FREE)
    			return false;
#endif

        for(uint8_t i=0; i<m_num_frequencyslots; i++){
            SlotInfo info = m_slot_info[t][i];
            //If slot is not free && not the case that it is proposed_tx for the same mac
#ifndef USE_RX_TX_SAME_TIMESLOT
            if(info.state != SlotState::FREE && !(info.state == SlotState::PROPOSED_TX && info.target_mac == mac)){
#else
            if((info.state != SlotState::FREE && info.state != SlotState::RX && info.state != SlotState::PROPOSED_RX) && !(info.state == SlotState::PROPOSED_TX && info.target_mac == mac)){
#endif
            		return false;
            }
        }
        return true;
    }
    return false;
}

bool MFTDMASlotGrid::setSlotPending(uint8_t t, uint8_t f, SlotState state, uint64_t mac)
{
    if(t >= m_num_timeslots || f >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)t << "-" << (uint16_t)f << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    //Can only set to pending if timeslot was not occupied
    if(isSetPendingAllowed(t, f, state, mac)){
        m_slot_info[t][f].state = state;
        m_slot_info[t][f].target_mac = mac;
        m_slot_info[t][f].last_used = 0;
        m_slot_info[t][f].pending_removal = 0;
        return true;
    }else{
        return false;
    }
}

bool MFTDMASlotGrid::setSlotAllocated(uint8_t t, uint8_t f, SlotState state, uint64_t mac)
{
    if(t >= m_num_timeslots || f >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)t << "-" << (uint16_t)f << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    if( ((state==SlotState::TX && m_slot_info[t][f].state==SlotState::PROPOSED_TX) || (state==SlotState::RX && m_slot_info[t][f].state==SlotState::PROPOSED_RX)) && m_slot_info[t][f].target_mac==mac ){
        m_slot_info[t][f].state = state;
        m_slot_info[t][f].last_used = clock_get_time_ns();
        m_slot_info[t][f].pending_removal = 0;
        return true;
    }else{
        return false;
    }
}

bool MFTDMASlotGrid::setSlotReserved(uint8_t timeslot, uint8_t frequencyslot)
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    NS_LOG_INFO("Set reserved slot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots);

    //Actually it is only allowed if the timeslot was not occupied. Should always be true, but safety handle
    bool allowed = isTimeslotFree(timeslot);
    m_slot_info[timeslot][frequencyslot].state = SlotState::RESERVED;
    m_slot_info[timeslot][frequencyslot].target_mac = 0;
    m_slot_info[timeslot][frequencyslot].last_used = 0;
    m_slot_info[timeslot][frequencyslot].pending_removal = 0;
    return allowed;
}

bool MFTDMASlotGrid::setPendingRemoval(uint8_t timeslot, uint8_t frequencyslot, uint64_t id)
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    if(isSlotAllocated(timeslot, frequencyslot) && !isRemovePending(timeslot, frequencyslot)) {
        m_slot_info[timeslot][frequencyslot].pending_removal = id;
        return true;
    }else{
        return false;
    }
}

bool MFTDMASlotGrid::updateUsage(uint8_t t, uint8_t f)
{
    if(t >= m_num_timeslots || f >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)t << "-" << (uint16_t)f << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    if(isSlotAllocated(t, f)){
        m_slot_info[t][f].last_used = clock_get_time_ns();
        return true;
    }else{
        return false;
    }
}

void MFTDMASlotGrid::abortPendingRemoval(uint64_t id)
{
    for(uint8_t t=0; t<m_num_timeslots; t++){
        for(uint8_t f=0; f<m_num_frequencyslots; f++){
            if(m_slot_info[t][f].pending_removal == id){
                m_slot_info[t][f].pending_removal = 0;
                return;
            }
        }
    }
}

void MFTDMASlotGrid::abortRXProposal(uint64_t mac)
{
    for(uint8_t t=0; t<m_num_timeslots; t++){
        for(uint8_t f=0; f<m_num_frequencyslots; f++){
            if(m_slot_info[t][f].state == SlotState::PROPOSED_RX && m_slot_info[t][f].target_mac == mac){
                freeSlot(t, f);
                return;
            }
        }
    }
}

void MFTDMASlotGrid::abortTXProposals(uint64_t mac)
{
    for(uint8_t t=0; t<m_num_timeslots; t++){
        for(uint8_t f=0; f<m_num_frequencyslots; f++){
            if(m_slot_info[t][f].state == SlotState::PROPOSED_TX && m_slot_info[t][f].target_mac == mac){
                freeSlot(t, f);
            }
        }
    }
}

Slot MFTDMASlotGrid::executePendingRemoval(uint64_t id)
{
    for(uint8_t t=0; t<m_num_timeslots; t++){
        for(uint8_t f=0; f<m_num_frequencyslots; f++){
            if(m_slot_info[t][f].pending_removal == id){
                freeSlot(t, f);
                return Slot(t,f);
            }
        }
    }
    return Slot();
}

bool MFTDMASlotGrid::isSlotAllocated(uint8_t timeslot, uint8_t frequencyslot) const
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    return (m_slot_info[timeslot][frequencyslot].state==SlotState::RX || m_slot_info[timeslot][frequencyslot].state==SlotState::TX);
}

bool MFTDMASlotGrid::isSlotAllocatedFor(uint8_t timeslot, uint8_t frequencyslot, uint64_t mac) const
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    return isSlotAllocated(timeslot, frequencyslot) && m_slot_info[timeslot][frequencyslot].target_mac == mac;
}

bool MFTDMASlotGrid::isSlotPendingFor(uint8_t timeslot, uint8_t frequencyslot, uint64_t mac) const
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    return (m_slot_info[timeslot][frequencyslot].state==SlotState::PROPOSED_TX || m_slot_info[timeslot][frequencyslot].state==SlotState::PROPOSED_RX) && m_slot_info[timeslot][frequencyslot].target_mac == mac;
}

bool MFTDMASlotGrid::isSlotReserved(uint8_t timeslot, uint8_t frequencyslot) const
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    return m_slot_info[timeslot][frequencyslot].state==SlotState::RESERVED;
}

bool MFTDMASlotGrid::isRemovePending(uint8_t timeslot, uint8_t frequencyslot) const
{
    if(timeslot >= m_num_timeslots || frequencyslot >= m_num_frequencyslots){
        NS_LOG_ERROR("ERROR: Defined out of bounds timeslot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << "! Bounds are " << (uint16_t)m_num_timeslots << "-" << (uint16_t)m_num_frequencyslots << ". Undefined behavior now!!");
        return false;
    }
    return m_slot_info[timeslot][frequencyslot].pending_removal != 0;
}

void MFTDMASlotGrid::logTableState() const
{
    printUsage();
    NS_LOG_DEBUG("Will print the current table state");
    for(uint8_t f=m_num_frequencyslots; f>=1; f--){
        std::stringstream ss;
        ss << "Frequency slot " << std::to_string(f-1) << " - ";
        for(uint8_t t=0; t<m_num_timeslots; t++){
            ss << std::to_string(t) << ": ";
            SlotInfo info = m_slot_info[t][f-1];
            if(info.state == SlotState::FREE){
                ss << "UN";
            }else if(info.state == SlotState::PROPOSED_TX){
                ss << "P_TX|M:" << info.target_mac;
            }else if(info.state == SlotState::PROPOSED_RX){
                ss << "P_RX|M:" << info.target_mac;
            }else if(info.state == SlotState::TX){
                ss << "TX|M:" << info.target_mac;
            }else if(info.state == SlotState::RX){
                ss << "RX|M:" << info.target_mac;
            }else if(info.state == SlotState::RESERVED){
                ss << "CB";
            }
            if(t<m_num_timeslots-1){
                ss << ", ";
            }
        }
        NS_LOG_DEBUG(ss.str());
    }
}

void MFTDMASlotGrid::printUsage() const
{
    uint32_t rx_count=0;
    uint32_t tx_count=0;
    uint32_t reserved_count=0;
    for (uint8_t t = 0; t < m_num_timeslots; t++) {
        for(uint8_t f=0; f < m_num_frequencyslots; f++) {
            switch(m_slot_info[t][f].state){
                case SlotState::TX:
                    tx_count++;
                    break;
                case SlotState::RX:
                    rx_count++;
                    break;
                case SlotState::RESERVED:
                    reserved_count++;
                    break;
                default:
                    break;
            }
        }
    }
    NS_LOG_INFO("[Slotusage] CB: " << reserved_count << ", TX: " << tx_count << ", RX: " << rx_count << ", TX+RX: " << tx_count+rx_count << ", Total: " << tx_count+rx_count+reserved_count);
}

}
