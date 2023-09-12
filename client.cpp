#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <thread>
#include <vector>
#include <deque>
using namespace boost::asio;

const int MAX_TEXT_LENGTH = 20000;


void read_data(ip::tcp::socket& socket)
{
    //create buffer
    std::array<char, MAX_TEXT_LENGTH> read_msg;
    std::cout << "IN 1" << std::endl;
    
    //verify the socket
    if (socket.is_open()) {

        std::cout << "IN 2" << std::endl;
    }
    else {
        std::cerr << "Socket is not open." << std::endl;
    }

    //read and repeat
    async_read(socket,
        boost::asio::buffer(read_msg.data(), read_msg.size()),
        [&](boost::system::error_code ec, std::size_t)
        {   
            std::cout << "IN 2.5";
            if (!ec)
            {
                std::cout << "IN 3";
                //display the message
                std::cout.write(read_msg.data(), read_msg.size());
                std::cout << "\n";
                //clear the message
                memset(read_msg.data(), '\0', read_msg.size());
                
                read_data(socket);//repeat
            }
            else {
                std::cout << "IN 4";
            }
            
        });
}


//list of messages
std::deque<std::string> messages;


// the actual sending funcion to the server
void write_data(ip::tcp::socket& socket, io_service& ioservice) {
    if (!messages.empty()) {
        //send data async
        async_write(socket, buffer(messages.front().data(), messages.front().length()), [&](boost::system::error_code ec, std::size_t) {
            if (!ec) {
                //clean after sening
                messages.pop_front();
                //repeat for more messages
                if (!messages.empty()) {
                    write_data(socket, ioservice);
                }
            }
            });
    }
}


//verify if it is a message
void send_message(ip::tcp::socket& socket, std::string message, io_service& ioservice) {
        messages.push_back(message);
        if (!message.empty()) {
            write_data(socket, ioservice);
            
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
                std::cout << "IN 0" << std::endl;
                read_data(socket);
            }
            else {
                std::cerr << "Exception: " << ec.message() << "\n";
            }
            });

        //ioservice thread
        std::cout << "IN 10" << std::endl;
        std::thread t([&ioservice]() { ioservice.run(); }); // the thread is runnging just on the t.join

        //writing loop
        char line[MAX_TEXT_LENGTH];
        while (std::cin.getline(line, MAX_TEXT_LENGTH + 1)) {
            std::string message(line);
            send_message(socket, message, ioservice);
        }
        
        //std::cout << "IN 12" << std::endl;
        t.join(); // Wait for the thread to finish
        //std::cout << "IN 13" << std::endl;
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

