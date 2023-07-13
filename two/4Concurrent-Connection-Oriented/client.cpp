#include <iostream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <thread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // Create a socket
    std::cout << "Creating Socket...\n" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    int c_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (c_socket == -1) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }else{std::cout << "Socket created...\n" << std::endl;}

    // Connect to the server
    std::cout << "Connecting to server...\n" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    sockaddr_in s_address;
    s_address.sin_family = AF_INET;
    s_address.sin_port = htons(4444);
    // Replace "SERVER_IP_ADDRESS" with the IP address of the server computer
    inet_pton(AF_INET, "127.0.0.1", &s_address.sin_addr);
    if (connect(c_socket, (sockaddr*)&s_address, sizeof(s_address)) == -1) {
        std::cerr << "Failed to connect to server\n";
        return 1;
    }else{std::cout << "Connected...\n" << std::endl;}

    // Send messages to the server
    std::string message;
    char buffer[1024];
    while (true) {
        // Prompt the user to enter two numbers
        float num1, num2;
        std::cout << "Enter two numbers:\n";
        std::cin >> num1 >> num2;

        // Prompt the user to select an operator
        char op;
        std::cout << "Select an operator\n";
        std::cout << "1. Addition (+)\n";
        std::cout << "2. Subtraction (-)\n";
        std::cout << "3. Multiplication (*)\n";
        int choice;
        std::cin >> choice;
        switch (choice) {
            case 1:
                op = '+';
                break;
            case 2:
                op = '-';
                break;
            case 3:
                op = '*';
                break;
            case 4:
                op = '/';
                break;
            default:
                std::cerr << "Invalid choice\n";
                continue;
        }

        // Send the message to the server
        message = std::to_string(num1) + ' ' + std::to_string(num2) + ' ' + op + '\n';
        send(c_socket, message.c_str(), message.length(), 0);

        // Read the result from the server
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(c_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            std::cerr << "Failed to receive data from server\n";
            break;
        } else if (bytes_received == 0) {
            std::cout << "Server disconnected\n";
            break;
        }

        // Display the result
        int result = std::stof(buffer);
        std::cout << "Result of operation: " << result << '\n';

        // Prompt the user to perform another operation or exit
        std::cout << "Choose next action:\n";
        std::cout << "1. Perform another operation\n";
        std::cout << "2. Exit the program\n";
        int option;
        std::cin >> option;
        if (option == 2) {
            break;
        }
    }

    // Close the socket
    close(c_socket);

    return 0;
}
