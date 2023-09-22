#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <thread>
#include <vector>
#include <deque>
using namespace boost::asio;
const int MAX_TEXT_LENGTH = 1000;
int client_id = 0;

std::vector<ip::tcp::socket> clients;


void read_from_client(ip::tcp::socket& socket, int client_id) {
    boost::asio::streambuf buffer;
    std::array<char, MAX_TEXT_LENGTH> read_msg;

    while (true) {
        boost::system::error_code ec;
        std::size_t bytes_transferred = socket.read_some(boost::asio::buffer(read_msg), ec);

        if (!ec) {
            std::cout << "Client "<< client_id <<": ";
            std::cout.write(read_msg.data(), bytes_transferred);
            std::cout << "\n";

        }
        else {
            std::cout << "Client " << client_id << " disconnected " << std::endl;
            break;  
        }
    }
}

void handle_client(ip::tcp::socket socket) {
    std::cout << "creating session on: "
        << socket.remote_endpoint().address().to_string()
        << ":" << socket.remote_endpoint().port() << '\n';
    client_id++;
    clients.push_back(std::move(socket));
    read_from_client(clients.back(), client_id);
}





void do_accept(io_service& ioservice, ip::tcp::acceptor& acceptor) {
    acceptor.async_accept([&](boost::system::error_code ec, ip::tcp::socket socket) {
        if (!ec) {
            std::thread(handle_client, std::move(socket)).detach();
            
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