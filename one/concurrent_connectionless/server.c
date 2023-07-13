#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#define PORT 8080
static FILE *database;
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

void handleClient(int sock)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
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
            printf("Registration failed!!\n");
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
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddress, clientAddress;
    socklen_t addressLength = sizeof(clientAddress);

    // Create socket
    printf("Creating Socket...\n");
    sleep(2);
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Socket created\n");
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    // Bind the socket to a specific address and port
    printf("Binding Socket...\n");
    sleep(2);
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    {
        perror("Binding failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        printf("Socket bound to port %d\n", PORT);
    }

    // Start listening for incoming connections
    printf("Listening for connections...\n");
    sleep(2);
    if (listen(serverSocket, MAX_CLIENTS) < 0)
    {
        perror("Listening failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1)
    {
        // Accept a new connection
        if ((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &addressLength)) < 0)
        {
            perror("Accepting connection failed");
            exit(EXIT_FAILURE);
        }
        else
        {
            sleep(2);
            printf("Accepting connection successful\n");
        }

        printf("New client connected\n");

        // Fork a new process to handle the client
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {
            // Child process
            close(serverSocket); // Close the server socket in the child process
            handleClient(clientSocket);
            exit(EXIT_SUCCESS);
        }
        else
        {
            // Parent process
            close(clientSocket); // Close the client socket in the parent process
        }
    }

    return 0;
}
