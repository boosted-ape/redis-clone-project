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

// Driver Code

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
        char client_request[4096];
        char server_response[4096];
        printf("Enter your command> ");
        scanf("%s", client_request);

        if (!strcmp(client_request, "END"))
        {
            break;
        }
        // Send data to the socket
        send(network_socket, client_request,
             sizeof(client_request), 0);
        ssize_t bytes_received = recv(network_socket, server_response, sizeof(server_response), 0);
        if(bytes_received <= 0){
            break;
        }
        printf("%s\n",server_response);
    }

    // Close the connection
    close(network_socket);
    pthread_exit(NULL);

    return 0;
}
