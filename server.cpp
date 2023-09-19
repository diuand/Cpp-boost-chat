#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <thread>
#include <vector>
#include <deque>
using namespace boost::asio;



void do_accept(io_service& ioservice, ip::tcp::acceptor& acceptor) {
    acceptor.async_accept([&](boost::system::error_code ec, ip::tcp::socket socket) {
        if (!ec) {
            std::cout << "CONNECTED\n";
            do_accept(ioservice, acceptor);
        }
        else {
            std::cerr << "Error: " << ec.message() << std::endl;
        }
        });

}

int main() {
    try {
        // Create a pipe to the server
        io_service ioservice;

        ip::tcp::endpoint endpoint(ip::tcp::v4(), 55004);

        ip::tcp::acceptor acceptor(ioservice, endpoint);

        do_accept(ioservice, acceptor);
        ioservice.run();


    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}