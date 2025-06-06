#include "Comms.h"

std::string Communication::request_state() {
    std::string response = ax25_client_.send_data(GET, "state");
    if (response.empty()){
        return "INIT";
    }
    json j = json::parse(response);
    if (!ax25_client_.check_valid_ax25_header(j)) {
        return "INIT";
    }
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
                << "\"data_type\": " << SCIENCE_DATA << ", "
                << "\"data\": " << data_array << ", "
                << "\"source_address\": \"" << source_address << "\", "
                << "\"destination_address\": \"" << destination_address << "\""
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
                << "\"data\": " << data_array << "\", "
                << "\"source_address\": " << source_address << "\", "
                << "\"destination_address\": " << destination_address << "\", "
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
    nlohmann::json payload = {
        {"time_utc", utc_time},
        {"satellite_id", unique_satellite_id},
        {"data", {
            {"batt_voltage", recording.battery_bus_voltage},
            {"batt_current", recording.battery_bus_current},
            {"current_3v3_bus", recording.reg_3v3_bus_current},
            {"current_5v_bus", recording.reg_5v_bus_current},
            {"temp_comm", recording.comms_subsystem_temp},
            {"temp_EPS", recording.eps_subsystem_temp},
            {"mode", static_cast<int>(recording.current_state)},
            {"temp_battery", recording.battery_temp}
        }},
        {"source_address", source_address},
        {"destination_address", destination_address},
        {"data_type", WOD_DATA}
    };

    std::string json_text = payload.dump();

    std::string post_body = json_text;

    std::string response = ax25_client_.send_data(POST, "data", post_body);
    std::cout << "WOD has been downlinked successfully." << std::endl;
};

void Communication::request_if_reset_required(Payload& payload) {
    std::string response = ax25_client_.send_data(GET, "to_reset");
    if (response.empty()){
        return;
    }
    json j = json::parse(response);

    if (!ax25_client_.check_valid_ax25_header(j)) {
        return;
    }

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