# ifndef AX25_H
# define AX25_H

#include <curl/curl.h>
#include <string>
#include <iostream>
#include <nlohmann/json.hpp>
#include <ctime>
#include "GPIO.h"

enum RequestType {
    POST,
    GET,
};

using json = nlohmann::json; 

enum DataType {
    WOD_DATA = 0x0E,      
    SCIENCE_DATA = 0x0F,
    ATTITUDE_DATA = 0x09
};

static std::string source_address = "LTIC01"; // lunatics-satellite
static std::string destination_address = "LTICGS"; // lunatics-ground-station

class AX25 {
    /*
    This class holds the logic required to send the requests to the server.a64l

    Followed from
    - https://gist.github.com/alghanmi/c5d7b761b2c9ab199157?permalink_comment_id=2046201
    */
    public:
        AX25(GPIOControl& gpio) {curl_ = curl_easy_init();gpio_=gpio;};
        ~AX25() {curl_easy_cleanup(curl_);};
        std::string send_data(RequestType request_type, const std::string& url_params, const std::string& post_body = ""); 
        int check_valid_ax25_header(const json& response_header);
    private:
        // stuff for wifi requests
        CURL* curl_;
        CURLcode res_;
        std::string readBuffer_;
        std::string base_url = "http://192.168.78.201:5000/";

        GPIOControl gpio_;
};

# endif