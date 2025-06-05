#include "AX25.h"

static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
};

std::string AX25::send_data(RequestType request_type, const std::string& url_params, const std::string& post_data) {
    gpio_.setGPIOState(18, "HIGH");
    
    if (!curl_) {
        std::cerr << "[ERROR] Curl not initialized." << std::endl;
        gpio_.setGPIOState(18, "LOW");
        return "";
    }
    
    readBuffer_.clear();
    std::string url = base_url + url_params;
    
    // Basic curl options with shorter timeouts for fast failure
    curl_easy_setopt(curl_, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &readBuffer_);
    curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 3L);        // Total timeout: 3 seconds
    curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, 2L); // Connection timeout: 2 seconds
    curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1L);       // Prevent hanging on some systems
    curl_easy_setopt(curl_, CURLOPT_FAILONERROR, 1L);    // Fail on HTTP error codes
    
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
    
    // Actual request here
    res_ = curl_easy_perform(curl_);
    
    // Clean up headers
    if (headers) {
        curl_slist_free_all(headers);
    }
    
    // Handle connection/server errors - just log and move on
    if (res_ != CURLE_OK) {
        switch (res_) {
            case CURLE_COULDNT_CONNECT:
                std::cerr << "[INFO] Server not reachable, continuing..." << std::endl;
                break;
            case CURLE_OPERATION_TIMEDOUT:
                std::cerr << "[INFO] Server timeout, continuing..." << std::endl;
                break;
            case CURLE_COULDNT_RESOLVE_HOST:
                std::cerr << "[INFO] Could not resolve host, continuing..." << std::endl;
                break;
            default:
                std::cerr << "[INFO] Connection failed (" << curl_easy_strerror(res_) << "), continuing..." << std::endl;
                break;
        }
        gpio_.setGPIOState(18, "LOW");
        return ""; // Return empty string and let the program continue
    }
    
    long response_code;
    curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &response_code);
    if (response_code < 200 || response_code >= 300) {
        std::cerr << "[INFO] HTTP error " << response_code << ", continuing..." << std::endl;
        gpio_.setGPIOState(18, "LOW");
        return ""; // Return empty string for HTTP errors too
    }
    
    gpio_.setGPIOState(18, "LOW");
    return readBuffer_;
}