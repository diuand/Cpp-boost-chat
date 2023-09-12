#include <boost/asio.hpp>
#include <iostream>
#include <vector> 
#include <boost/array.hpp>

using namespace boost::asio;

//std::vector<std::string> Users{ "1","2" };

std::vector<ip::tcp::socket> clients;


void client_handle(ip::tcp::socket client) {
    try {
        boost::system::error_code ec;

        // add a new communication client
        clients.push_back(std::move(client));
        std::cout << "Client " << clients.size()<< " Connected\n";
        

        //continuous communication until the user leaves
        while (true) {

            //data buffer
            boost::array<char, 200> buff;

            //waiting for a respond and put it into the buffer
            size_t len = clients.back().read_some(buffer(buff), ec);

            // disconnect the user after ending the communication
            if (ec) {
                std::cerr << "Client "<< clients.size() << " Disconnected\n";
                clients.pop_back();
                break;
            }

            // print data on terminal
            std::cout << "Client " << clients.size()<<": " << std::string(buff.data(), len) << std::endl;

            //get the sender endpoint
            auto snder_endpoint = clients.back().remote_endpoint();

            //// send the data to all clients except the user
            //for (auto& client_s : clients) {
            //    // get the client endpoint
            //    auto client_endpoint = client_s.remote_endpoint();
            //    if (client_endpoint != snder_endpoint) { // verify if the sender is not in clients
            //        auto text = "NME: " + std::string(buff.data(), len);
            //        write(client_s, buffer(text), ec);
            //    }
            //    else {
            //        auto text = "ME: " + std::string(buff.data(), len);
            //        write(client_s, buffer(text), ec);
            //    }

            //}

            //send message to all users
            for (auto& client_s : clients) {
                //create the sending text
                auto text =  "Server: " + std::string(buff.data(), len);
                //sending it
                write(client_s, buffer(text), ec);
                //verification
                if (ec) {
                    std::cerr << "ERRROR: " << ec.message() << std::endl;
                }
                else {
                    std::cout << "sended;" << std::endl;
                }
            }

        }
        
    
    }
    catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    

}


int main() {
    
    //initiate server
    io_service ioservice;
    auto endpoint = ip::tcp::endpoint(ip::tcp::v4(), 55004);
    ip::tcp::acceptor acceptor(ioservice, endpoint);

    std::cout << "Server creat pe port 55004" << std::endl;
    
    //waiting for new clients
    while (true) {
        ip::tcp::socket client(ioservice);
        acceptor.accept(client);
        
        // new thread for communication after a client connected
        std::thread(client_handle, std::move(client)).detach();
        
    
    
    }
    return 0;
}

