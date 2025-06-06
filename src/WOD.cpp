#include "WOD.h"

std::vector<struct WODRecording> WOD::get_current_storage(){
    std::vector<struct WODRecording> vector(current_storage_.begin(), current_storage_.end());
    return vector;
};

// random float generator
float random_float(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void WOD::take_WOD_recording_and_log(std::time_t current_time, enum State current_state, Payload payload) {
    // this takes a recording and pushes to storage, ensuring only 32 recordings are stored.
    struct WODRecording recording;
    uint8_t temp = payload.get_payload_temp();

    // TODO. replace this with actual values for these
    recording.recording_time = current_time;
    recording.current_state = current_state;
    recording.battery_bus_voltage    = 3.9; 
    recording.battery_bus_current    = random_float(0.1f, 0.2f); // have to simulate these due to broken sensors
    recording.reg_3v3_bus_current    = random_float(0.1f, 0.2f);
    recording.reg_5v_bus_current     = random_float(0.1f, 0.2f);
    recording.comms_subsystem_temp   = temp-0.234; // calculated from simulations
    recording.eps_subsystem_temp     = temp;
    recording.battery_temp           = temp+0.214;

    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << "\n";
    std::cout << "WOD recording created:\n";
    std::cout << "Time: " << recording.recording_time << "\n";
    std::cout << "State: " << recording.current_state << "\n";
    std::cout << "Battery Voltage: " << recording.battery_bus_voltage << " V\n";
    std::cout << "Battery Current: " << recording.battery_bus_current << " A\n";
    std::cout << "3.3V Bus Current: " << recording.reg_3v3_bus_current << " A\n";
    std::cout << "5V Bus Current: " << recording.reg_5v_bus_current << " A\n";
    std::cout << "Comms Temp: " << recording.comms_subsystem_temp << " degC\n";
    std::cout << "EPS Temp: " << recording.eps_subsystem_temp << " degC\n";
    std::cout << "Battery Temp: " << recording.battery_temp << " degC\n";
    std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << "\n";
    
    if (current_storage_.size() > this->max_size_) {
        current_storage_.pop_front();
    }
    current_storage_.push_back(recording);
};