// C program for the Server Side

// inet_addr
#include <arpa/inet.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <poll.h>
#include <string>
#include <iostream>
#include <sstream>
#include "ReaderThreadPool.hpp"
#include "WriterThreadPool.hpp"

using namespace std;

// Semaphore variables
sem_t x, y;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;

unordered_map<string, string> db;

// Reader Function
void *reader(void *param)
{
    sem_wait(&x);
    readercount++;
    if (readercount == 1)
        sem_wait(&y);
    sem_post(&x);

    printf("\n%d reader is inside", readercount);
    sleep(5);

    sem_wait(&x);
    readercount--;
    if (readercount == 0)
        sem_post(&y);
    sem_post(&x);

    printf("\n%d Reader is leaving", readercount + 1);
    pthread_exit(NULL);
}

std::string parseCommand(const std::string &command)
{
    std::istringstream iss(command);
    std::string action;
    iss >> action;

    if (action == "GET")
    {
        std::string key;
        if (iss >> key)
        {
            try
            {
                string result = db.at(key);
                return result;
            }
            catch (const out_of_range &e)
            {
                cerr << "Exception at " << e.what() << endl;
                return "Element is not in hash table";
            }
        }
    }
    else if (action == "SET")
    {
        std::string key, value;
        if (iss >> key >> value)
        {
            db[key] = value;
            return "SET key: " + key + " with value: " + value;
        }
    }
    else if (action == "DEL")
    {
        std::string key;
        if (iss >> key)
        {
            db.erase(key);
            return "DEL key: " + key;
        }
    }

    return "Error: Invalid command format.";
}

// Writer Function
void *writer(void *param)
{
    printf("\nWriter is trying to enter");
    sem_wait(&y);
    printf("\nWriter has entered");
    sem_post(&y);
    printf("\nWriter is leaving");
    pthread_exit(NULL);
}

// Driver Code
int main()
{
    // Initialize variables
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    vector<int> all_sockets;
    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8989);

    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    if (listen(serverSocket, 50) == 0)
        printf("Listening\n");
    else
        printf("Error\n");

    all_sockets.push_back(serverSocket);
    struct pollfd fds[100]; // To hold poll file descriptors

    printf("hi\n");

    while (true)
    {
        // Prepare the fds array for polling
        for (int i = 0; i < all_sockets.size(); i++)
        {
            fds[i].fd = all_sockets[i];
            fds[i].events = POLLIN;
        }

        int poll_count = poll(fds, all_sockets.size(), -1);
        if (poll_count < 0)
        {
            perror("poll error");
            continue;
        }
        else if (poll_count == 0)
        {
            continue;
        }

        for (int i = 0; i < all_sockets.size(); i++)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == serverSocket)
                {
                    // Accept new connection
                    addr_size = sizeof(serverStorage);
                    newSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size);
                    all_sockets.push_back(newSocket);
                }
                else
                {
                    // Handle data from client
                    string message;
                    string response;

                    uint32_t rcvDataLength;
                    uint32_t dataLength;
                    ssize_t bytesReceived = recv(fds[i].fd, &rcvDataLength, sizeof(uint32_t), 0);

                    if (bytesReceived <= 0)
                    {
                        // Client disconnected or error occurred
                        close(fds[i].fd);
                        all_sockets.erase(all_sockets.begin() + i);
                        // Since we erased the element, we should adjust i
                        i--;      // Adjust the index after removal
                        continue; // Continue to the next iteration
                    }

                    dataLength = ntohl(rcvDataLength);

                    std::vector<uint8_t> rcvBuf;     // Allocate a receive buffer
                    rcvBuf.resize(dataLength, 0x00); // with the necessary size

                    bytesReceived = recv(fds[i].fd, &(rcvBuf[0]), dataLength, 0);
                    if (bytesReceived <= 0)
                    {
                        // Client disconnected or error occurred
                        close(fds[i].fd);
                        all_sockets.erase(all_sockets.begin() + i);
                        i--;      // Adjust the index after removal
                        continue; // Continue to the next iteration
                    }

                    // assign buffered data to a string
                    message.assign((char *)&(rcvBuf[0]), rcvBuf.size()); // string
                    response = parseCommand(message);

                    uint32_t sendDataLength = htonl(response.size());
                    // Send data to the socket
                    send(fds[i].fd, &sendDataLength, sizeof(uint32_t), 0);
                    send(fds[i].fd, response.c_str(), response.size(), 0);
                }
            }
        }
    }

    return 0;
}
