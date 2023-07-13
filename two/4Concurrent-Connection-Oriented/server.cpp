#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

void handle_client(int c_socket) {
    // Read messages from the client
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(c_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            std::cerr << "Failed to receive data from client\n";
            break;
        } else if (bytes_received == 0) {
            std::cout << "Client disconnected\n";
            break;
        }

        // Parse the message
        std::istringstream message(buffer);
        float num1, num2;
        char op;
        message >> num1 >> num2 >> op;

        // Perform the calculation
        float result;
        switch (op) {
            case '+':
                result = num1 + num2;
                break;
            case '-':
                result = num1 - num2;
                break;
            case '*':
                result = num1 * num2;
                break;
            case '/':
                result = num1 / num2;
                break;
            default:
                std::cerr << "Invalid operator: " << op << '\n';
                continue;
        }

        std::string resultString = std::to_string(result);

        // Send the result back to the client
        const char* resultBuffer = resultString.c_str();
        int bytes_sent = send(c_socket, resultBuffer, resultString.length(), 0);
        if (bytes_sent == -1) {
            std::cerr << "Failed to send data to client\n";
            break;
        }
    }

    // Close the socket
    close(c_socket);
}

int main() {
    // Create a socket
    std::cout << "Creating Socket...\n" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    int s_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (s_socket == -1) {
        std::cerr << "Failed to create socket\n";
        return 1;
    }else{std::cout << "Socket created...\n" << std::endl;}

    // Bind the socket to an IP address and port
    std::cout << "Binding Socket...\n" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    sockaddr_in s_address;
    s_address.sin_family = AF_INET;
    s_address.sin_port = htons(4444);
    s_address.sin_addr.s_addr = INADDR_ANY;
    if (bind(s_socket, (sockaddr*)&s_address, sizeof(s_address)) == -1) {
        std::cerr << "Failed to bind socket\n";
        return 1;
    }else{std::cout << "Binding socket complete...\n" << std::endl;}

    // Display the port the server is running on
    std::cout << "Server running on port " << ntohs(s_address.sin_port) << '\n';

    // Listen for incoming connections
    if (listen(s_socket, 10) == -1) {
        std::cerr << "Failed to listen on socket\n";
        return 1;
    }

    while (true) {
        // Accept an incoming connection
        sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        int c_socket = accept(s_socket, (sockaddr*)&client_address, &client_address_size);
        if (c_socket == -1) {
            std::cerr << "Failed to accept connection\n";
            continue;
        }

        // Log a message when a client connects
        std::cout << "Client connected\n";

        // Fork a new process to handle the client
        pid_t pid = fork();
        if (pid == -1) {
            std::cerr << "Failed to fork\n";
            return 1;
        } else if (pid == 0) {
            // Child process
            handle_client(c_socket);
            exit(0);
        } else {
            // Parent process
            close(c_socket);
        }
    }
}
