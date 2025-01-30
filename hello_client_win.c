#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#pragma comment(lib, "Ws2_32.lib") // 链接 Winsock 库

#define BUFLEN 4096  // 缓冲区大小
#define HTTP_PORT "80"  // HTTP 端口

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    SOCKET fd;
    struct addrinfo hints, * ai, * ai0;
    char buf[BUFLEN];
    int rlen;
    struct timespec before_connect, after_connect;
    
    if (argc != 2) {
        printf("Usage: %s <hostname>\n", argv[0]);
        return 1;
    }

    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }

    // 配置 addrinfo
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(argv[1], HTTP_PORT, &hints, &ai0) != 0) {
        fprintf(stderr, "Unable to resolve hostname: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // 连接服务器
    for (ai = ai0; ai != NULL; ai = ai->ai_next) {
        fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (fd == INVALID_SOCKET) {
            fprintf(stderr, "Unable to create socket: %d\n", WSAGetLastError());
            continue;
        }

        // 记录 connect() 前的时间
        clock_gettime(CLOCK_MONOTONIC, &before_connect);

        if (connect(fd, ai->ai_addr, (int)ai->ai_addrlen) == SOCKET_ERROR) {
            fprintf(stderr, "Unable to connect: %d\n", WSAGetLastError());
            closesocket(fd);
            continue;
        }

        // 记录 connect() 后的时间
        clock_gettime(CLOCK_MONOTONIC, &after_connect);

        // 计算并打印 TCP 连接时间
        double connect_time = (after_connect.tv_sec - before_connect.tv_sec) +
                              (after_connect.tv_nsec - before_connect.tv_nsec) / 1e9;
        printf("Connected to %s in %.6f seconds\n", argv[1], connect_time);

        // 发送 HTTP 请求
        snprintf(buf, BUFLEN, "GET / HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n", argv[1]);
        if (send(fd, buf, (int)strlen(buf), 0) == SOCKET_ERROR) {
            fprintf(stderr, "Unable to send request: %d\n", WSAGetLastError());
            closesocket(fd);
            WSACleanup();
            return 1;
        }

        // 接收 HTTP 响应
        printf("\n--- HTTP RESPONSE START ---\n");
        while ((rlen = recv(fd, buf, BUFLEN - 1, 0)) > 0) {
            buf[rlen] = '\0';  // 确保字符串终止
            printf("%s", buf);
        }
        printf("\n--- HTTP RESPONSE END ---\n");

        // 关闭连接
        closesocket(fd);
        freeaddrinfo(ai0);
        WSACleanup();
        return 0;
    }

    printf("Unable to connect\n");
    freeaddrinfo(ai0);
    WSACleanup();
    return 1;
}
