#include "RTC.h"

std::time_t RTC::get_time() {
    return std::time(nullptr);

    // TODO. here send i2c requesting the time from the clock and turn into std::time_t
};

void RTC::set_current_time(std::string new_time) {
    std::cout << "Setting current time: " << new_time << std::endl;

    // TODO: here send i2c to the clock that sets the time
}