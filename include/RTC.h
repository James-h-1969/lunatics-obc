# ifndef RTC_H
# define RTC_H

#include <ctime>
#include <iostream>
#include <chrono>
#include "Comms.h"

# define SECONDS_IN_A_MINUTE 60

class RTC {
    public: 
        RTC(Communication &comms) {this->set_current_time("2000-01-01 00:00:00");}
        std::time_t get_time();
    private:    
        void set_current_time(std::string new_time);
};

# endif