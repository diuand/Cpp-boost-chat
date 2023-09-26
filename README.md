
# Chat Server

This is a chat server implemented in C++ using the Boost.Asio library. The server allows multiple clients to connect simultaneously and exchange messages in real-time. Additionally, the server stores a limited history of messages, which are sent to new clients when they connect.

## Features

- **Real-Time Chat**: Clients can connect to the server and send messages to each other in real-time.

- **Message History**: The server stores a limited history of messages, which are sent to new clients when they join the chat.

- **Multiple Clients**: The server supports multiple clients connecting and chatting simultaneously.

## How It Works

The chat server works by creating a separate thread for each connected client. When a client sends a message, it is broadcasted to all connected clients in real-time. Additionally, historical messages are sent to new clients upon connection.

## Usage

- **Connecting Clients**: Clients can connect to the server using a TCP client, specifying the server's IP address and port.

- **Sending Messages**: Clients can send messages to the server, which are then broadcasted to all connected clients.

- **Message History**: New clients receive a limited history of messages upon connecting to the server.


## Exemple
- Starts server on a home computer
- Create multiple client instances, and connect them to the server by specifying the server's IP address and port.
![image](https://github.com/diuand/Cpp-boost-chat/assets/64546774/ea681fc2-d1c4-495f-b970-2800946839ae)
- Exchange messages between the connected clients to observe real-time communication.
![image](https://github.com/diuand/Cpp-boost-chat/assets/64546774/5748d389-8de6-4d6d-b00d-662ff87cd12e)
- Then, connect an additional client to the server and witness the historical messages that were generated in previous interactions. This feature adds depth to the chat experience by allowing new participants to catch up on earlier conversations.
![image](https://github.com/diuand/Cpp-boost-chat/assets/64546774/4f726f71-c024-4704-98fc-e2dad4d303fe)


## Dependencies

- **Boost.Asio**: The project relies on the Boost.Asio library for asynchronous I/O operations and networking.
