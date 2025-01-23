#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib") // 链接 Winsock 库

#define BUFLEN 1500

#ifdef SERVER

int main(void) {
    WSADATA wsaData;
    SOCKET fd, conn_fd;
    struct sockaddr_in addr, conn_addr;
    int conn_addr_len;
    char buf[BUFLEN];
    int flags = 0;
    int rlen;

    // 初始化 Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        return 1;
    }

    // 创建 TCP/IP 套接字
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == INVALID_SOCKET) {
        perror("Unable to create socket");
        WSACleanup();
        return 1;
    }

    // 绑定到端口 5001
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5001);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR) {
        perror("Unable to bind to port");
        closesocket(fd);
        WSACleanup();
        return 1;
    }

    // 监听连接
    if (listen(fd, 1) == SOCKET_ERROR) {
        perror("Unable to listen for connection");
        closesocket(fd);
        WSACleanup();
        return 1;
    }

    // 接受第一个连接
    conn_addr_len = sizeof(conn_addr);
    conn_fd = accept(fd, (struct sockaddr*)&conn_addr, &conn_addr_len);
    if (conn_fd == INVALID_SOCKET) {
        perror("Unable to accept connection");
        closesocket(fd);
        WSACleanup();
        return 1;
    }

    // 接收数据
    while ((rlen = recv(conn_fd, buf, BUFLEN, flags)) > 0) {
        for (int i = 0; i < rlen; i++) {
            printf("%c", buf[i]);
        }
        printf("\n");
    }

    // 关闭连接和清理
    closesocket(conn_fd);
    closesocket(fd);
    WSACleanup();

    return 0;
}

#endif // SERVER
