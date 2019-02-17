//
// Created by Maxim Claeys on 28/10/2017.
//

#include "RandomSlotSelection.h"
#include <stdlib.h>
#include <algorithm>
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("RandomSlotSelection");
NS_OBJECT_ENSURE_REGISTERED (RandomSlotSelection);

TypeId
RandomSlotSelection::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RandomSlotSelection")
    .SetParent<SlotSelectionAIModule> ()
    .SetGroupName("Darpa")
    .AddConstructor<RandomSlotSelection> ()
		.AddTraceSource ("SlotAllocationCount",
										 "Slot allocation count.",
										 MakeTraceSourceAccessor (&RandomSlotSelection::slot_allocation_count),
										 "ns3::TracedValueCallback::Uint32")
		.AddTraceSource ("SlotRemovalCount",
										 "Slot removal count.",
										 MakeTraceSourceAccessor (&RandomSlotSelection::slot_removal_count),
										 "ns3::TracedValueCallback::Uint32")
  ;
  return tid;
}

RandomSlotSelection::RandomSlotSelection()
    : SlotSelectionAIModule(), m_removal_threshold(0), m_consecutive_frames_threshold(0)
{
	slot_allocation_count = 0;
	slot_removal_count = 0;
}

void RandomSlotSelection::RandomSlotSelectionInit(float removal_threshold, uint8_t consecutive_frames_threshold, uint8_t num_timeslots, uint8_t num_frequencyslots, uint8_t slot_duration_ms, uint32_t transaction_timeout_ms, uint32_t idle_timeout_ms, uint32_t external_idle_timeout_ms, uint64_t seed)
{
    SlotSelectionAIModuleInit(num_timeslots, num_frequencyslots, slot_duration_ms, transaction_timeout_ms, idle_timeout_ms, external_idle_timeout_ms);
    m_removal_threshold = removal_threshold;
    m_consecutive_frames_threshold = consecutive_frames_threshold;

    NS_LOG_INFO("Random slot selection module constructed");

    //init(seed); //TODO srand function already called
}

RandomSlotSelection::~RandomSlotSelection()
{
    NS_LOG_INFO("Random slot selection module destructed");
}

void RandomSlotSelection::init(uint64_t seed)
{
    srand(seed); //TODO use some other seed
    NS_LOG_INFO("Initialization done");
}

void RandomSlotSelection::terminate()
{
    NS_LOG_INFO("Module terminated");
}

std::vector<Slot> RandomSlotSelection::propose_tx_slots(uint64_t mac, uint32_t num_slots)
{
    std::vector<Slot> proposal;
    //Loop over all slots and add the free ones that are not used by anyone else
    uint8_t num_timeslots = m_slotgrid->getNumTimeslots();
    uint8_t num_frequencyslots = m_slotgrid->getNumFrequencyslots();
    for(uint8_t t=0; t<num_timeslots; t++){
        if(m_slotgrid->isTimeslotAvailableTX(t)) {
            //All slots in this timeslot are free, so can add them all
            for (uint8_t f = 0; f < num_frequencyslots; f++) {
                if (m_slotgrid->isSlotAvailableTX(t, f) && m_external_slotgrid->isSlotFree(t, f)) {
                    Slot p(t, f);
                    proposal.push_back(p);
                }
            }
        }
    }
    //If no such slots are found, select free ones, even though allocated by others
    if(proposal.empty()){
        NS_LOG_LOGIC("I did not find any slots which are not used externally");
    }
    else
  		slot_allocation_count++;

    NS_LOG_LOGIC("In total, " << proposal.size() <<" slots were available. Will remove random ones until I have " << num_slots << " slots");
    //Remove random slots as long as we have too much
    while(proposal.size() > num_slots){
        uint8_t pos = rand() % proposal.size();
        proposal.erase(proposal.begin()+pos);
    }
    //Shuffle the remaining proposals
    std::random_shuffle(proposal.begin(), proposal.end());
    return proposal;
}

Slot RandomSlotSelection::select_rx_slot(uint64_t mac, const std::vector<Slot>& proposed)
{
    Slot selected; //255,255 is the default
    //Search for the first slot that is available for me and not used by anyone else
    for(Slot slot : proposed){
        uint8_t t = slot.timeslot_num;
        uint8_t f = slot.frequencyslot_num;
        NS_LOG_LOGIC("Will check slot " << (uint16_t)t << "-" << (uint16_t)f);
        //The slot should still be free and RX should be allowed
        if(m_slotgrid->isSlotAvailableRX(t, f) && m_external_slotgrid->isSlotFree(t, f)){
            selected.timeslot_num = t;
            selected.frequencyslot_num = f;
            break;
        }
    }
    //If no such slot is found, select a free one for me
    if(selected.timeslot_num==255 && selected.frequencyslot_num==255){
        NS_LOG_LOGIC("All of the proposals either didn't work for me or were not free externally");
    }
    return selected;
}

void RandomSlotSelection::handle_statistics(const Internal &m)
{
    //Check if this is a MAC ACK statistic so I can update the time for idle detection
    if(m.attribute == MESSAGE_STATS){
        Mac_stat macstat = m.stats.mac_stat;
        if(macstat.attribute == MAC_STAT_TX){ //TODO real flag
            Mac_tx_stat txstat = macstat.tx_stat;
            int num_ackstats = txstat.ack_stat.size();
            for (int i = 0; i < num_ackstats; i++) {
                Mac_ack_stat ack_stat = txstat.ack_stat[i];
                process_ack_statistic(ack_stat);
            }
        }
    }
}

void RandomSlotSelection::process_ack_statistic(const Mac_ack_stat &stat)
{
    uint8_t t = stat.slot_index;
    uint8_t f = stat.tx_channel;
    //Only process statistics for slots that are still allocated
    if(m_slotgrid->isSlotAllocated(t, f)) {
        uint8_t num_freqslots = m_slotgrid->getNumFrequencyslots();
        uint64_t frame_dur_ns = (uint64_t)(m_slotgrid->getNumTimeslots() * m_slot_duration_ms) * 1000000L;
        uint64_t now = clock_get_time_ns();
        //This will increase every framedur
        uint64_t curframe = now / frame_dur_ns;
        uint32_t slot_id = t * num_freqslots + f;

        if (curframe != m_curframes[slot_id]) {
            uint32_t success = m_successcount[slot_id];
            uint32_t fail = m_failcount[slot_id];
            float ratio = 0.0;
            if (success + fail > 0) {
                ratio = (float) success / (float) (success + fail);
            }
            NS_LOG_LOGIC(
                    "[SLOT_MON] In last frame, slot " << (uint16_t)t << "-" << (uint16_t)f << " I had " << success << " success, " << fail << " fails = " << success + fail << " acks. Ratio " << ratio);
            //At least 10 packets should have been send to avoid removing slots too early
            if (success + fail > ACK_RECEIVED_THRESHOLD) {
                if (ratio < m_removal_threshold) {
                    m_consec_failing_frames[slot_id] = m_consec_failing_frames[slot_id] + 1;
                    NS_LOG_DEBUG(
                            "[SLOT_MON] The success ratio of slot " << (uint16_t)t << "-" << (uint16_t)f << " was below the threshold for " << m_consec_failing_frames[slot_id] << " consecutive frames");
                    if (m_consec_failing_frames[slot_id] >= m_consecutive_frames_threshold) {
                        if (!m_slotgrid->isRemovePending(t, f)) {
                            NS_LOG_DEBUG(
                                    "[SLOT_MON] The number of consecutive failing frames for slot " << (uint16_t)t << "-" << (uint16_t)f << " reached the threshold " << m_consecutive_frames_threshold << " so this slot will be removed");
                            initiate_slot_removal(t, f);
                        } else {
                            NS_LOG_DEBUG(
                                    "[SLOT_MON] Slot " << (uint16_t)t << "-" << (uint16_t)f << " should be removed and removal is already pending");
                        }
                    }
                } else {
                    m_consec_failing_frames[slot_id] = 0;
                }
            }
            //Reset the counters because its a new frame
            m_curframes[slot_id] = curframe;
            m_successcount[slot_id] = 0;
            m_failcount[slot_id] = 0;
        }

        //Increment the counter
        if (stat.status == Mac_ack_stat::ACKSTATUS_SUCCESS) {
            m_successcount[slot_id]++;
        } else {
            m_failcount[slot_id]++;
        }
    }
}

void RandomSlotSelection::handle_rf_monitor_info(const Internal &m)
{
    //Do nothing
}

void RandomSlotSelection::notify_slot_allocated(uint8_t timeslot_num, uint8_t frequencyslot_num)
{
    uint8_t num_freqslots = m_slotgrid->getNumFrequencyslots();
    uint32_t slot_id = timeslot_num * num_freqslots + frequencyslot_num;
    uint64_t frame_dur_ns = (uint64_t)(m_slotgrid->getNumTimeslots() * m_slot_duration_ms) * 1000000L;
    uint64_t now = clock_get_time_ns();
    uint64_t curframe = now/frame_dur_ns;
    m_curframes[slot_id] = curframe;
    m_consec_failing_frames[slot_id] = 0;
    m_successcount[slot_id] = 0;
    m_failcount[slot_id] = 0;
}

void RandomSlotSelection::notify_slot_removed(uint8_t timeslot_num, uint8_t frequencyslot_num)
{
    uint8_t num_freqslots = m_slotgrid->getNumFrequencyslots();
    uint32_t slot_id = timeslot_num * num_freqslots + frequencyslot_num;
    m_curframes.erase(slot_id);
    m_consec_failing_frames.erase(slot_id);
    m_successcount.erase(slot_id);
    m_failcount.erase(slot_id);
    NS_LOG_LOGIC("Slotselection info for slot " << (uint16_t)timeslot_num << "-" << (uint16_t)frequencyslot_num << " was deleted");
}

void RandomSlotSelection::notify_external_slot_removed(uint8_t timeslot_num, uint8_t frequencyslot_num)
{

}

void RandomSlotSelection::stats_slot_removal()
{
	slot_removal_count++;
}

void RandomSlotSelection::notify_external_slot_allocated(uint8_t timeslot_num, uint8_t frequencyslot_num, bool new_allocation)
{

}

}
