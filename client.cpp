#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <thread>
#include <vector>
#include <deque>
using namespace boost::asio;

const int MAX_TEXT_LENGTH = 1000;


void read_data(ip::tcp::socket& socket)
{
    // Create a buffer
    std::array<char, MAX_TEXT_LENGTH> read_msg;

    // read from server
    socket.async_read_some(boost::asio::buffer(read_msg),
        [&](boost::system::error_code ec, std::size_t bytes_transferred) {
            if (!ec) {
                // Display the received message
                std::cout.write(read_msg.data(), bytes_transferred);
                std::cout << "\n";
                // Continue reading
                read_data(socket); 
            }
            else { // handle errors
                std::cerr << "Read error: " << ec.message() << "\n";
            }
        });
}


//list of messages
std::deque<std::string> messages;


// sending messages to the server
void write_data(ip::tcp::socket& socket, io_service& ioservice) {
    if (!messages.empty()) {
        //send data to the server
        async_write(socket, buffer(messages.front().data(), messages.front().length()), [&](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                //remove message after sending it
                messages.pop_front();
                //repeat for more messages
                if (!messages.empty()) {
                    write_data(socket, ioservice);
                }
            }
            });
    }
}


//message verification function
void send_message(ip::tcp::socket& socket, std::string message, io_service& ioservice) {
    messages.push_back(message);
    if (!message.empty()) {
        write_data(socket, ioservice);//send to the server

    }
}



int main() {
    try {
        // Create a pipe to the server
        io_service ioservice;
        ip::tcp::socket socket(ioservice);
        ip::tcp::resolver resolver(ioservice);

        // read data after connecting
        async_connect(socket, resolver.resolve({ "127.0.0.1", "55004" }), [&](boost::system::error_code ec, ip::tcp::endpoint) {
            if (!ec) {
                read_data(socket);
            }
            else {
                std::cerr << "Exception: " << ec.message() << "\n";
            }
            });

        //ioservice thread
        std::thread t([&ioservice]() { ioservice.run(); }); // the thread is runnging just on the t.join

        //writing loop
        char line[MAX_TEXT_LENGTH];
        while (std::cin.getline(line, MAX_TEXT_LENGTH + 1)) {
            std::string message(line);
            send_message(socket, message, ioservice);

        }

        t.join(); // Wait for the thread to finish
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}