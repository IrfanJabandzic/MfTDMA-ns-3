//
// Created by Maxim Claeys on 26/10/2017.
//

#ifndef SLOTSELECTIONAIMODULE_H
#define SLOTSELECTIONAIMODULE_H

#include "MFTDMASlotGrid.h"
#include "MFTDMAExternalSlotGrid.h"
#include "slot_manage_protocol.h"
#include <map>
#include <vector>
#include "ns3/object.h"

namespace ns3 {

#define SLOT_ALLOCATION_EXPOSED_NODE
//#define OPTIMIZE_SPECTRUM_USAGE

class MFTDMAMacNetDevice;

class SlotSelectionAIModule : public Object
{
private:
    //Keep track of all pending transactions for timeouts
    std::map<uint64_t, uint64_t> m_pending_tx_proposals; //Maps MAC to time
    std::map<uint64_t, uint64_t> m_pending_rx_proposals; //Maps MAC to time
    std::map<uint64_t, uint64_t> m_pending_removals; //Maps transaction_id to time

    //Message processing functions
    void process_get(const Internal& m);
    void process_set(const Internal& m);
    void process_set_response(const Internal& m);
    void process_stats(const Internal& m);

    //Slot management functions (request_idx passed to use same transaction index in reply)
    void propose_slots(uint64_t dst_mac, std::vector<Slot>& proposal, uint64_t request_idx=UINT64_MAX);
    void select_slot(uint64_t request_idx, const Proposed_Slots& proposal);
    void allocate_slot(uint64_t request_idx, const Slot_Allocation& allocation);
    void remove_slot(uint64_t request_idx, const Slot_Removal& removal);
    void allocate_reserved_slots(uint64_t request_id, const BC_Slots_Announcement& ann);
    //Processing of broadcast usage info about external slot usage
    virtual void process_external_slot_allocated(const External_Slot_Allocation& alloc);
    virtual void process_external_slot_removed(const External_Slot_Removal& rem);
#ifdef SLOT_ALLOCATION_EXPOSED_NODE
    virtual void process_external_slot_allocated_exposed(const External_Slot_Allocation& alloc);
    virtual void process_external_slot_removed_exposed(const External_Slot_Removal& rem);
#endif

    //Check for the different timeouts periodically
    void check_tx_proposal_timeouts();
    void check_rx_proposal_timeouts();
    void check_removal_timeouts();
    void check_idle_timeouts();
    void check_external_idle_timeouts();
    void check_external_idle_timeouts_exposed();
protected:
    //General info about the MFTDMA grid structure
    uint8_t m_slot_duration_ms;
    //Timeout durations
    uint32_t m_transaction_timeout_ms;
    uint32_t m_idle_timeout_ms;
    uint32_t m_external_idle_timeout_ms;

    //Stores the information about the different slots
    Ptr<MFTDMASlotGrid> m_slotgrid;
    Ptr<MFTDMAExternalSlotGrid> m_external_slotgrid;
  	Ptr<MFTDMAMacNetDevice> m_device;

    //Initiate a slot removal because a slot was idle for too long
    void initiate_slot_removal(uint8_t timeslot, uint8_t frequencyslot);
    void initiate_slot_allocation(uint64_t dst_mac, std::vector<Slot>& proposal);


    //FUNCTIONS TO BE IMPLEMENTED BY SUBCLASSES
    //Initialization and termination
    virtual void init(uint64_t seed) = 0;
    virtual void terminate() = 0;
    //Propose at most num_slots possible TX slots for destination mac
    virtual std::vector<Slot> propose_tx_slots(uint64_t mac, uint32_t num_slots) = 0;
    virtual std::vector<Slot> propose_tx_slots_exposed(uint64_t mac, uint32_t num_slots) = 0;
    //Select one of the proposed RX slots for destination mac. If no slot fits: return 255,255
    virtual Slot select_rx_slot(uint64_t mac, const std::vector<Slot>& proposed) = 0;
    virtual Slot select_rx_slot_exposed(uint64_t mac, const std::vector<Slot>& proposed) = 0;
    //All statistics messages are passed to subclass to keep track of scores and stuff
    virtual void handle_statistics(const Internal& m) = 0;
    virtual void handle_rf_monitor_info(const Internal& m) = 0;
    //Subclass is notified when a slot is allocated or removed. Slot info can be found in m_slot_info
    virtual void notify_slot_allocated(uint8_t timeslot_num, uint8_t frequencyslot_num) = 0;
    virtual void notify_slot_removed(uint8_t timeslot_num, uint8_t frequencyslot_num) = 0;
    //Subclass is notified when an external slot is allocated or removed. Slot info can be found in m_external_slot_info
    virtual void notify_external_slot_allocated(uint8_t timeslot_num, uint8_t frequencyslot_num, bool new_allocation) = 0;
    virtual void notify_external_slot_removed(uint8_t timeslot_num, uint8_t frequencyslot_num) = 0;
    virtual void stats_slot_removal() = 0;
    virtual void stats_slot_allocation(uint64_t pending_tx_proposal) = 0;
public:
    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId (void);
    SlotSelectionAIModule();
    void SlotSelectionAIModuleInit(uint8_t num_timeslots, uint8_t num_frequencyslots, uint8_t slot_duration_ms, uint32_t transaction_timeout_ms, uint32_t idle_timeout_ms, uint32_t external_idle_timeout_ms);
    virtual ~SlotSelectionAIModule();

  	Ptr<MFTDMAMacNetDevice> GetDevice (void) const;
  	void SetDevice (Ptr<MFTDMAMacNetDevice> device);

    //Message processing functions
    virtual void process_message(Internal& message);

    //Check for the different timeouts periodically
    virtual void interloop_steps();

    virtual void clear_external_table() = 0;
};

}

#endif //SLOTSELECTIONAIMODULE_H
