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

using namespace std;

// Semaphore variables
sem_t x, y;
pthread_t writerthreads[100];
pthread_t readerthreads[100];
int readercount = 0;

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
    int num_clients = 0;

    while (1)
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
                    char message[4096];
                    memset(message, 0, sizeof(message));
                    ssize_t bytes_received = recv(fds[i].fd, message, sizeof(message), 0);
                    if (bytes_received <= 0)
                    {
                        // Client disconnected
                        close(fds[i].fd);
                        all_sockets.erase(all_sockets.begin() + i);
                        i--; // Adjust the index after removal
                    }
                    else
                    {
                        // Process command
                        message[bytes_received] = '\0';

                        // Process command
                        char *command = strtok(message, " ");
                        char *key = strtok(NULL, " ");
                        char *val = strtok(NULL, " ");

                        // Check if command is NULL
                        if (command == NULL)
                        {
                            send(fds[i].fd, "Invalid Command", 15, 0);
                            continue; // Skip to next iteration
                        }

                        // Command processing
                        if (strcmp(command, "INSERT") == 0)
                        {
                            send(fds[i].fd, "INSERT", strlen("INSERT"), 0);
                        }
                        else if (strcmp(command, "GET") == 0)
                        {
                            send(fds[i].fd, "GET", strlen("GET"), 0);
                        }
                        else if (strcmp(command, "REMOVE") == 0)
                        {
                            send(fds[i].fd, "REMOVE", strlen("REMOVE"), 0);
                        }
                        else
                        {
                            send(fds[i].fd, "Invalid Command", 15, 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}
