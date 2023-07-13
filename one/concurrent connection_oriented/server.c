#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
static FILE *database;
#define MAX_CLIENTS 5

void handle_client(int sock)
{
    char buffer[1024] = {0};
    char name[10];
    char reg[20];
    char serial[10];
    int option;

    read(sock, buffer, 1024);
    sscanf(buffer, "%d %s %s %s", &option, name, serial, reg);

    if (option == 2)
    {
        printf("Viewing........\n");
        // Send the contents of the file to the client
        database = fopen("registrations.txt", "a+");
        if (database == NULL)
        {
            send(sock, "Error opening database", strlen("Error opening database"), 0);
        }
        else
        {
            char line[1024];
            char result[1024];
            char temp[1024];
            while (fgets(temp, 50, database))
            {
                strcpy(line, temp);
                strcat(line, "\n");
                strcat(result, line);
            }
            send(sock, result, strlen(result), 0);
            strcpy(result, "");
            fclose(database);
            printf("Done sending!\n");
        }
    }
    else if (option == 1)
    {
        // Receive the registration data from the client and save it to the file
        printf("Registering........\n");

        database = fopen("registrations.txt", "a+");
        if (database == NULL)
        {
            send(sock, "Error opening database", strlen("Error opening database"), 0);
        }
        char temp[100];
        int index = 0;

        while (fgets(temp, 100, database) != NULL)
        {
            char *token = strtok(temp, " ");
            int count = 0;
            while (token != NULL)
            {

                if (count == 2)
                {
                    if (strstr(token, reg) != NULL)
                    {
                        index++;
                        printf("%d", index);
                    }
                }
                count++;
                token = strtok(NULL, " ");
            }
        }
        if (index == 0)
        {
            fprintf(database, "%s %s %s\n", name, serial, reg);
            fclose(database);
            send(sock, "Registration complete!", strlen("Registration complete!"), 0);
            printf("Done registering\n");
        }
        else
        {
            send(sock, "Registration number in use!", strlen("Registration number in use!"), 0);
            printf("Registration failed!!");
        }
    }
    else
    {
        send(sock, "unknown command", strlen("unknown command"), 0);
    }
    close(sock);
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    int client_address_len = sizeof(client_address);
    int pid;

    // Create socket
    printf("Creating socket\n");
    sleep(2);
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Socket created\n");
        sleep(2);
    }

    // Prepare server address structure
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Bind socket to server address
    printf("Binding socket to port %d\n", PORT);
    sleep(2);
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Socket binding complete\n");
        sleep(2);
    }

    // Listen for incoming connections
    printf("Listening for incoming connections\n");
    sleep(2);
    if (listen(server_socket, MAX_CLIENTS) < 0)
    {
        perror("Error listening");
        exit(EXIT_FAILURE);
    }
    else
    {
        sleep(1);
    }

    // Accept incoming connections
    while (1)
    {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, (socklen_t *)&client_address_len);
        if (client_socket < 0)
        {
            perror("Error accepting connection");
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("Connected to a client\n");
        }

        // Fork a new process to handle the client
        pid = fork();
        if (pid < 0)
        {
            perror("Error forking process");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // Child process (client handler)
            close(server_socket); // Close the server socket in the child process
            handle_client(client_socket);
            exit(EXIT_SUCCESS);
        }
        else
        {
            // Parent process (continue accepting connections)
            close(client_socket); // Close the client socket in the parent process
        }
    }

    // Close server socket
    close(server_socket);

    return 0;
}
