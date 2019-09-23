//
// Created by Maxim Claeys on 30/10/2017.
//

#ifndef MFTDMASLOTGRID_H
#define MFTDMASLOTGRID_H

#include <cstdint>
#include <vector>
#include "ns3/object.h"
#include "helpers.h"

namespace ns3 {

#define USE_RX_TX_SAME_TIMESLOT

class SlotSelectionAIModule;

enum class SlotState {FREE=0, RESERVED=1, RX=2, TX=3, PROPOSED_TX=4, PROPOSED_RX=5};

struct SlotInfo {
    SlotInfo() : target_mac(0), state(SlotState::FREE), last_used(0), pending_removal(0){};
    uint64_t target_mac;
    SlotState state;
    uint64_t last_used;
    uint64_t pending_removal;
};

struct Slot {
    Slot() : timeslot_num(255), frequencyslot_num(255){};
    Slot(uint8_t t, uint8_t f) : timeslot_num(t), frequencyslot_num(f){};
    uint8_t timeslot_num;
    uint8_t frequencyslot_num;
    inline bool operator <( const Slot &rhs ) const
    {
        if(timeslot_num==rhs.timeslot_num){
            return frequencyslot_num < rhs.frequencyslot_num;
        }else{
            return timeslot_num < rhs.timeslot_num;
        }
    }
};

class MFTDMASlotGrid : public Object {
    friend class SlotSelectionAIModule;
private:
    uint8_t m_num_timeslots;
    uint8_t m_num_frequencyslots;
    SlotInfo **m_slot_info;

    //Helper function
    bool isSetPendingAllowed(uint8_t timeslot, uint8_t frequencyslot, SlotState state, uint64_t mac) const;
protected:
    //These are protected, should only be accessible by the friends (i.e. SlotSelectionAIModule)

    /**
     * Resets the slot to FREE state
     * @param timeslot Timeslot number
     * @param frequencyslot Frequencyslot number
     */
    void freeSlot(uint8_t timeslot, uint8_t frequencyslot);

    /**
     * Sets the slot to pending state.
     * Only accepts states PROPOSED_RX and PROPOSED_TX
     * @param timeslot Timeslot number
     * @param frequencyslot Frequencyslot number
     * @param state Slot state (PROPOSED_RX or PROPOSED_RX)
     * @param mac Target mac address
     * @return True if set succeeded (valid state, entire timeslot was free in case of RX or all slots in timeslot were free or PROPOSED_TX for same MAC in case of TX)
     */
    bool setSlotPending(uint8_t timeslot, uint8_t frequencyslot, SlotState state, uint64_t mac);

    /**
     * Sets the specified slot to allocated mode.
     * Only accepts states RX and TX
     * @param timeslot Timeslot number
     * @param frequencyslot Frequencyslot number
     * @param state Slot state (RX or TX)
     * @param mac Target mac address
     * @return True if set succeeded (mac address matches and PROPOSED_TX->TX or PROPOSED_RX->RX)
     */
    bool setSlotAllocated(uint8_t timeslot, uint8_t frequencyslot, SlotState state, uint64_t mac);

    /**
     * Sets the specified slot to reserved state.
     * The state update will always be executed, but FALSE will be returned in case the timeslotslot was not free.
     * In theory this should not happen as slot reservations should be done at initialization.
     * @param timeslot Timeslot number
     * @param frequencyslot Frequencyslot number
     * @return False if the slot was not free
     */
    bool setSlotReserved(uint8_t timeslot, uint8_t frequencyslot);

    /**
     * Set the pending removal transaction for this slot
     * @param timeslot Timeslot number
     * @param frequencyslot Frequencyslot number
     * @param id Transaction ID of removal
     * @return True if set succeeded (in case slot was allocated (i.e. either in RX or TX state))
     */
    bool setPendingRemoval(uint8_t timeslot, uint8_t frequencyslot, uint64_t id);

    /**
     * Updates the last_used field for the specified timeslot to the current time
     * @param timeslot Timeslot number
     * @param frequencyslot Frequencyslot number
     * @return True if update succeeded (in case slot was allocate (i.e. either in RX or TX state))
     */
    bool updateUsage(uint8_t timeslot, uint8_t frequencyslot);

    /**
     * Search for the slot with the specified pending removal and remove the pending removal
     * @param id Removal transaction ID to cancel
     */
    void abortPendingRemoval(uint64_t id);

    /**
     * Search for the slot with the specified pending removal and remove the slot (set to FREE state)
     * @param id Removal transaction ID to execute
     * @return The timeslot that got freed (or 255,255 in case id was not found)
     */
    Slot executePendingRemoval(uint64_t id);

    /**
     * Searches for the PROPOSED_RX state with specified mac and sets the slots back to free
     * @param mac Target mac address
     */
    void abortRXProposal(uint64_t mac);

    /**
     * Searches for all PROPOSED_TX states with specified mac and sets the slots back to free
     * @param mac Target mac address
     */
    void abortTXProposals(uint64_t mac);
public:
    /**
     * Gives the number of timeslots
     * @return Number of timeslots
     */
    uint8_t getNumTimeslots() const;

    /**
     * Gives the number of frequency slots
     * @return Number of frequencyslots
     */
    uint8_t getNumFrequencyslots() const;

    /**
     * Gives the slot information about the specified slot
     * @param timeslot Timeslot number
     * @param frequencyslot Frequencyslot number
     * @return SlotInfo about the specified slot
     */
    const SlotInfo& getSlotInfo(uint8_t timeslot, uint8_t frequencyslot) const;

    /**
     * Checks if a TX slot can be allocated in this timeslot row (i.e. no (PROPOSED_)TX slots or RESERVED slots in entire row)
     * @param timeslot Specified timeslot number
     * @return Availability of timeslot for TX
     */
    bool isTimeslotAvailableTX(uint8_t timeslot) const;

    /**
     * Checks if a RX slot can be allocated in this timeslot row (i.e. no (PROPOSED_)RX slots or RESERVED slots in entire row)
     * @param timeslot Specified timeslot number
     * @return Availability of timeslot for RX
     */
    bool isTimeslotAvailableRX(uint8_t timeslot) const;

    /**
     * Checks if the slot is available for TX: the timeslot is available for TX and the slot itself is still free
     * @param timeslot Timeslot number
     * @param frequencyslot Frequency number
     * @return Slot availability for TX
     */
    bool isSlotAvailableTX(uint8_t timeslot, uint8_t frequencyslot) const;

    /**
     * Checks if the slot is available for RX: the timeslot is available for RX and the slot itself is still free
     * @param timeslot Timeslot number
     * @param frequencyslot Frequency number
     * @return Slot availability for RX
     */
    bool isSlotAvailableRX(uint8_t timeslot, uint8_t frequencyslot) const;

    /**
     * Checks if the specified slot is occupied (i.e. not in FREE state)
     * A slot is occupied as soon as it is in RX, TX, PROPOSED_RX or PROPOSED_TX state
     * @param timeslot Timeslot number
     * @param frequencyslot Frequencyslot number
     * @return Availability of the slot
     */
    bool isSlotFree(uint8_t timeslot, uint8_t frequencyslot) const;

    /**
     * Checks wether the entire timeslot is in FREE state
     * @param timeslot Timeslot number
     * @return Is entire timeslot free
     */
    bool isTimeslotFree(uint8_t timeslot) const;

    /**
     * Gives a list of all allocated slots (state RX or TX) of which the last_used field was more than the specified timeout ago.
     * IMPORTANT: Only slots without a pending removal are returned
     * @param idle_timeout_ms Idle timeout in milliseconds
     * @return List of idle slots
     */
    std::vector<Slot> getIdleSlots(uint32_t idle_timeout_ms, float rx_multiplier) const;

    /**
     * Checks if the slot is allocated: i.e. either in RX or TX mode (as opposed to isSlotOccupied which also takes into account proposed states)
     * @param timeslot Timeslot number
     * @param frequencyslot Frequencyslot number
     * @return Allocation status of the slot
     */
    bool isSlotAllocated(uint8_t timeslot, uint8_t frequencyslot) const;

    /**
     * Checks if the slot is allocated for the specified target mac: i.e. either in RX or TX mode with target_mac==mac
     * @param timeslot Timeslot number
     * @param frequencyslot Frequencyslot number
     * @param mac Target mac address
     * @return Allocation status of the slot
     */
    bool isSlotAllocatedFor(uint8_t timeslot, uint8_t frequencyslot, uint64_t mac) const;
    bool isSlotAllocatedForExposed(uint8_t timeslot, uint8_t frequencyslot, uint64_t mac, SLOT_MODE mode) const;

    /**
     * Checks if the slot is pending allocation for the specified target mac: i.e. either in PROPOSED_RX or PROPOSED_TX mode with target_mac==mac
     * @param timeslot Timeslot number
     * @param frequencyslot Frequencyslot number
     * @param mac Target mac address
     * @return Allocation status of the slot
     */
    bool isSlotPendingFor(uint8_t timeslot, uint8_t frequencyslot, uint64_t mac) const;
    bool isSlotPendingForExposed(uint8_t timeslot, uint8_t frequencyslot, uint64_t mac, SLOT_MODE mode) const;

    /**
     * Checks if the slot is a special reserved slot (e.g. broadcast or control)
     * @param timeslot Timeslot number
     * @param frequencyslot Frequencyslot number
     * @return True if slot state is RESERVED
     */
    bool isSlotReserved(uint8_t timeslot, uint8_t frequencyslot) const;

    /**
     * Checks if there is a pending removal on this slot
     * @param timeslot Timeslot number
     * @param frequencyslot Frequency slot number
     * @return True if a removal is pending
     */
    bool isRemovePending(uint8_t timeslot, uint8_t frequencyslot) const;

    /**
     * Logs the current table state using debug
     */
    void logTableState() const;

    /**
     * Prints the current slot usage count
     */
    void printUsage() const;

    /**
     * \brief Get the type ID.
     * \return the object TypeId
     */
    static TypeId GetTypeId (void);
    MFTDMASlotGrid();
    void MFTDMASlotGridInit(uint8_t num_timeslots, uint8_t num_frequencyslots);
    virtual ~MFTDMASlotGrid();
};

}

#endif //MFTDMASLOTGRID_H
