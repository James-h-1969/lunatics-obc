#include "Comms.h"

std::string Communication::request_state() {
    std::string response = ax25_client_.send_data(GET, "state");
    json j = json::parse(response);
    std::string state_value = j["state"];
    return state_value;
};

void Communication::send_payload_data(std::vector<float> recording, std::time_t current_time) {
    // converts std::time_t into std::string
    std::ostringstream time_stream;
    time_stream << std::put_time(std::gmtime(&current_time), "%Y-%m-%d %H:%M:%S");
    std::string utc_time = time_stream.str();

    // converts vector to std::string
    std::ostringstream data_stream;
    data_stream << "[";
    for (size_t i = 0; i < recording.size(); ++i) {
        data_stream << recording[i];
        if (i != recording.size() - 1) {
            data_stream << ", ";
        }
    }
    data_stream << "]";
    std::string data_array = data_stream.str();

    // json construction
    std::ostringstream json_stream;
    json_stream << "{"
                << "\"time_utc\": \"" << utc_time << "\", "
                << "\"data_type\": \"" << DataType::SCIENCE_DATA << "\", "
                << "\"data\": " << data_array
                << "}";

    std::string post_body = json_stream.str();

    std::string response = ax25_client_.send_data(POST, "data", post_body);
    std::cout << response << std::endl;
};

void Communication::send_attitude_data(std::vector<float> recording, std::time_t current_time){
    // converts std::time_t into std::string
    std::ostringstream time_stream;
    time_stream << std::put_time(std::gmtime(&current_time), "%Y-%m-%d %H:%M:%S");
    std::string utc_time = time_stream.str();

    // converts vector to std::string
    std::ostringstream data_stream;
    data_stream << "[";
    for (size_t i = 0; i < recording.size(); ++i) {
        data_stream << recording[i];
        if (i != recording.size() - 1) {
            data_stream << ", ";
        }
    }
    data_stream << "]";
    std::string data_array = data_stream.str();

    // json construction
    std::ostringstream json_stream;
    json_stream << "{"
                << "\"time_utc\": \"" << utc_time << "\", "
                << "\"data_type\": \"" << DataType::ATTITUDE_DATA << "\", "
                << "\"data\": " << data_array
                << "}";

    std::string post_body = json_stream.str();

    std::string response = ax25_client_.send_data(POST, "data", post_body);
    std::cout << response << std::endl;
};

void Communication::send_wod_data(struct WODRecording recording, std::time_t current_time, int unique_satellite_id) {
    // converts std::time_t into std::string
    std::ostringstream time_stream;
    time_stream << std::put_time(std::gmtime(&current_time), "%Y-%m-%d %H:%M:%S");
    std::string utc_time = time_stream.str();

    // construct json
    std::ostringstream json_stream;
    json_stream << "{";
    json_stream << "\"time_utc\": \"" << utc_time << "\", ";
    json_stream << "\"satellite_id\": " << unique_satellite_id << ", ";
    json_stream << "\"data\": {";
    json_stream << "\"batt_voltage\": " << recording.battery_bus_voltage << ", ";
    json_stream << "\"batt_current\": " << recording.battery_bus_current << ", ";
    json_stream << "\"current_3v3_bus\": " << recording.reg_3v3_bus_current << ", ";
    json_stream << "\"current_5v_bus\": " << recording.reg_5v_bus_current << ", ";
    json_stream << "\"temp_comm\": " << recording.comms_subsystem_temp << ", ";
    json_stream << "\"temp_EPS\": " << recording.eps_subsystem_temp << ", ";
    json_stream << "\"mode\": " << static_cast<int>(recording.current_state) << ", ";
    json_stream << "\"temp_battery\": " << recording.battery_temp;
    json_stream << "}, "; 

    json_stream << "\"data_type\": \"" << DataType::WOD_DATA;
    json_stream << "}";

    std::string post_body = json_stream.str();

    std::string response = ax25_client_.send_data(POST, "data", post_body);
    std::cout << "WOD has been downlinked successfully." << std::endl;
};

void Communication::request_if_reset_required(Payload& payload) {
    std::string response = ax25_client_.send_data(GET, "to_reset");
    json j = json::parse(response);

    bool to_reset = j["to_reset"];
    if (!to_reset) {
        return;
    }

    std::string reset_time_str = j["reset_time"];
    std::tm tm_time = {};
    strptime(reset_time_str.c_str(), "%Y-%m-%dT%H:%M", &tm_time);  
    std::time_t reset_time = timegm(&tm_time);

    std::cout << "Deleting any payload data before " << reset_time << std::endl;

    payload.remove_before_time(reset_time);
}