#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int clientSocket;
    struct sockaddr_in serverAddress;
    struct hostent *server;
    char buffer[BUFFER_SIZE];

    // Create socket
    printf("Creating Socket...\n");
    sleep(2);
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Socket created\n");
    }

    server = gethostbyname("localhost");

    if (server == NULL)
    {
        perror("Host not found");
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr = *((struct in_addr *)server->h_addr);
    memset(&(serverAddress.sin_zero), '\0', 8);

    // Connect to the server
    printf("Connecting to server.....\n");
    if (connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Connected to server");
        sleep(1);
    }

    int option;
    char name[20];
    char reg[20];
    char serial[20];
    printf("\n1. Register a new user\n");
    printf("2. View existing registrations\n");
    printf("3. Exit\n");
    printf("Enter an option: ");
    scanf("%d", &option);

    switch (option)
    {
    case 1: // Register a new user
        printf("Enter name: ");
        scanf("%s", name);
        printf("Enter serial number: ");
        scanf("%s", serial);
        printf("Enter registration number: ");
        scanf("%s", reg);
        sprintf(buffer, "%d %s %s %s\n", option, name, serial, reg);
        send(clientSocket, buffer, sizeof(buffer), 0);
        buffer[0] = '\0';
        read(clientSocket, buffer, 1024);
        printf("%s\n", buffer);
        if (strcmp(buffer, "error") == 0)
        {
            exit(1);
        }
        break;
    case 2: // View existing registrations
        sprintf(buffer, "%d\n", option);
        buffer[0] = '\0';
        send(clientSocket, buffer, sizeof(buffer), 0);
        strcpy(buffer, " ");
        read(clientSocket, buffer, 1024);
        printf("Registrations:\n");
        printf("%s", buffer);

        break;

    case 3: // Exit
        printf("Exiting program\n");
        break;
    default:
        printf("Invalid option\n");
        break;
    }

    close(clientSocket);

    return 0;
}

// Send data to the server
// strcpy(buffer, "Hello, server!");
// if (send(clientSocket, buffer, strlen(buffer), 0) < 0)
// {
//     perror("Error sending data to server");
//     exit(EXIT_FAILURE);
// }

// // Receive response from the server
// memset(buffer, 0, sizeof(buffer));
// if (recv(clientSocket, buffer, sizeof(buffer), 0) < 0)
// {
//     perror("Error receiving response from server");
//     exit(EXIT_FAILURE);
// }

// printf("Server response: %s\n", buffer);