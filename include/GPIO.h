#ifndef GPIO_H
#define GPIO_H

#include <iostream>
#include <pigpio.h>

class GPIOControl {
    public:
        GPIOControl() {
            if (gpioInitialise() < 0) {
                std::cerr << "pigpio init failed" << std::endl;
            };
            int outputPins[] = {5, 6, 18, 19, 20, 21};

            for (int pin : outputPins) {
                gpioSetMode(pin, PI_OUTPUT);
                if (pin == 19){
                    gpioWrite(pin, 1);
                } else {
                    gpioWrite(pin, 0);
                }
            }
        };
        ~GPIOControl() {
            gpioTerminate();
        }
        void setGPIOState(int pin_number, std::string value);
            
    
    
};

#endif