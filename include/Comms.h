# ifndef COMMS_H
# define COMMS_H

#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <iostream>
#include <nlohmann/json.hpp>

#include "WOD.h"
#include "Payload.h"
#include "AX25.h"

using json = nlohmann::json;

enum DataType {
    PAYLOAD_DATA,
    WOD_DATA,
    ATTITUDE_DATA,
};

class Communication {
    /*
    This class holds the functions in order to send messages using wifi between our ground station and 
    the satellite.
    */
    public:
        std::string request_state();
        void request_if_reset_required(Payload& payload);
        void send_payload_data(std::vector<float> recording, std::time_t current_time);
        void send_attitude_data(std::vector<float> recording, std::time_t current_time);
        void send_wod_data(struct WODRecording recording, std::time_t current_time, int unique_satellite_id);
    private:
        AX25 ax25_client_;
};


# endif