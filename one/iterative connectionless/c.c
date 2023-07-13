#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int sockfd,valread;
    char name[20];
    char reg[20];
    char serial[20];
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // Create socket
    printf("Creating Socket.....\n");
    sleep(1);
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }else{printf("Socket created");}

    memset(&serverAddr, 0, sizeof(serverAddr));

    // Configure server address
    printf("Configuring Server address.....\n");
    sleep(1);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);

    printf("Done Configuring Socket.....\n");
    sleep(1);
        int option;
        printf("\n1. Register a new user\n");
        printf("2. View existing registrations\n");
        printf("3. Exit\n");
        printf("Enter an option: ");
        scanf("%d", &option);

        switch(option) {
            case 1:
                printf("Enter name: ");
                scanf("%s", name);
                printf("Enter serial number: ");
                scanf("%s", serial);
                printf("Enter registration number: ");
                scanf("%s", reg);

                sprintf(buffer, "%d %s %s %s\n", option, name, serial, reg);
                sendto(sockfd, buffer, sizeof(buffer), 0,(const struct sockaddr *)&serverAddr, sizeof(serverAddr));
                strcpy(buffer," ");
                recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
                printf("%s\n",buffer);
                if(strcmp(buffer,"Error")==0) {exit(1);}
                break;
            case 2: 
                sprintf(buffer, "%d\n", option);
                sendto(sockfd, buffer, sizeof(buffer), 0,(const struct sockaddr *)&serverAddr, sizeof(serverAddr));
                strcpy(buffer," ");
                recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, 0, NULL, NULL);
                
                printf("Registrations:\n");
                printf("%s",buffer);
                
                break;
            
            case 3:
                printf("Exiting program\n");
                break;
            default:
                printf("Invalid option\n");
                break;
        }

    return 0;
}