#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main()
{
    int client_socket;
    struct sockaddr_in server_address;
    char buffer[BUFFER_SIZE];

    // Create socket
    printf("Creating socket........\n");
    sleep(1);
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }else{printf("Socket Created");}

    // Prepare server address structure
    printf("Converting IPv4 and IPv6 addresses from text to binary form");
    sleep(1);
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change to server IP address
    server_address.sin_port = htons(PORT);

    // Connect to server
    printf("Connecting...\n");
    sleep(1);
    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Error connecting to server");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Connected to server\n");
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
        send(client_socket, buffer, sizeof(buffer), 0);
        strcpy(buffer, " ");
        read(client_socket, buffer, 1024);
        printf("%s\n", buffer);
        if (strcmp(buffer, "error") == 0)
        {
            exit(1);
        }
        break;
    case 2: // View existing registrations
        sprintf(buffer, "%d\n", option);
        send(client_socket, buffer, sizeof(buffer), 0);
        strcpy(buffer, " ");
        read(client_socket, buffer, 1024);
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

    // Close the socket
    close(client_socket);

    return 0;
}
