//
// Created by Maxim Claeys on 26/10/2017.
//

#include "SlotSelectionAIModule.h"
#include "ns3/log.h"
#include "mftdma-mac-net-device.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SlotSelectionAIModule");
NS_OBJECT_ENSURE_REGISTERED (SlotSelectionAIModule);

TypeId
SlotSelectionAIModule::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SlotSelectionAIModule")
    .SetParent<Object> ()
    .SetGroupName("Darpa")
    //.AddConstructor<SlotSelectionAIModule> ()
  ;
  return tid;
}

SlotSelectionAIModule::SlotSelectionAIModule()
    : m_slot_duration_ms(0), m_transaction_timeout_ms(0), m_idle_timeout_ms(0), m_external_idle_timeout_ms(0)
{

}

void SlotSelectionAIModule::SlotSelectionAIModuleInit(uint8_t num_timeslots, uint8_t num_frequencyslots, uint8_t slot_duration_ms, uint32_t transaction_timeout_ms, uint32_t idle_timeout_ms, uint32_t external_idle_timeout_ms)
{
    m_slot_duration_ms = slot_duration_ms;
    m_transaction_timeout_ms = transaction_timeout_ms;
    m_idle_timeout_ms = idle_timeout_ms;
    m_external_idle_timeout_ms = external_idle_timeout_ms;

    m_slotgrid = CreateObject<MFTDMASlotGrid> ();
    m_slotgrid->MFTDMASlotGridInit(num_timeslots, num_frequencyslots);
    m_external_slotgrid = CreateObject<MFTDMAExternalSlotGrid> ();
    m_external_slotgrid->MFTDMAExternalSlotGridInit(num_timeslots, num_frequencyslots);
    NS_LOG_INFO("Constructed a slot selection module with " << (uint16_t)num_timeslots << " timeslots, " << (uint16_t)num_frequencyslots << " frequencyslots, slot duration " << (uint16_t)m_slot_duration_ms << "ms, transaction timeout " << m_transaction_timeout_ms << "ms, idle timeout " << m_idle_timeout_ms << "ms, external idle timeout " << m_external_idle_timeout_ms << "ms");
}

SlotSelectionAIModule::~SlotSelectionAIModule()
{
    NS_LOG_INFO("Destructed the SlotSelectionAIModule");
}

Ptr<MFTDMAMacNetDevice>
SlotSelectionAIModule::GetDevice (void) const
{
  NS_LOG_FUNCTION (this);
  return m_device;
}

void
SlotSelectionAIModule::SetDevice (Ptr<MFTDMAMacNetDevice> device)
{
  NS_LOG_FUNCTION (this << device);
  m_device = device;
}

/**
 * Interloop steps are used to check timeouts:
 * - Proposed slots timeout
 * - Removal transaction timeouts
 * - Idle slot timeouts
 */
void SlotSelectionAIModule::interloop_steps()
{
    //Check if TX proposal timeouts expired
    check_tx_proposal_timeouts();
    //Check if RX proposal timeouts expired
    check_rx_proposal_timeouts();
    //Check if removal timeouts expired
    check_removal_timeouts();
    //Check if idle timeouts have expired
    check_idle_timeouts();
    //Check if we have timeouts for broadcast info
    check_external_idle_timeouts();
}

void SlotSelectionAIModule::check_idle_timeouts()
{
    std::vector<Slot> idle = m_slotgrid->getIdleSlots(m_idle_timeout_ms, 1.5);
    for(Slot slot : idle){
        const SlotInfo& info = m_slotgrid->getSlotInfo(slot.timeslot_num, slot.frequencyslot_num);
        NS_LOG_DEBUG("Idle timeout expired for slot " << (uint16_t)slot.timeslot_num << "-" << (uint16_t)slot.frequencyslot_num << " to mac " << info.target_mac << ". Will remove slot");
        initiate_slot_removal(slot.timeslot_num, slot.frequencyslot_num);
    }
}

void SlotSelectionAIModule::check_external_idle_timeouts()
{
    //Don't check for external idle timeouts when disabled
    if(m_external_idle_timeout_ms > 0) {
        std::vector <Slot> idle = m_external_slotgrid->getIdleSlots(m_external_idle_timeout_ms);
        for (Slot slot : idle) {
            m_external_slotgrid->setExternalSlotFree(slot.timeslot_num, slot.frequencyslot_num);
            NS_LOG_LOGIC("External slot usage timed out for slot " << (uint16_t)slot.timeslot_num << "-" << (uint16_t)slot.frequencyslot_num << ". Will consider free" << (uint16_t)slot.timeslot_num << "-" << (uint16_t)slot.frequencyslot_num);
            notify_external_slot_removed(slot.timeslot_num, slot.frequencyslot_num);
            m_external_slotgrid->logTableState();
        }
    }
}

void SlotSelectionAIModule::check_tx_proposal_timeouts()
{
    uint64_t now = clock_get_time_ns();
    std::map<uint64_t, uint64_t>::iterator it=m_pending_tx_proposals.begin();
    //Loop over all pending TX proposals
    while(it!=m_pending_tx_proposals.end()){
        uint64_t mac = it->first;
        uint64_t timestamp = it->second;
        //If proposal has expired
        if((now - timestamp) >= m_transaction_timeout_ms*1000000L){
            NS_LOG_DEBUG("Pending TX proposal for mac " << mac << " expired. Will cleanup");
            m_slotgrid->abortTXProposals(mac);
            m_pending_tx_proposals.erase(it++);
        }else{
            ++it;
        }
    }
}

void SlotSelectionAIModule::check_rx_proposal_timeouts()
{
    uint64_t now = clock_get_time_ns();
    std::map<uint64_t, uint64_t>::iterator it=m_pending_rx_proposals.begin();
    //Loop over all pending RX proposals
    while(it!=m_pending_rx_proposals.end()){
        uint64_t mac = it->first;
        uint64_t timestamp = it->second;
        //If proposal has expired
        if((now - timestamp) >= m_transaction_timeout_ms*1000000L){
            NS_LOG_DEBUG("Pending RX proposal for mac " << mac << " expired. Will cleanup");
            m_slotgrid->abortRXProposal(mac);
            m_pending_rx_proposals.erase(it++);
        }else{
            ++it;
        }
    }
}

void SlotSelectionAIModule::check_removal_timeouts()
{
    uint64_t now = clock_get_time_ns();
    std::map<uint64_t, uint64_t>::iterator it=m_pending_removals.begin();
    //Loop over all pending removals
    while(it!=m_pending_removals.end()){
        uint64_t id = it->first;
        uint64_t timestamp = it->second;
        //If timeout has expired
        if((now - timestamp) >= m_transaction_timeout_ms*1000000L){
            NS_LOG_DEBUG("Removal transaction with ID " << id << " expired. Will cleanup");
            m_slotgrid->abortPendingRemoval(id);
            m_pending_removals.erase(it++);
        }else{
            ++it;
        }
    }
}

void SlotSelectionAIModule::initiate_slot_removal(uint8_t timeslot, uint8_t frequencyslot)
{
    if(m_slotgrid->isSlotAllocated(timeslot, frequencyslot)) {
        if(!m_slotgrid->isRemovePending(timeslot, frequencyslot)) {
            const SlotInfo &info = m_slotgrid->getSlotInfo(timeslot, frequencyslot);
            Internal message;
            message.module = MODULE_MAC;
            message.attribute = MESSAGE_SET;
            message.set.attribute = SLOT_REMOVAL;

            Slot_Removal remove_msg;

            MFTDMA_Slot slotinfo;
            slotinfo.timeslot_num = timeslot;
            slotinfo.frequency_num = frequencyslot;
            remove_msg.slot = slotinfo;
            remove_msg.mac = info.target_mac;

            message.set.slot_removal = remove_msg;

            //Send the message and capture the index
            message.transaction_index = m_device->MacAIMessageIndexUpdate();

            //Store the pending transaction
            m_pending_removals[message.transaction_index] = clock_get_time_ns();
            m_slotgrid->setPendingRemoval(timeslot, frequencyslot, message.transaction_index);
            NS_LOG_DEBUG("Removal transaction initiated for slot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << " for mac " << info.target_mac);

            //Update slot removal stats
            stats_slot_removal();

            m_device->SendMacAIMessage(message);

        }else{
            NS_LOG_LOGIC("WARNING: Tried to remove a slot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << " that already has a pending removal");
        }
    }else{
        NS_LOG_ERROR("ERROR: Tried to remove a slot " << (uint16_t)timeslot << "-" << (uint16_t)frequencyslot << " that was not allocated. SHOULD NOT HAPPEN!");
    }
}

/**
 * The slot selection module must react to different types of messages
 * - Get
 *      - PROPOSED_SLOTS -> react with Proposed_Slots Get_r message (introduces state->pending)
 *      - SELECTED_SLOT -> select a single slot from the list and react with Selected_Slot Get_r message (introduces state->pending)
 * - Set
 *      - Slot_Allocation -> allocate the slot and react with Set_r (removes state)
 *      - Slot_Removal -> remove the slot and react with Set_r
 * - Set_r
 *      - Reaction to a Slot_Removal initiated by me
 * - Stats
 *      - Update the usage of a slot
 *      - Pass everything to subclass for updating scores table
 */
void SlotSelectionAIModule::process_message(Internal &m)
{
    switch(m.attribute){
        case MESSAGE_GET:
            //Can be a get selected or get proposed
            process_get(m);
            break;
        case MESSAGE_SET:
            //Can be a set message to allocate or remove a slot
            process_set(m);
            break;
        case MESSAGE_SETR:
            //Can be a response to my set remove message
            process_set_response(m);
            break;
        case MESSAGE_STATS:
            //Statistics to be handled
            process_stats(m);
            break;
        default:
            //Nothing to do with these types of messages
            return;
    }
}

void SlotSelectionAIModule::process_get(const Internal &m)
{
    Get get = m.get;
    //PROPOSED_SLOTS request message
    if(get.attribute == PROPOSED_SLOTS){
        Proposed_Slots_Request req = get.slot_request;
        uint64_t mac = req.mac;
        //Check if we still have pending TX proposal for this MAC. If so: ignore this message
        if(m_pending_tx_proposals.find(mac) == m_pending_tx_proposals.end()){
            uint64_t mac = req.mac;
            uint32_t num_slots = req.num_slots;
            NS_LOG_LOGIC("Received request for " << num_slots << " slot proposals for TX to mac " << mac);

            //Gather the proposed slots according to implementation
            std::vector<Slot> proposal = propose_tx_slots(mac, num_slots);
            //Send the proposal to MAC
            propose_slots(mac, proposal, m.transaction_index);
        }else{
            //Logger::log_trace<SlotSelectionAIModule>("Received a slot request for mac {} but still got a pending proposal, so ignored", mac);
        }
    //SELECTED_SLOT request message
    }else if(get.attribute == SELECTED_SLOT){
        Proposed_Slots slots = get.proposed_slots;
        select_slot(m.transaction_index, slots);
    }
}

void SlotSelectionAIModule::propose_slots(uint64_t dst_mac, std::vector<Slot>& proposal, uint64_t request_idx)
{
    uint64_t now = clock_get_time_ns();
    //Set all the slots to proposed state for the specified mac
    std::vector<Slot>::iterator it = proposal.begin();
    NS_LOG_LOGIC("Received list with " << proposal.size() << " proposals");
    while(it!=proposal.end()){
        Slot slot = *it;
        NS_LOG_LOGIC("Will set slot " << (uint16_t)slot.timeslot_num << "-" << (uint16_t)slot.frequencyslot_num << " to PROPOSED_TX state");
        if(!m_slotgrid->setSlotPending(slot.timeslot_num, slot.frequencyslot_num, SlotState::PROPOSED_TX, dst_mac)){
            NS_LOG_ERROR("ERROR: Tried an invalid TX slot proposal, removed slot. SHOULD NOT HAPPEN!!!");
            proposal.erase(it++);
        }else{
            ++it;
        }
    }
    //If there were slots proposed, add the proposal to the pending table
    if(!proposal.empty()) {
        m_pending_tx_proposals[dst_mac] = now;
    }

    //Send the response back to MAC
    Internal message;
    message.module = MODULE_MAC;
    message.attribute = MESSAGE_GETR;
    message.getr.attribute = PROPOSED_SLOTS;

    Proposed_Slots prop;
    prop.mac = dst_mac;

    for(Slot slot : proposal){
        MFTDMA_Slot sl;
        sl.frequency_num = slot.frequencyslot_num;
        sl.timeslot_num = slot.timeslot_num;
        prop.slots.push_back(sl);
    }

    message.getr.proposed_slots = prop;

    message.transaction_index = m_device->MacAIMessageIndexUpdate(request_idx);
    m_device->SendMacAIMessage(message);
    NS_LOG_LOGIC("Proposed " << prop.slots.size() << " TX slots for mac " << dst_mac);
}

void SlotSelectionAIModule::initiate_slot_allocation(uint64_t dst_mac, std::vector<Slot>& proposal)
{
    NS_LOG_DEBUG("AI will initiate TX slot allocation for destination mac " << dst_mac);
    propose_slots(dst_mac, proposal);
}

void SlotSelectionAIModule::select_slot(uint64_t request_idx, const Proposed_Slots& proposal)
{
    uint64_t mac = proposal.mac;
    //Get all the proposed slots from the message and push them in a vector
    std::vector<Slot> prop;
    uint32_t count = proposal.slots.size();
    for(uint32_t i=0; i<count; i++){
        Slot slot;
        MFTDMA_Slot sl = proposal.slots[i];
        slot.timeslot_num = sl.timeslot_num;
        slot.frequencyslot_num = sl.frequency_num;
        prop.push_back(slot);
    }
    NS_LOG_LOGIC("Received " << prop.size() << " proposed RX slots for mac " << mac);
    uint64_t now = clock_get_time_ns();
    //Select a slot from the list according to the implementation
    Slot selected = select_rx_slot(mac, prop);
    //Set the slot to proposed state
    if(selected.timeslot_num!=255 || selected.frequencyslot_num!=255) {
        if (!m_slotgrid->setSlotPending(selected.timeslot_num, selected.frequencyslot_num, SlotState::PROPOSED_RX,
                                        mac)) {
            NS_LOG_ERROR("ERROR: Tried an invalid RX slot selection, aborted. SHOULD NOT HAPPEN!!!");
            selected.timeslot_num = 255;
            selected.frequencyslot_num = 255;
        }else {
            //Add the pending proposal transaction
            m_pending_rx_proposals[mac] = now;
        }
    }else{
        NS_LOG_LOGIC("Non of the proposed RX slots worked for me");
    }

    //Send the response back to MAC
    Internal message;
    message.module = MODULE_MAC;
    message.attribute = MESSAGE_GETR;
    message.getr.attribute = SELECTED_SLOT;

    Selected_Slot sel;
    sel.mac = mac;

    MFTDMA_Slot sl;
    sl.frequency_num = selected.frequencyslot_num;
    sl.timeslot_num = selected.timeslot_num;

    sel.slot = sl;

    message.getr.selected_slot = sel;

    message.transaction_index = m_device->MacAIMessageIndexUpdate(request_idx);
    m_device->SendMacAIMessage(message);
    NS_LOG_LOGIC("Selected RX slot " << (uint16_t)selected.timeslot_num << "-" << (uint16_t)selected.frequencyslot_num << " for mac " << mac);
}

void SlotSelectionAIModule::process_set(const Internal &m)
{
    Set set = m.set;
    SLOT_ATTRIBUTE attribute = set.attribute;
    if(attribute == SLOT_ALLOCATION){
        Slot_Allocation alloc = set.slot_allocation;
        allocate_slot(m.transaction_index, alloc);
    }else if(attribute == SLOT_REMOVAL){
        Slot_Removal rem = set.slot_removal;
        remove_slot(m.transaction_index, rem);
    }else if(attribute == EXTERNAL_SLOT_ALLOCATION){
        External_Slot_Allocation alloc = set.external_slot_allocation;
        process_external_slot_allocated(alloc);
    }else if(attribute == EXTERNAL_SLOT_REMOVAL){
        External_Slot_Removal rem = set.external_slot_removal;
        process_external_slot_removed(rem);
    }else if(attribute == BC_SLOTS_ANNOUNCEMENT){
        BC_Slots_Announcement ann = set.bc_slots_announcement;
        allocate_reserved_slots(m.transaction_index, ann);
    }
}

void SlotSelectionAIModule::allocate_reserved_slots(uint64_t request_id, const BC_Slots_Announcement &ann)
{
    uint32_t count = ann.slots.size();
    NS_LOG_DEBUG("Received an announcement of " << count << " reserved slots");
    for(uint32_t i=0; i<count; i++){
        MFTDMA_Slot sl = ann.slots[i];
        if(!m_slotgrid->setSlotReserved(sl.timeslot_num, sl.frequency_num)){
            NS_LOG_ERROR("ERROR: tried to set slot " << (uint16_t)sl.timeslot_num << "-" << (uint16_t)sl.frequency_num << " to RESERVED state but timeslot was already occupied. Executed anyway. SHOULD NOT HAPPEN!");
        }else{
            NS_LOG_INFO("Slot " << (uint16_t)sl.timeslot_num << "-" << (uint16_t)sl.frequency_num << " was set to RESERVED state");
        }
    }
    //Return a Set_r message. Always OK for now
    Internal message;
    message.module = MODULE_MAC;
    message.attribute = MESSAGE_SETR;
    message.setr.result = TRANSACTION_RESULT::OK;

    message.transaction_index = m_device->MacAIMessageIndexUpdate(request_id);
    m_device->SendMacAIMessage(message);
}

void SlotSelectionAIModule::allocate_slot(uint64_t request_idx, const Slot_Allocation& allocation)
{
    uint64_t mac = allocation.mac;
    MFTDMA_Slot slot = allocation.slot;
    uint8_t t = slot.timeslot_num;
    uint8_t f = slot.frequency_num;

    SlotState state;
    if(allocation.mode == SLOT_MODE::RX){
        state = SlotState::RX;
    }else if(allocation.mode == SLOT_MODE::TX){
        state = SlotState::TX;
    }else{
        NS_LOG_ERROR("ERROR: Received an invalid SLOT_MODE in my allocate_slot call. SHOULD NOT HAPPEN");
        return;
    }

    bool success;
    //First check if this is a slot that is already allocated with the exact same configuration. If so, I just reply OK
    if(m_slotgrid->isSlotAllocatedFor(t, f, mac) && m_slotgrid->getSlotInfo(t, f).state==state) {
        //This is the exact same slot so its a duplicate. I will just reply ok, slot is already allocated
        NS_LOG_DEBUG("Had to allocate slot " << t << "-" << f << " for mac " << mac << ", but was already allocated, possibly a retransmission");
        success=true;
        //Else, check if we have a pending proposal here
    }else if((allocation.mode == SLOT_MODE::TX && m_pending_tx_proposals.find(mac) == m_pending_tx_proposals.end())
       || (allocation.mode == SLOT_MODE::RX && m_pending_rx_proposals.find(mac) == m_pending_rx_proposals.end())) {
        //If no pending proposal: do not allocate
        success=false;
    }else{
        //If there is a pending proposal, try to allocate. This will fail when it is still an old proposal
        success = m_slotgrid->setSlotAllocated(t, f, state, mac);
    }
    if(success){
        NS_LOG_INFO("Allocated slot " << (uint16_t)t << "-" << (uint16_t)f << " for mac " << mac);
    }else{
        NS_LOG_DEBUG("WARNING: Tried to allocate slot " << (uint16_t)t << "-" << (uint16_t)f << " that was not proposed, the proposal expired or is already finished! Not allocated");
    }

    if(success){
        if(state == SlotState::TX){
            //Remove the other TX proposals for this MAC
            m_slotgrid->abortTXProposals(mac);
            m_pending_tx_proposals.erase(mac);
        }else{
            m_pending_rx_proposals.erase(mac);
        }
        //Notify the subclass that this slot is allocated now
        notify_slot_allocated(t, f);
    }

    //Return a Set_r message
    Internal message;
    message.module = MODULE_MAC;
    message.attribute = MESSAGE_SETR;

    if(success) {
        message.setr.result = TRANSACTION_RESULT::OK;
    }else{
        message.setr.result = TRANSACTION_RESULT::NOK;
    }

    message.transaction_index = m_device->MacAIMessageIndexUpdate(request_idx);
    m_device->SendMacAIMessage(message);
    m_slotgrid->logTableState();
}

void SlotSelectionAIModule::remove_slot(uint64_t request_idx, const Slot_Removal& removal)
{
    uint64_t mac = removal.mac;
    MFTDMA_Slot slot = removal.slot;
    uint8_t t = slot.timeslot_num;
    uint8_t f = slot.frequency_num;
    bool success = false;
    if(m_slotgrid->isSlotAllocatedFor(t, f, mac)){
        //Remove the slot
        m_slotgrid->freeSlot(t, f);
        //Notify the subclass that this slot is free now
        notify_slot_removed(t, f);
        NS_LOG_INFO("Removed slot " << (uint16_t)t << "-" << (uint16_t)f << " for mac " << mac << " based on MACs request");
        success = true;
    }else{
        NS_LOG_LOGIC("WARNING: Received a remove message for slot " << (uint16_t)t << "-" << (uint16_t)f << " that was not allocated for this mac");
    }

    //Return a Set_r message
    Internal message;
    message.module = MODULE_MAC;
    message.attribute = MESSAGE_SETR;

    if(success) {
        message.setr.result = TRANSACTION_RESULT::OK;
    }else{
        message.setr.result = TRANSACTION_RESULT::NOK;
    }

    message.transaction_index = m_device->MacAIMessageIndexUpdate(request_idx);
    m_device->SendMacAIMessage(message);
    m_slotgrid->logTableState();
}

void SlotSelectionAIModule::process_set_response(const Internal &m)
{
    Set_r setr = m.setr;
    uint64_t idx = m.transaction_index;
    //Check if I have a removal transaction with this idx
    if(m_pending_removals.find(idx) != m_pending_removals.end()){
        //Search and remove the slot
        if(setr.result == TRANSACTION_RESULT::OK) {
            Slot slot = m_slotgrid->executePendingRemoval(idx);
            if(slot.timeslot_num==255 && slot.frequencyslot_num==255){
                NS_LOG_WARN("WARNING: Could not find pending removal with idx " << idx);
            }else{
                notify_slot_removed(slot.timeslot_num, slot.frequencyslot_num);
                NS_LOG_INFO("Removed slot " << (uint16_t)slot.timeslot_num << "-" << (uint16_t)slot.frequencyslot_num << " based on my request");
            }
        }else{
            NS_LOG_DEBUG("Receive Set_r message for my removal, but was not successful. Will keep slot");
        }
        //Remove the pending removal
        m_pending_removals.erase(idx);
        m_slotgrid->logTableState();
    }
}

void SlotSelectionAIModule::process_external_slot_allocated(const External_Slot_Allocation &alloc)
{
    //TODO: Create a separate field for that
    uint64_t mac=alloc.src_mac;
    MFTDMA_Slot slot = alloc.slot;
    uint8_t t = slot.timeslot_num;
    uint8_t f = slot.frequency_num;
    //Only process this slot when it is not one of my slots
    if(!m_slotgrid->isSlotAllocatedFor(t, f, mac) && !m_slotgrid->isSlotPendingFor(t, f, mac)) {
        bool new_allocation = m_external_slotgrid->setExternalSlotUsed(t, f);
        notify_external_slot_allocated(t, f, new_allocation);
        NS_LOG_LOGIC("Got an external slot allocation message for slot " << (uint16_t)t << "-" << (uint16_t)f << " from MAC " << mac);
        if (new_allocation) {
            m_external_slotgrid->logTableState();
        }
    }
}

void SlotSelectionAIModule::process_external_slot_removed(const External_Slot_Removal &rem)
{
    //TODO: Create a separate field for that
    uint64_t mac=rem.src_mac;
    MFTDMA_Slot slot = rem.slot;
    uint8_t t = slot.timeslot_num;
    uint8_t f = slot.frequency_num;
    bool change = m_external_slotgrid->setExternalSlotFree(t, f);
    if (change) {
        notify_external_slot_removed(t, f);
        NS_LOG_LOGIC("Got an external slot removal message for slot " << (uint16_t)t << "-" << (uint16_t)f << " from MAC " << mac);
        m_external_slotgrid->logTableState();
    }
}

void SlotSelectionAIModule::process_stats(const Internal &m)
{
    //Check if this is a MAC ACK statistic so I can update the time for idle detection
    Stats stats = m.stats;
    if(true){
        Mac_stat macstat = stats.mac_stat;
        if(macstat.attribute == MAC_STAT_TX){
            Mac_tx_stat txstat = macstat.tx_stat;
            int num_ackstats = txstat.ack_stat.size();
            for (int i = 0; i < num_ackstats; i++) {
                Mac_ack_stat ack_stat = txstat.ack_stat[i];
                uint8_t t = ack_stat.slot_index;
                uint8_t f = ack_stat.tx_channel;
                //Do not update usage of reserved slots
                if(!m_slotgrid->isSlotReserved(t, f)) {
                    //Update the slot usage time
                    if (m_slotgrid->updateUsage(t, f)) {
                        //Logger::log_trace<SlotSelectionAIModule>("Updated usage of TX slot {}-{}", t, f);
                    }
                }
            }
        }else if(macstat.attribute == MAC_STAT_RX){
            Mac_rx_stat rxstat = macstat.rx_stat;
            uint8_t t = rxstat.slot_index;
            uint8_t f = rxstat.tx_channel;
            //Do not update usage of reserved slots
            if(!m_slotgrid->isSlotReserved(t, f)) {
                //Update the slot usage time
                if (m_slotgrid->updateUsage(t, f)) {
                    //Logger::log_trace<SlotSelectionAIModule>("Updated usage of RX slot {}-{}", t, f);
                }
            }
        }
    }
    //Forward statistics to the subclass to update score
    handle_statistics(m);
}

}
