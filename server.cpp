#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <thread>
#include <vector>
#include <deque>
using namespace boost::asio;
const int MAX_TEXT_LENGTH = 1000;

void wait_for_request(ip::tcp::socket& socket) {
    boost::asio::streambuf buffer;
    std::array<char, MAX_TEXT_LENGTH> read_msg;
    socket.async_read_some(boost::asio::buffer(read_msg),
        [&](boost::system::error_code ec, std::size_t bytes_transferred)
        {
            if (!ec) {
                std::cout.write(read_msg.data(), bytes_transferred);
                std::cout << "\n";
                wait_for_request(socket);
            }
            else {
                std::cout << "error: " << ec << std::endl;;
            }
        });
}







std::vector<ip::tcp::socket> clients;


void do_accept(io_service& ioservice, ip::tcp::acceptor& acceptor) {
    acceptor.async_accept([&](boost::system::error_code ec, ip::tcp::socket socket) {
        if (!ec) {
            std::cout << "creating session on: "
                << socket.remote_endpoint().address().to_string()
                << ":" << socket.remote_endpoint().port() << '\n';
            clients.push_back(std::move(socket));

            wait_for_request(clients.back());
            
        }
        else {
            std::cerr << "Error: " << ec.message() << std::endl;
        }
        do_accept(ioservice, acceptor);
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
        std::cout << "aici";

    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}