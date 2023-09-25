#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <thread>
#include <vector>
#include <deque>
#include <memory>
using namespace boost::asio;


const int MAX_TEXT_LENGTH = 1000; //maximum text length
int client_id = 0; // for verbosity

//list of clients
std::vector<std::shared_ptr<ip::tcp::socket>> clients;

//read from client and write to all clients
void read_from_client(std::shared_ptr<ip::tcp::socket>  socket, int client_id) {
    //read message buffer
    boost::asio::streambuf buffer;
    std::array<char, MAX_TEXT_LENGTH> read_msg;

    //reading then writing data loop
    while (true) {
    
        boost::system::error_code ec;

        //the actual read
        std::size_t bytes_transferred = socket->read_some(boost::asio::buffer(read_msg), ec);

        if (!ec) {
            //display the client data
            std::cout << "Client "<< client_id <<": ";
            std::cout.write(read_msg.data(), bytes_transferred);
            std::cout << "\n";

            //create a message to be sended to all clients
            std::string message = "Client " + std::to_string(client_id) + ": ";
            message.append(read_msg.data(), bytes_transferred);

            //sending the message to all clients
            for (auto& client : clients) {
                boost::asio::write(*client, boost::asio::buffer(message));
            }
        }
        else { // error ocurred on reading form clients or disconnecting client from the server
            std::cout << "Client " << client_id << " disconnected " << std::endl;
            clients.erase(std::remove(clients.begin(), clients.end(), socket), clients.end());
            break;  
        }
    }
}

//intermediary function for client handling
void handle_client(std::shared_ptr<ip::tcp::socket> socket) {

    //display client details after connection
    std::cout << "creating session on: "
        << socket->remote_endpoint().address().to_string()
        << ":" << socket->remote_endpoint().port() << '\n';
    
    client_id++; // for verbosity
   
    clients.push_back(socket);  //insert the new client
    
    read_from_client(socket, client_id); //read data from client and write data to all clients
}

//accept the new client and start a separate thread for him
void do_accept(io_service& ioservice, ip::tcp::acceptor& acceptor) {
    acceptor.async_accept([&](boost::system::error_code ec, ip::tcp::socket socket) {
        if (!ec) {
            //create a separate thread for the new client
            auto client_socket = std::make_shared<ip::tcp::socket>(std::move(socket)); 
            std::thread(handle_client, client_socket).detach();
            
        }
        else { //handle client connection errors
            std::cerr << "Error: " << ec.message() << std::endl;
        }
        //waiting for new client
        do_accept(ioservice, acceptor);
        });

}

int main() {
    try {
        // Create the server
        io_service ioservice;
        ip::tcp::endpoint endpoint(ip::tcp::v4(), 55004);
        ip::tcp::acceptor acceptor(ioservice, endpoint);

        //handle incoming clients
        do_accept(ioservice, acceptor);
        ioservice.run();

    }
    catch (std::exception& e) { 
        std::cerr << "Exception: " << e.what() << std::endl; //handle server errors
    }

    return 0;
}