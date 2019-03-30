#include "UdpSocket.h"

UdpSocket::UdpSocket(std::string ip_, const unsigned int port_, char type)
{
    ip = std::move(ip_);
    port = port_;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0) exit(EXIT_FAILURE);

    length = sizeof(server);
    memset(&server, 0, length);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(port);

    if(type == 0)
    {
        server.sin_addr.s_addr = INADDR_ANY;
        if(bind(sock, (struct sockaddr *) &server, length) < 0) exit(EXIT_FAILURE);
    }

    fromlen = sizeof(struct sockaddr_in);
}

void UdpSocket::ReadDatagrama(uint8_t *buf, size_t &size, int bufsize)
{
    int n = recvfrom(sock, buf, BUFFER_SIZE, 0, (struct sockaddr *) &server, &fromlen);
    size = n; 
}

void UdpSocket::SendDatagrama(uint8_t *buf, size_t &size, int bufsize)
{
    sendto(sock, buf, bufsize, MSG_DONTWAIT, (const struct sockaddr *) &server, fromlen);
    size = bufsize;
}