#include <curl/curl.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fmt/format.h>

static std::size_t fnWriteData(void *ptr, std::size_t size, std::size_t nmemb, void *stream) {
    std::string data((const char *)ptr, (std::size_t)size * nmemb);
    *((std::stringstream *)stream) << data;
    return size * nmemb;
}

class WebClient {

public:
    static std::string Post(const std::string &url, const std::string &body) {
        auto curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_POST, 1L);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, body.length());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  // Prevent "longjmp causes uninitialized stack frame" bug
            curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
            std::stringstream out;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fnWriteData);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
            /* Perform the request, res will get the return code */
            CURLcode res = curl_easy_perform(curl);
            /* Check for errors */
            if (res != CURLE_OK) {
                throw fmt::format("curl_easy_perform() failed: {}", curl_easy_strerror(res));
            }
            curl_easy_cleanup(curl);
            return out.str();
        }
        return "";
    }

    static std::string Get(const std::string &url) {
        auto curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
            curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);  // Prevent "longjmp causes uninitialized stack frame" bug
            curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "deflate");
            std::stringstream out;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fnWriteData);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &out);
            /* Perform the request, res will get the return code */
            CURLcode res = curl_easy_perform(curl);
            /* Check for errors */
            if (res != CURLE_OK) {
                throw fmt::format("curl_easy_perform() failed: {}", curl_easy_strerror(res));
            }
            curl_easy_cleanup(curl);
            return out.str();
        }
        return "";
    }
};

int main() {
    try {
        std::cout << WebClient::Get("http://localhost:9090/testing11111");

        std::cout << std::endl;
        std::cout << std::endl;

        std::cout << WebClient::Post("http://localhost:9090/testing11111", "coe=123&name=radzi");

    } catch (...) {
    }
}