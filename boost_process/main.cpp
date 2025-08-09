#include <boost/asio.hpp>
#include <boost/asio/readable_pipe.hpp>
#include <boost/asio/writable_pipe.hpp>
#include <boost/process.hpp>
#include <iostream>
#include <string>
int main() {
    boost::asio::io_context ctx;
    boost::asio::readable_pipe pipeOut(ctx);
    boost::asio::writable_pipe pipeIn(ctx);
    boost::process::v2::process proc(ctx, "/usr/bin/g++", {"--version"}, boost::process::process_stdio {pipeIn, pipeOut, {/*err*/}});

    pipeIn.write_some(boost::asio::buffer("xxx\n\n\n\n"));

    std::vector<char> buffer(1024);  // Buffer to hold read data
    boost::system::error_code ec;
    size_t bytes_read;
    std::string output;
    do {
        bytes_read = pipeOut.read_some(boost::asio::buffer(buffer), ec);
        if (ec) {
            if (ec == boost::asio::error::eof) {
                // End of file reached, exit loop
                break;
            } else {
                std::cerr << "Error reading from pipe: " << ec.message() << std::endl;
                break;
            }
        }
        // Process the data in buffer (0 to bytes_read)
        output.append(buffer.data(), bytes_read);
    } while (bytes_read > 0);

    proc.wait();

    std::cout << "\nOutput: " << output << std::endl;
}