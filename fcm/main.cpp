#include <curl/curl.h>
#include <string>
#include <iostream>

// Callback to handle HTTP response data
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    s->append((char*)contents, newLength);
    return newLength;
}

int main() {
    CURL* curl;
    CURLcode res;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // FCM endpoint
        std::string url = "https://fcm.googleapis.com/fcm/send";

        // Replace with your FCM server key and device token
        std::string server_key = "YOUR_SERVER_KEY";
        std::string device_token = "DEVICE_REGISTRATION_TOKEN";

        // JSON payload (customize as needed)
        std::string json_payload = R"(
            {
                "to": ")" + device_token + R"(",
                "priority": "high",
                "notification": {
                    "title": "Hello from C++",
                    "body": "This is a test message"
                },
                "data": {
                    "custom_key": "custom_value"
                }
            }
        )";

        // Set headers
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: key=" + server_key).c_str());

        // Response container
        std::string response;

        // Configure libcurl
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Send request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        } else {
            std::cout << "Response: " << response << std::endl;
        }

        // Cleanup
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return 0;
}