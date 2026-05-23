#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <thread>

void handle_client(int client_fd) {
    std::cout << "Ready" << std::endl;
    
    while(true) {
        std::string command;
        std::cout << "mastermind agent> ";
        std::getline(std::cin, command);

        if(command.empty()) {
            continue;
        } 
    
        if (command == "exit") {
            send(client_fd, "exit", 4, 0);
            close(client_fd);
            std::cout << "Closed" << std::endl;
            return;
        }

        ssize_t bytes_sent = send(client_fd, command.c_str(), command.length(), 0);
        if(bytes_sent <= 0){
            std::cout << "Failed to send command" << std::endl;
            close(client_fd);
            return;
        }

        char response_buffer[4096] = {0};
        ssize_t bytes_received = recv(client_fd, response_buffer, sizeof(response_buffer) - 1, 0);

        if(bytes_received <= 0) {
            std::cout << "\n disconnected" << std::endl;
            close(client_fd);
            return;
        }

        std::cout << response_buffer << std::endl;
    } 
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Error socket" << std::endl;
        return -1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        std::cerr << "Error bind" << std::endl;
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, 3) < 0) {
        std::cerr << "Listen error" << std::endl;
        close(server_fd);
        return 1;
    }

    std::cout << "server is waiting" << std::endl;

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            std::cerr << "Error client accept" << std::endl;
            continue;
        }

        std::cout << "\n[Client connected" << std::endl;
        std::thread(handle_client, client_fd).detach();
    }

    close(server_fd);
    return 0;
}