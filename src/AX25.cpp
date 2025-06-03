#include "AX25.h"

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
};

std::string AX25::send_data(RequestType request_type, const std::string& url_params, const std::string& post_data) {
    if (!curl_) {
        std::cerr << "[ERROR] Curl not initialized." << std::endl;
        return "";  
    }    
    readBuffer_.clear();
    std::string url = base_url + url_params;
    
    // Basic curl options
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &readBuffer_);
    curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 30L);
    curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, 10L);
    
    struct curl_slist* headers = nullptr;
    
    // Configure based on request type
    switch (request_type) {
        case GET:
            curl_easy_setopt(curl_, CURLOPT_HTTPGET, 1L);
            curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, 0L);
            curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, nullptr);  
            break;

        case POST:
            curl_easy_setopt(curl_, CURLOPT_POST, 1L);
            
            if (!post_data.empty()) {
                curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, post_data.c_str());
                curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, post_data.length());
            } else {
                curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, "");
                curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, 0L);
            }
            
            headers = curl_slist_append(headers, "Content-Type: application/json");
            curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers);
            break;
    }
    
    // actual request here!!!
    res_ = curl_easy_perform(curl_);
    // !!!!!!!!!!!!!!!!!!!!!!

    if (headers) {
        curl_slist_free_all(headers);
    }
    
    if (res_ != CURLE_OK) {
        std::cerr << "[ERROR] curl_easy_perform() failed: "
                  << curl_easy_strerror(res_) << std::endl;
        return "";
    }
    
    long response_code;
    curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &response_code);
    if (response_code < 200 || response_code >= 300) {
        std::cerr << "[WARNING] HTTP response code: " << response_code << std::endl;
    }
    
    return readBuffer_;
}