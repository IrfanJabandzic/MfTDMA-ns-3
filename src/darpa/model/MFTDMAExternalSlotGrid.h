//
// Created by Maxim Claeys on 30/10/2017.
//

#ifndef MFTDMAEXTERNALSLOTGRID_H
#define MFTDMAEXTERNALSLOTGRID_H

#include <cstdint>
#include <vector>
#include "MFTDMASlotGrid.h"
#include "ns3/object.h"

namespace ns3 {

class SlotSelectionAIModule;

enum class ExternalSlotState {FREE=0, USED=1};

struct ExternalSlotInfo {
    ExternalSlotInfo() : state(ExternalSlotState::FREE), last_announcement(0){};
    ExternalSlotState state;
    uint64_t last_announcement;
};

class MFTDMAExternalSlotGrid : public Object {
    friend class SlotSelectionAIModule;
private:
    uint8_t m_num_timeslots;
    uint8_t m_num_frequencyslots;
    ExternalSlotInfo **m_slot_info;

protected:
    /**
     * Sets the specified slot to being used
     * @param timeslot_num Timeslot number
     * @param frequencyslot_num Frequencyslot number
     * @return True if state was changed (i.e. when slot was free before)
     */
    bool setExternalSlotUsed(uint8_t timeslot, uint8_t frequencyslot);

    /**
     * Sets the specified slot to being free
     * @param timeslot_num Timeslot number
     * @param frequencyslot_num Frequencyslot number
     * @return True if state was changed (i.e. when slot was used before)
     */
    bool setExternalSlotFree(uint8_t timeslot, uint8_t frequencyslot);

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
     * Checks if the specified slot is still free
     * @param timeslot_num Timeslot number
     * @param frequencyslot_num Frequencyslot number
     * @return
     */
    bool isSlotFree(uint8_t timeslot, uint8_t frequencyslot) const;

    /**
     * Checks all the slots for idleness. If there was no broadcast info during the timeout period, the slot is assumed to be free.
     * @param timeout_ns
     */
    std::vector<Slot> getIdleSlots(uint32_t timeout_ms) const;

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
    MFTDMAExternalSlotGrid();
    void MFTDMAExternalSlotGridInit(uint8_t num_timeslots, uint8_t num_frequencyslots);
    virtual ~MFTDMAExternalSlotGrid();
};

}
#endif //MFTDMAEXTERNALSLOTGRID_H
