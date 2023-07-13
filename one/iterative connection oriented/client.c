#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>   
#include <unistd.h>

#define PORT 8080

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Creating socket file descriptor
    printf("Creating socket........\n");
    sleep(1);
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Socket creation error\n");
        return -1;
    }else{printf("Socket creation error\n");}

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    printf("Converting IPv4 and IPv6 addresses from text to binary form");
    sleep(1);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }else{printf("Converted\n");}

    printf("Connecting...\n");
    sleep(1);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }else{printf("Connected to server\n");}

    // Main menu
        int option;
        char name[20];
        char reg[20];
        char serial[20];
        printf("\n1. Register a new user\n");
        printf("2. View existing registrations\n");
        printf("3. Exit\n");
        printf("Enter an option: ");
        scanf("%d", &option);

        switch(option) {
            case 1: // Register a new user
                printf("Enter name: ");
                scanf("%s", name);
                printf("Enter serial number: ");
                scanf("%s", serial);
                printf("Enter registration number: ");
                scanf("%s", reg);
                sprintf(buffer, "%d %s %s %s\n", option, name, serial, reg);
                send(sock, buffer, sizeof(buffer), 0);
                strcpy(buffer," ");
                read(sock, buffer, 1024);
                printf("%s\n",buffer);
                if(strcmp(buffer,"error")==0) {exit(1);}
                break;
            case 2: // View existing registrations
                sprintf(buffer, "%d\n", option);
                send(sock, buffer, sizeof(buffer), 0);
                strcpy(buffer," ");
                valread = read(sock, buffer, 1024);
                printf("Registrations:\n");
                printf("%s",buffer);
                
                break;
            
            case 3: // Exit
                printf("Exiting program\n");
                break;
            default:
                printf("Invalid option\n");
                break;
        }

close(sock);
return 0;
}
