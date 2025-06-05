#include "FSM.h"
#include "GPIO.h"
#include "Comms.h"
#include "Payload.h"
#include "RTC.h"
#include "WOD.h"
#include "Attitude.h"

/*
Change the following parameters depending on requirements. i felt like 5s polling was reasonable but 
will test to make sure it is smooth. 
*/

# define UNIQUE_LUNATICS_ID 0x125D // 4701 in hex, req as per S-TTC-02
# define STATE_POLLING_TIME 1 // (s) - time in between checking the state
# define WOD_SENDING_TIMEFRAME 30 // (s) - time in between sending the WOD as per req S-TTC-02

int main() {
    // initialise interface
    GPIOControl gpio = GPIOControl();
    RaspberryPiI2C i2c(1);

    // create interface
    AS7265X sensor(&i2c);
        
    // create all the objects
    FSM fsm = FSM();
    AX25 ax25_client = AX25(gpio);
    Communication comms = Communication(ax25_client);
    RTC rtc = RTC(comms);
    WOD wod = WOD();
    Attitude attitude = Attitude();
    Payload payload = Payload(sensor);

     // setup the loop
    enum State current_state = fsm.poll_state(comms, gpio, attitude);
    std::time_t start_time = rtc.get_time(); // handles sending WOD every 32 minutes
    std::time_t interval_time = start_time; // handles recording WOD every minute
    std::time_t current_time = start_time; 
    std::time_t poller_time = start_time; // handles polling the state every 5s

    printf("Beginning the loop...\n");
    wod.take_WOD_recording_and_log(current_time, current_state);
    comms.request_if_reset_required(payload);

    while (true) {
        current_time = rtc.get_time();
        double seconds_between_poll = std::difftime(current_time, poller_time);

        if (seconds_between_poll > STATE_POLLING_TIME) {
            std::cout << "Polling the state and reset!" << std::endl;
            current_state = fsm.poll_state(comms, gpio, attitude);
            comms.request_if_reset_required(payload);
            poller_time = current_time;

            payload.take_reading(current_time); // take a reading every 5s

            if (current_state == PAYLOAD) { // downlink all of the payload data
                std::vector<struct PayloadReading> recordings = payload.get_payload_storage();
                for (struct PayloadReading recording: recordings){
                    comms.send_payload_data(recording.readings, recording.reading_time);
                }
            }

            // comms.send_attitude_data(attitude.take_attitude_recording(), current_time);
        }

        // take a WOD recording every 60s 
        double seconds_between_interval = std::difftime(current_time, interval_time);
        if (seconds_between_interval >= SECONDS_IN_A_MINUTE) {
            wod.take_WOD_recording_and_log(current_time, current_state);
            interval_time = current_time;
        }

        // send WOD every 30s
        double seconds_between_start = std::difftime(current_time, start_time);
        if (seconds_between_start >= WOD_SENDING_TIMEFRAME) {
            comms.send_wod_data(wod.get_current_storage().back(), current_time, UNIQUE_LUNATICS_ID);
            start_time = current_time;
        } 
    }


    return 0;
}