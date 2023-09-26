#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <thread>
#include <vector>
#include <deque>
#include <memory>
using namespace boost::asio;


const int MAX_TEXT_LENGTH = 1000; //maximum text length

// create a message history for a new client
const int MAX_HISTORY_MESSAGES = 100;
std::deque<std::string> history_messages;


//client details
struct Client_data {
    std::shared_ptr<ip::tcp::socket> socket;
    int id;
};

//list of clients
std::vector<Client_data> clients;


void write_history(Client_data client) {

    for (const std::string& history_message : history_messages) {
        // Send the message length followed by the message itself
       /* std::size_t message_length = history_message.size();
        boost::asio::write(*client.socket, boost::asio::buffer(&message_length, sizeof(message_length)));*/
        boost::asio::write(*client.socket, boost::asio::buffer(history_message+'\n'));
    }
}


//read from client and write to all clients
void read_from_client(Client_data client) {
    //read message buffer
    boost::asio::streambuf buffer;
    std::array<char, MAX_TEXT_LENGTH> read_msg;

    //reading then writing data loop
    while (true) {
    
        boost::system::error_code ec;

        //the actual read
        std::size_t bytes_transferred = client.socket->read_some(boost::asio::buffer(read_msg), ec);

        if (!ec) {
            //display the client data
            std::cout << "Client "<< client.id <<": ";
            std::cout.write(read_msg.data(), bytes_transferred);
            std::cout << "\n";

            //create a message to be sended to all clients
            std::string message = "Client " + std::to_string(client.id) + ": ";
            message.append(read_msg.data(), bytes_transferred);

            history_messages.push_back(message);// append the message history


            // Limit the number of historical messages
            if (history_messages.size() > MAX_HISTORY_MESSAGES) {
                history_messages.pop_front();
            }


            //sending the message to all clients
            for (auto& client : clients) {
                boost::asio::write(*client.socket, boost::asio::buffer(message));
            }
        }
        else { // error ocurred on reading form clients or disconnecting client from the server
            std::cout << "Client " << client.id << " disconnected " << std::endl;
            //remove the client after disconnecting
            clients.erase(std::remove_if(clients.begin(), clients.end(),
                [&](const Client_data& c) { return c.socket == client.socket; }), clients.end());
            break;  
        }
    }
}

//intermediary function for client handling
void handle_client(std::shared_ptr<ip::tcp::socket> socket) {

    static int id{ 0 };// for verbosity

    //display client details after connection
    std::cout << "Client "<< ++id <<" connected to port " << socket->remote_endpoint().port() << "\n";
    
    
    clients.push_back(Client_data{ socket,id });  //insert the new client
    if (history_messages.size() == 0) {
        read_from_client(clients.back()); //read data from client and write data to all clients
    }
    else {
        write_history(clients.back());
        read_from_client(clients.back()); //read data from client and write data to all clients
    }
    
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