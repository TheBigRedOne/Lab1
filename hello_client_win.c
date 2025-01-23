#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma comment(lib, "Ws2_32.lib") // 链接 Winsock 库

#define BUFLEN 1500

#ifdef CLIENT

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    SOCKET fd;
    struct addrinfo hints, * ai, * ai0;
    char buf[BUFLEN];
    int wlen;

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

    if (getaddrinfo(argv[1], "5001", &hints, &ai0) != 0) {
        fprintf(stderr, "Unable to resolve hostname: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // 尝试连接
    for (ai = ai0; ai != NULL; ai = ai->ai_next) {
        fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
        if (fd == INVALID_SOCKET) {
            fprintf(stderr, "Unable to create socket: %d\n", WSAGetLastError());
            continue;
        }

        if (connect(fd, ai->ai_addr, (int)ai->ai_addrlen) == SOCKET_ERROR) {
            fprintf(stderr, "Unable to connect: %d\n", WSAGetLastError());
            closesocket(fd);
            continue;
        }

        // 发送数据
        while (1) {
            snprintf(buf, BUFLEN, "Hello, world!");
            wlen = (int)send(fd, buf, (int)strlen(buf), 0); // 修复警告
            if (wlen == SOCKET_ERROR) {
                fprintf(stderr, "Unable to send data: %d\n", WSAGetLastError());
                closesocket(fd);
                freeaddrinfo(ai0);
                WSACleanup();
                return 1;
            }
            printf("Data sent successfully\n");
            Sleep(1000); // 每秒发送一次
        }

        // 关闭连接
        closesocket(fd);
        freeaddrinfo(ai0);
        WSACleanup();
        return 0;
    }

    // 无法连接
    printf("Unable to connect\n");
    freeaddrinfo(ai0);
    WSACleanup();
    return 1;
}

#endif // CLIENT
