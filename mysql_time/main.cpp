#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <cstdlib>
#include <fmt/format.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <unistd.h>
#include <vector>
#include <mysqlx/xdevapi.h>
#include <iostream>
#include <chrono>
#include <string>
#include <random>
#include <iomanip>
#include <unordered_map>

typedef std::chrono::system_clock::time_point TimePoint;


TimePoint toTimePoint(const std::string& mysql_time) {
    if (mysql_time.empty()) {
        throw std::runtime_error("Cannot parse empty MySQL timestamp string");
    }

    // Check if the timestamp includes microseconds
    size_t microsecond_pos = mysql_time.find('.');
    std::string base_time = mysql_time;
    int microseconds = 0;

    if (microsecond_pos != std::string::npos) {
        base_time = mysql_time.substr(0, microsecond_pos);
        std::string microsecond_str = mysql_time.substr(microsecond_pos + 1);

        // Pad or truncate to 6 digits
        if (microsecond_str.length() < 6) {
            microsecond_str.append(6 - microsecond_str.length(), '0');
        } else if (microsecond_str.length() > 6) {
            microsecond_str = microsecond_str.substr(0, 6);
        }

        try {
            microseconds = std::stoi(microsecond_str);
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid microseconds in MySQL timestamp: " + mysql_time);
        }
    }

    std::tm tm = {};
    std::istringstream ss(base_time);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

    if (ss.fail()) {
        throw std::runtime_error("Failed to parse MySQL timestamp: " + mysql_time);
    }

    // Use timegm for UTC interpretation to be consistent with mysqlValueToTimePoint
    time_t time_t_val = timegm(&tm);
    if (time_t_val == static_cast<time_t>(-1)) {
        throw std::runtime_error("Failed to convert parsed time to time_t: " + mysql_time);
    }

    auto time_point = std::chrono::system_clock::from_time_t(time_t_val);

    // Add microseconds if present
    if (microseconds > 0) {
        time_point += std::chrono::microseconds(microseconds);
    }

    return time_point;
}

TimePoint fromInt(int64_t unix_epoch_seconds) {
    std::chrono::seconds duration_since_epoch(unix_epoch_seconds);
    return std::chrono::time_point<std::chrono::system_clock>(duration_since_epoch);
    // std::time_t converted_time_t = std::chrono::system_clock::to_time_t(time_point);
    // std::cout << "Converted time_point: " << std::ctime(&converted_time_t) << std::endl;
}
template<typename T>
std::string get(const mysqlx::Value& value, T default_value) {
    if (value.isNull()) {
        return "";
    }
    if constexpr (std::is_same_v<T, TimePoint>) {
        auto t = fromInt(value.get<int64_t>());
        auto ct = std::chrono::system_clock::to_time_t(t);
        std::string s = std::ctime(&ct);
        return boost::trim_copy(s);
    }
    return value.get<std::string>();
}

using namespace std::string_literals;

// std::string toString(const mysqlx::Value &b) {
//     auto x = b.get<std::string>();
//     return fmt::format("bin({} {})", (void *)x.first, x.second);
// }

int main(int argc, char* argv[]) {
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            auto e = std::getenv(argv[i]);
            if (e) std::cout << argv[i] << ": " << e << std::endl;
        }
    }
    
    std::cout << std::endl;
    try {
        const char* url = "mysqlx://root:ft1832h@127.0.0.1:33061/bkk_master";

        std::cout << "Creating session on " << url << " ..." << std::endl;
        mysqlx::Client client(url);
        auto sess = client.getSession();

        std::cout << "Successfully connected to MySQL Server!" << std::endl;

        auto rs = sess.sql("select firebase_uid, email, UNIX_TIMESTAMP(created_at), UNIX_TIMESTAMP(updated_at) from users;").execute();
        for (auto x : rs) {
            std::cout << x[0] << " \t" << x[1] << "\t" << get(x[2], TimePoint{}) << "\t" << get(x[3], TimePoint{}) << std::endl;
        }

        auto now = std::chrono::system_clock::now();
        sess.sql("update users set updated_at=?").bind(std::format("{:%Y-%m-%d %H:%M:%S}", now)).execute();
        sess.close();

    } catch (const mysqlx::Error& err) {
        std::cerr << "MySQLX Error: " << err.what() << std::endl;
        return 1;
    } catch (const std::exception& ex) {
        std::cerr << "Standard Exception: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}
