/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   server.h
 * Author: chenwz5
 *
 * Created on 2016年7月7日, 下午7:52
 */

#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <string>
#include <arpa/inet.h>
#include "error.h"
#include "socket_depatcher.h"

namespace N {
    
class TcpServer {
public:
    TcpServer();
    ~TcpServer();
    
    int Server(std::string endpoint);
private:
    std::string ip;
    uint16_t port;
    struct sockaddr_in sockaddr_;
    int listenfd;
    const int ACCP_QU = 32;
    SocketDepatcher *depatcher;
    
    int parseEndpoint(std::string endpoint);
    int makeSockAddr();
};
    
}

#endif /* SERVER_H */

