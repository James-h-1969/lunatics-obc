#include "Payload.h"

void Payload::take_reading(std::time_t current_time) {
    if (payload_storage_.size() > MAX_RECORDINGS) {
        std::cerr << "Max payload recordings have been taken. You will not be able to downlink any more" << std::endl;
        return;
    }

    std::vector<float> reading(18);

    sensor_.takeMeasurements();
    
    // Fill readings in the specified order
    reading[0]  = sensor_.getCalibratedA();
    reading[1]  = sensor_.getCalibratedB();
    reading[2]  = sensor_.getCalibratedC();
    reading[3]  = sensor_.getCalibratedD();
    reading[4]  = sensor_.getCalibratedE();
    reading[5]  = sensor_.getCalibratedF();
    reading[6]  = sensor_.getCalibratedG();
    reading[7]  = sensor_.getCalibratedH();
    reading[8]  = sensor_.getCalibratedR();
    reading[9]  = sensor_.getCalibratedI();
    reading[10] = sensor_.getCalibratedS();
    reading[11] = sensor_.getCalibratedJ();
    reading[12] = sensor_.getCalibratedT();
    reading[13] = sensor_.getCalibratedU();
    reading[14] = sensor_.getCalibratedV();
    reading[15] = sensor_.getCalibratedW();
    reading[16] = sensor_.getCalibratedK();
    reading[17] = sensor_.getCalibratedL();

    struct PayloadReading curr_payload_reading;
    curr_payload_reading.readings = reading;
    curr_payload_reading.reading_time = current_time;
    payload_storage_.push_back(curr_payload_reading);
};

std::vector<struct PayloadReading> Payload::get_payload_storage() {
    std::vector<struct PayloadReading> storage = payload_storage_;
    return storage;
};

void Payload::remove_before_time(std::time_t time_input) {
    payload_storage_.erase(
        std::remove_if(
            payload_storage_.begin(),
            payload_storage_.end(),
            [time_input](const PayloadReading& reading) {
                return reading.reading_time < time_input;
            }
        ),
        payload_storage_.end()
    );
};

void Payload::delete_all_payload_readings(){
    payload_storage_.clear();
}