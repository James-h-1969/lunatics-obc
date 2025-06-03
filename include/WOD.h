# ifndef WOD_H
# define WOD_H

#include <deque>
#include <vector>
#include <cstddef>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "State.h"


struct WODRecording {
    std::time_t recording_time;
    enum State current_state;
    float battery_bus_voltage;
    float battery_bus_current;
    float reg_3v3_bus_current;
    float reg_5v_bus_current;
    float comms_subsystem_temp;
    float eps_subsystem_temp;
    float battery_temp;
};

class WOD {
    public:
        WOD(): max_size_(32) {};
        std::vector<struct WODRecording> get_current_storage();
        void take_WOD_recording_and_log(std::time_t current_time,  enum State current_state);
        size_t max_size_;
    private:
        std::deque<struct WODRecording> current_storage_;
        void add_WOD_recording(struct WODRecording recording);
};

# endif