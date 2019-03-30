#ifndef UDP_SERVER_H_
#define UDP_SERVER_H_

#include "Utils.h"

#define BUFFER_SIZE 1316
#define SERVER 0
#define CLIENT 1

class UdpSocket {

public:

    UdpSocket(std::string ip_, const unsigned int port_, char type);

    void ReadDatagrama(uint8_t *buf, size_t &size, int bufsize);

    void SendDatagrama(uint8_t *buf, size_t &size, int bufsize);

private:

    std::string ip;
    unsigned int port, sock, length;

    struct sockaddr_in server;
    socklen_t fromlen;

    char buffer[BUFFER_SIZE] = {0};
    
};

#endif //UDP_SERVER_H_