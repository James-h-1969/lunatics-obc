#include "Attitude.h"

std::vector<float> Attitude::take_attitude_recording() {
    // TODO. find how to get from i2c the attitude from the pico

    std::vector<float> vector(6);
    return vector;
};

void Attitude::set_attitude_state(enum State state, GPIOControl gpio) {
    /*
    Based on the overall state machine, we pass the current attitude state down through 
    GPIO to the pico.    
    */
    switch(state) {
        case INIT:
            gpio.setGPIOState(ATTITUDE_GPIO_1, "LOW");
            gpio.setGPIOState(ATTITUDE_GPIO_2, "LOW");
            break;
        case PAYLOAD:
            gpio.setGPIOState(ATTITUDE_GPIO_1, "LOW");
            gpio.setGPIOState(ATTITUDE_GPIO_2, "HIGH");
            break;
        case DETUMBLING:
            gpio.setGPIOState(ATTITUDE_GPIO_1, "HIGH");
            gpio.setGPIOState(ATTITUDE_GPIO_2, "LOW");
            break;
        case NOMINAL:
            gpio.setGPIOState(ATTITUDE_GPIO_1, "LOW");
            gpio.setGPIOState(ATTITUDE_GPIO_2, "HIGH");
            break;
        case POWER_OFF:
            gpio.setGPIOState(ATTITUDE_GPIO_1, "LOW");
            gpio.setGPIOState(ATTITUDE_GPIO_2, "LOW");
            break;
        case NULL_STATE:
            gpio.setGPIOState(ATTITUDE_GPIO_1, "LOW");
            gpio.setGPIOState(ATTITUDE_GPIO_2, "LOW");
            break;
    }
}