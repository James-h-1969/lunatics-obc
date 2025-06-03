#include "GPIO.h"

void GPIOControl::setGPIOState(int pin_number, std::string value){
    if (gpioInitialise() < 0) {
        std::cerr << "pigpio init failed" << std::endl;
    };
    if (value == "HIGH") {
        gpioWrite(pin_number, 1);
    } else {
        gpioWrite(pin_number, 0);
    }
};