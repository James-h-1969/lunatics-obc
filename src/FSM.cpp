#include "FSM.h"

enum State FSM::string_state_to_enum(std::string state) {
    if (state == "INIT") return INIT;
    if (state == "NOMINAL") return NOMINAL;
    if (state == "POWER_OFF") return POWER_OFF; 
    if (state == "PAYLOAD") return PAYLOAD;
    if (state == "DETUMBLING") return DETUMBLING;
    return NULL_STATE;
};

void FSM::show_state_on_leds(enum State state, GPIOControl gpio) {
    std::bitset<3> binary(state);
    std::string binary_string = binary.to_string();
    std::cout << "Setting the LEDs to: " << binary_string << std::endl; 
    int counter = 0;
    for (char letter:binary_string) {
        int pin = -1;
        switch(counter) {
            case 0:
                pin = 19;
                break;
            case 1:
                pin = 20;
                break;
            case 2:
                pin = 21;
                break;
            default:
                pin = -1;
                break;
        }
        if (letter == '0') {
            gpio.setGPIOState(pin, "LOW");
        } else {
            gpio.setGPIOState(pin, "HIGH");
        }
        std::cout << "Setting pin " << pin << " to " << letter << std::endl;
        counter++;
    }

};

State FSM::poll_state(Communication& comms, GPIOControl gpio, Attitude att) {
    std::string polled_state_str = comms.request_state();
    State polled_state = this->string_state_to_enum(polled_state_str);

    // if required, change the LEDs
    if (polled_state != this->get_current_state()) {
        std::cout << "Changing state from " << this->get_current_state() << " to " << polled_state << std::endl;
        this->show_state_on_leds(polled_state, gpio);
        this->set_prev_state(this->get_current_state());
        this->set_current_state(polled_state);
        att.set_attitude_state(polled_state, gpio);
    }

    return polled_state;
}