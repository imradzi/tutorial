#include <chrono>
#include <ctime>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <format>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <chrono>
#include <format>
#include <iostream>
#include <mutex>
#include <functional>


using TimePoint = std::chrono::system_clock::time_point;
using namespace std::chrono_literals;
constexpr auto timezoneHr = 8h;

// {
//     auto now = std::chrono::system_clock::now();
//     const auto *tz = std::chrono::current_zone();  // System's local time zone
//     auto local_now = tz->to_local(now);            // Convert to local time

//     // Truncate to days in LOCAL time
//     auto local_today = std::chrono::floor<std::chrono::days>(local_now);
// }

TimePoint GetBeginOfYear(const TimePoint &tp) {
    using namespace std::chrono;
    const time_zone *tz = current_zone();
    auto local_tp = zoned_time(tz, tp).get_local_time();
    auto ld = floor<days>(local_tp);
    year_month_day ymd{ld};
    auto year_start = local_days {ymd.year() / 1 / 1};
    return tz->to_sys(year_start, choose::earliest);
}

TimePoint GetEndOfYear(const TimePoint &tp) {
    using namespace std::chrono;
    const time_zone *tz = current_zone();
    auto local_tp = zoned_time(tz, tp).get_local_time();
    auto ld = floor<days>(local_tp);
    year_month_day ymd{ld};
    auto next_year_start = local_days {(ymd.year() + years {1}) / 1 / 1};
    auto sys_next_year_start = tz->to_sys(next_year_start, choose::latest);
    return sys_next_year_start - 1ms;
}

auto constexpr dfmt = "{:%Y-%m-%d %a (%W)}";
void mainOut() {
    auto d = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(d.time_since_epoch());
    auto s = std::chrono::duration_cast<std::chrono::seconds>(d.time_since_epoch());
    std::cout << fmt::format("{:%F >> %X}", d) << std::endl;
    std::cout << fmt::format("  X:      {:%F >> %H:%M:%S}", d) << std::endl;
    std::cout << fmt::format(" MS:      {:%F >> %H:%M}:{:%S}", d, ms) << std::endl;
    std::cout << fmt::format(" SECONDS: {:%F >> %H:%M}:{:%S}", d, s) << std::endl;
}

constexpr auto loggerAppCode = "SVR";

std::string GetTimeInString(std::chrono::system_clock::time_point now) {
    std::string s;
    std::string absTime = std::string(loggerAppCode) + "~";
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return fmt::format("{}{:%H:%M}:{:%S}", absTime, now, ms);
}

std::string GetThreadID() {
    static std::recursive_mutex mtx;
    static std::size_t n = 0;
    static std::unordered_map<std::thread::id, std::size_t> threadMap;
    std::lock_guard<std::recursive_mutex> lock(mtx);
    auto tid = std::this_thread::get_id();
    auto iter = threadMap.find(tid);
    if (iter == threadMap.end()) {
        threadMap[tid] = n++;
    }
    return fmt::format("TID: {:04d}", threadMap[tid]);
}

void main2() {
    auto tid = std::this_thread::get_id();
    std::cout << tid << std::endl;
    std::cout << GetThreadID() << std::endl;
    for (int i = 0; i < 10; i++) {
        std::thread t([]() {
            std::cout << GetThreadID() << std::endl;
        });
        t.detach();
    }
    mainOut();
    std::cout << "---------------" << std::endl;
    auto now = std::chrono::system_clock::now();
    std::cout << GetTimeInString(now) << std::endl;
    std::cout << fmt::format(dfmt, now) << std::endl;
}


std::mutex cout_mutex;  // Synchronize access to std::cout

void print_local_time(std::function<TimePoint(TimePoint)> fnTranslate) {
    // Get current system time
    auto now = std::chrono::system_clock::now();

    now = fnTranslate(now); // return local
    // Convert to local time using current time zone
    std::chrono::zoned_time local_time {std::chrono::current_zone(), now};

    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(local_time.get_local_time().time_since_epoch()).count() % 1000; 
    std::string time_str = std::format("{0} >>> {0:%H:%M}:{0:%S}", local_time);
    std::string ms_str = std::format("{:05}", ms);

    // Thread-safe output
    {
        std::lock_guard lock(cout_mutex);
        std::cout << "Local time: " << time_str << " " << ms_str << '\n';
        std::cout << "MS: " << ms << '\n';
    }


    //again: r
    {
        now = fnTranslate(now);
        // Convert to local time using current time zone
        std::chrono::zoned_time local_time {std::chrono::current_zone(), now};

        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(local_time.get_local_time().time_since_epoch()).count() % 1000;
        std::string time_str = std::format("{0} >>> {0:%H:%M}:{0:%S}", local_time);
        std::string ms_str = std::format("{:05}", ms);

        // Thread-safe output
        {
            std::lock_guard lock(cout_mutex);
            std::cout << "Local time: " << time_str << " " << ms_str << '\n';
            std::cout << "MS: " << ms << '\n';
        }
    }
}

// Example usage
int main() {
    print_local_time([](auto t) { return GetBeginOfYear(t); });
    print_local_time([](auto t) { return GetEndOfYear(t); });
}