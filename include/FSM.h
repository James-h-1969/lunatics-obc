#ifndef FSM_H
#define FSM_H

#include <cstdio>
#include <string>
#include <bitset>
#include "GPIO.h"
#include "Comms.h"
#include "Attitude.h"
#include "State.h"
#include <iostream>

class Communication;

class FSM {
    /*
    This class holds the current state of the satellite, and has 
    a function to poll it from the GUI.
    */
    public:
        FSM(): current_state_(INIT), prev_state_(INIT) {};
        enum State poll_state(Communication& comms, GPIOControl gpio, Attitude att);
        void set_current_state(enum State new_state) {current_state_ = new_state;};
        enum State get_current_state() {return current_state_;};
        void set_prev_state(enum State new_state) {prev_state_ = new_state;};
        enum State get_prev_state() {return prev_state_;};
        enum State string_state_to_enum(std::string state);
    private:
        enum State current_state_;
        enum State prev_state_;
        void show_state_on_leds(enum State state, GPIOControl gpio);
};

#endif 