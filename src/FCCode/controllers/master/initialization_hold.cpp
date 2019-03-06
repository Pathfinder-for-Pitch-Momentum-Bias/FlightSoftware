#include "master_helpers.hpp"

/**
 * @brief Puts the satellite into the initialization hold mode.
 * 
 * @param reason The failure code that's responsible for entering initialization hold mode.
 */
void Master::initialization_hold(unsigned short int reason) {
    debug_println("Entering initialization hold mode...");
    rwMtxWLock(&State::Master::master_state_lock);
        // The two state declarations below don't do anything; they're just for cosmetics/maintaining invariants
        State::Master::master_state = State::Master::MasterState::INITIALIZATION_HOLD;
        State::Master::pan_state = State::Master::PANState::MASTER_INITIALIZATIONHOLD;
    rwMtxWUnlock(&State::Master::master_state_lock);

    chMtxLock(&eeprom_lock);
        EEPROM.put(EEPROM_ADDRESSES::INITIALIZATION_HOLD_FLAG, true);
    chMtxUnlock(&eeprom_lock);

    if (State::ADCS::angular_rate() >= Constants::ADCS::MAX_STABLE_ANGULAR_RATE) {
        if (State::Hardware::can_get_data(Devices::adcs_system)) {
            rwMtxWLock(&State::ADCS::adcs_state_lock);
                State::ADCS::adcs_state = State::ADCS::ADCSState::ADCS_DETUMBLE;
            rwMtxWUnlock(&State::ADCS::adcs_state_lock);
            chThdEnqueueTimeoutS(&RTOSTasks::adcs_detumbled, S2ST(Constants::Master::INITIALIZATION_HOLD_DETUMBLE_WAIT)); // Wait for detumble to finish.
        }
    }
    // Quake controller will send SOS packet. Control is now handed over to
    // master_loop(), which will continuously check Quake uplink for manual control/mode shift commands
}