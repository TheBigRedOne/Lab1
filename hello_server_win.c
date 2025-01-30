#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib") // 链接 Winsock 库

#define BUFLEN 1500
#define HTTP_PORT 80

int main(void) {
    WSADATA wsaData;
    SOCKET server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int client_addr_len;
    char buf[BUFLEN];
    int flags = 0;
    int rlen;

    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // 创建 TCP/IP 套接字
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        fprintf(stderr, "Unable to create socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // 绑定到端口 80
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(HTTP_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        fprintf(stderr, "Unable to bind to port: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    // 监听连接
    if (listen(server_fd, 5) == SOCKET_ERROR) {
        fprintf(stderr, "Unable to listen for connection: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("HTTP Server running on port %d...\n", HTTP_PORT);

    // 接受连接并处理请求
    while (1) {
        client_addr_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd == INVALID_SOCKET) {
            fprintf(stderr, "Unable to accept connection: %d\n", WSAGetLastError());
            continue; // 继续接受下一个连接
        }

        printf("Client connected...\n");

        // 读取 HTTP 请求
        memset(buf, 0, BUFLEN);
        rlen = recv(client_fd, buf, BUFLEN - 1, flags);
        if (rlen > 0) {
            buf[rlen] = '\0';  // 确保是 C 字符串
            printf("Received request:\n%s\n", buf);

            // 发送 HTTP 响应
            const char* http_response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Connection: close\r\n"
                "\r\n"
                "<html><body><h1>Hello, World!</h1></body></html>\r\n";

            send(client_fd, http_response, (int)strlen(http_response), 0);
        }

        // 关闭连接
        closesocket(client_fd);
    }

    // 关闭服务器
    closesocket(server_fd);
    WSACleanup();

    return 0;
}

