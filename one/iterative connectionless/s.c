#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8888
#define BUFFER_SIZE 1024

struct sockaddr_in serverAddr, clientAddr;
void handle_request(int sock);

static FILE *database;

int main()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    char buffer[BUFFER_SIZE];
    printf("Creating socket...");
    sleep(1);

    // Create socket
    if ((sockfd) < 0)
    {
        perror("\nSocket creation failed");
        exit(EXIT_FAILURE);
    }else{printf("\nSocket created");}

    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));

    printf("\nConfiguring server address...");
    sleep(1);

    // Configure server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_port = htons(PORT);

    printf("\nBinding socket...");
    sleep(1);

    // Bind socket to server address
    if (bind(sockfd, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }else{printf("\nBinding socket done");}

    printf("\nServer started and listening on port %d\n", PORT);
    sleep(1);
    while (1)
    {
        handle_request(sockfd);
    }
    return 0;
}

void handle_request(int sockfd)
{
    socklen_t len = sizeof(clientAddr);
    char name[20];
    char reg[20];
    char serial[20];
    int option;
    char buffer[1024] = {0};

    recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &len);
    sscanf(buffer, "%d %s %s %s", &option, name, serial, reg);

    if (option == 2)
    {
        printf("Viewing........\n");
        sleep(5);
        // Send the contents of the file to the client
        database = fopen("registrations.txt", "a+");
        if (database == NULL)
        {
            sendto(sockfd, "Error opening database", strlen("Error opening database"), 0, (const struct sockaddr *)&clientAddr, len);
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
            sendto(sockfd, result, sizeof(buffer), 0, (const struct sockaddr *)&clientAddr, len);
            strcpy(result, "");
            fclose(database);
            printf("Done sending!\n");
        }
    }
    else if (option == 1)
    {
        // Receive the registration data from the client and save it to the file
        printf("Registering........\n");
        sleep(5);
        database = fopen("registrations.txt", "a+");
        if (database == NULL)
        {
            sendto(sockfd, "Error", strlen("Error"), 0, (const struct sockaddr *)&clientAddr, len);
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
                    if (strstr(token, reg) != 0)
                    {
                        index++;
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
            sendto(sockfd, "Registration complete!", strlen("Registration complete!"), 0, (const struct sockaddr *)&clientAddr, len);

            printf("Done registering\n");
        }
        else
        {
            sendto(sockfd, "Registration number in use!", strlen("Registration number in use!"), 0, (const struct sockaddr *)&clientAddr, len);

            printf("Registration failed!!");
        }
    }
    else
    {
        sendto(sockfd, "unknown command", strlen("unknown command"), 0, (const struct sockaddr *)&clientAddr, len);
    }
}
