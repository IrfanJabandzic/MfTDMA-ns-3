//
// Created by Maxim Claeys on 28/10/2017.
//

#ifndef RANDOMSLOTSELECTION_H
#define RANDOMSLOTSELECTION_H

#include "SlotSelectionAIModule.h"
#include <map>
#include "ns3/traced-value.h"
#include "ns3/trace-source-accessor.h"

namespace ns3 {

#define TRANSACTION_TIMEOUT_MS 12000 //3000
#define IDLE_TIMEOUT_MS 10000
#define EXTERNAL_IDLE_TIMEOUT_MS 60000 //20000
#define REMOVAL_THRESHOLD 0.8 //0.5
#define CONSEC_FAIL_THRESHOLD 2 //4
#define ACK_RECEIVED_THRESHOLD 5 //10

class RandomSlotSelection : public SlotSelectionAIModule
{
private:
  //Stats
	TracedValue<uint32_t> slot_allocation_tried_count;
	TracedValue<uint32_t> slot_allocation_count;
	TracedValue<uint32_t> slot_removal_count;
	TracedValue<uint32_t> slot_allocation_durations;
	TracedValue<uint32_t> slot_allocation_duration_min;
	TracedValue<uint32_t> slot_allocation_duration_max;
protected:
    float m_removal_threshold;
    uint8_t m_consecutive_frames_threshold;
    std::map<uint32_t, uint64_t> m_curframes;
    std::map<uint32_t, uint8_t> m_consec_failing_frames;
    std::map<uint32_t, uint32_t> m_successcount;
    std::map<uint32_t, uint32_t> m_failcount;

    virtual void init(uint64_t seed);
    virtual void terminate();

    virtual std::vector<Slot> propose_tx_slots(uint64_t mac, uint32_t num_slots);
    virtual Slot select_rx_slot(uint64_t mac, const std::vector<Slot>& proposed);
    virtual std::vector<Slot> propose_tx_slots_exposed(uint64_t mac, uint32_t num_slots);
    virtual Slot select_rx_slot_exposed(uint64_t mac, const std::vector<Slot>& proposed);
    virtual void handle_statistics(const Internal& m);
    virtual void handle_rf_monitor_info(const Internal& m);
    virtual void notify_slot_allocated(uint8_t timeslot_num, uint8_t frequencyslot_num);
    virtual void notify_slot_removed(uint8_t timeslot_num, uint8_t frequencyslot_num);
    virtual void notify_external_slot_allocated(uint8_t timeslot_num, uint8_t frequencyslot_num, bool new_allocation);
    virtual void notify_external_slot_removed(uint8_t timeslot_num, uint8_t frequencyslot_num);
    virtual void stats_slot_removal();
    virtual void stats_slot_allocation(uint64_t pending_tx_proposal);

    virtual void process_ack_statistic(const Mac_ack_stat& stat);
public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId (void);
    RandomSlotSelection();
    void RandomSlotSelectionInit(float removal_threshold, uint8_t consecutive_frames_threshold, uint8_t num_timeslots, uint8_t num_frequencyslots, uint8_t slot_duration_ms, uint32_t transaction_timeout_ms, uint32_t idle_timeout_ms, uint32_t external_idle_timeout_ms, uint64_t seed);
    virtual ~RandomSlotSelection();

    virtual void clear_external_table();
};

}

#endif //RANDOMSLOTSELECTION_H
