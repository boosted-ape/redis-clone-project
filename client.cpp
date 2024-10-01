// C program for the Client Side
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

// inet_addr
#include <arpa/inet.h>
#include <unistd.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>


#include <vector>
#include <string>
#include <iostream>
using namespace std;

int main()
{

    int network_socket;

    // Create a stream socket
    network_socket = socket(AF_INET,
                            SOCK_STREAM, 0);

    // Initialise port number and address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8989);

    // Initiate a socket connection
    int connection_status = connect(network_socket,
                                    (struct sockaddr *)&server_address,
                                    sizeof(server_address));

    // Check for connection error
    if (connection_status < 0)
    {
        puts("Error\n");
        return 0;
    }

    printf("Connection established\n");

    while (true)
    {
        string client_request;
        string server_response;
        cout << "Enter your command> ";
        getline(cin, client_request);

        if (!client_request.compare("END"))
        {
            break;
        }

        uint32_t dataLength = htonl(client_request.size());
        // Send data to the socket
        send(network_socket, &dataLength, sizeof(uint32_t), 0);
        send(network_socket, client_request.c_str(),
             client_request.size(), 0);

        uint32_t rcvDataLength;
        uint32_t cleanedDataLength;
        recv(network_socket, &rcvDataLength, sizeof(uint32_t), 0); // Receive the message length
        cleanedDataLength = ntohl(rcvDataLength);                    // Ensure host system byte order

        std::vector<uint8_t> rcvBuf;     // Allocate a receive buffer
        rcvBuf.resize(cleanedDataLength, 0x00); // with the necessary size

        recv(network_socket, &(rcvBuf[0]), cleanedDataLength, 0); // Receive the string data

        // assign buffered data to a
        server_response.assign((char *)&(rcvBuf[0]), rcvBuf.size()); // string

        
        if (rcvBuf.size() <= 0)
        {
            break;
        }
        cout << server_response << '\n';
    }

    // Close the connection
    close(network_socket);

    return 0;
}
