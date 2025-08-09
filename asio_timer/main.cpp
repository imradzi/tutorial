#include <boost/asio.hpp>
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>
#include <atomic>

using namespace std::chrono_literals;

class HeartBeat {
    boost::asio::io_context io;
    boost::asio::steady_timer timer;
    std::thread asio_thread;

    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> isTriggered {true};

public:
    HeartBeat(std::chrono::seconds nSec): io(), timer(io) {
        // Run Asio in separate thread
        asio_thread = std::thread([&] (std::chrono::seconds dur) {
            std::cout << "starting timer...\n";
            //timer = new boost::asio::steady_timer(io);
            std::function<void(boost::system::error_code)> timer_handler;
            timer_handler = [&](boost::system::error_code ec) {
                std::cout << "Timer expires\n";
                {
                    std::lock_guard lock(mtx);
                    isTriggered = true;
                }
                cv.notify_all();
                timer.expires_after(dur);
                timer.async_wait(timer_handler);
            };

            // Start timer (1 second interval)
            timer.expires_after(dur);
            timer.async_wait(timer_handler);
            io.run();
            //delete timer;
        }, nSec);
    }
    void Wait() {
        std::unique_lock lock(mtx);
        isTriggered = false;
        cv.wait(lock, [&] {
            return isTriggered.load();
        });
    }
    void Stop() {
        {
            std::lock_guard lock(mtx);
            isTriggered = true;
        }
        cv.notify_all();

        timer.cancel();
        io.stop();
        asio_thread.join();
    }
};

int main() {
    // Shared state
    HeartBeat hb(2s);
    std::atomic<bool> shutDown {false};
    std::thread thrd([&]() {
        while (!shutDown.load()) {
            //std::cout << "waiting signal\n";
            //std::this_thread::sleep_for(2ms);
            hb.Wait();
            std::cout << "signal caught\n";
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(5));
    shutDown = true;
    thrd.join();
    hb.Stop();
    std::cout << "Shutdown complete\n";
    return 0;
}

int TestExpires() {
    // Shared state
    boost::asio::io_context io;
    boost::asio::steady_timer timer(io);

    std::function<void(boost::system::error_code)> timer_handler;
    timer_handler = [&](boost::system::error_code ec) {
        std::cout << "Timer expires\n";
        timer.expires_at(timer.expiry() + std::chrono::seconds(1));
        timer.async_wait(timer_handler);
    };

    // Start timer (1 second interval)
    timer.expires_after(std::chrono::seconds(1));
    timer.async_wait(timer_handler);

    // Run Asio in separate thread
    std::thread asio_thread([&] {
        io.run();
    });

    // Run for 5 seconds
    std::this_thread::sleep_for(std::chrono::seconds(10));

    timer.cancel();
    io.stop();
    asio_thread.join();

    std::cout << "Shutdown complete\n";
    return 0;
}