# ifndef ATTITUDE_H
# define ATTITUDE_H

#include <vector>
#include "GPIO.h"
#include "State.h"

#define ATTITUDE_GPIO_1 5
#define ATTITUDE_GPIO_2 6

class Attitude {
    public:
        std::vector<float> take_attitude_recording();
        void Attitude::set_attitude_state(enum State state, GPIOControl gpio);
};

# endif