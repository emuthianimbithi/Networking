#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

static FILE *database;
#define PORT 8080

void handle_request(int sock);

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create a socket for the server
    printf("Creating socket\n");
    sleep(2);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }else{printf("Socket created\n"); sleep(2);}

    // Set socket options to allow reuse of address and port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to a port and address
    printf("Binding socket to port %d\n",PORT);
    sleep(2);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }else{printf("Socket binding complete\n");sleep(2);}

    // Listen for incoming connections
    printf("Listening for incoming connections\n");
    sleep(2);
    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }else{sleep(1);}

    // Accept incoming connections and handle them
    while (1)
    {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Connected to client\n");
        handle_request(new_socket);

        close(new_socket);
    }

    return 0;
}

void handle_request(int sock)
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
            while(fgets(temp,50,database)){
            	strcpy(line,temp);
            	strcat(line,"\n");
            	strcat(result,line);
            }
            send(sock, result, strlen(result), 0);
            strcpy(result,"");
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
        
        while(fgets(temp,100,database)!=NULL){
        	char *token = strtok(temp," ");
        	int count = 0;
        	while(token != NULL){
        	
        		if (count== 2){
        			if(strstr(token,reg)!=NULL){
				index++;
				printf("%d",index);
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
        }else{
        	send(sock, "Registration number in use!", strlen("Registration number in use!"), 0);
        	printf("Registration failed!!");
        }
    
    }else
    {
        send(sock, "unknown command", strlen("unknown command"), 0);
    }
}
