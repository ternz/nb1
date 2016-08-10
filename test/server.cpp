/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <string>
#include <cstring>
#include <stdlib.h>
#include <sys/socket.h>
#include <boost/shared_ptr.hpp>

#include "server.h"
#include "logging/logging.h"
#include "error.h"
#include "simple_collector.h"

using namespace std;
using namespace N;

TcpServer::TcpServer() {
    depatcher = new SocketDepatcher(boost::shared_ptr<SimpleCollector>(new SimpleCollector()));
}

TcpServer::~TcpServer() {
    delete depatcher;
}

int 
TcpServer::parseEndpoint(string endpoint) {
    size_t n = endpoint.find(":");
    if(n == string::npos) {
        return errcode::ERR_NET_ADDR;
    }
    this->ip = endpoint.substr(0, n);
    string port_str = endpoint.substr(n, string::npos);
    this->port = uint16_t(atoi(port_str.c_str()));
    return errcode::ERR_OK;
}

int 
TcpServer::makeSockAddr() {
    memset(sockaddr_, 0, sizeof(sockaddr_));
    sockaddr_.sin_family = AF_INET;
    sockaddr_.sin_port = htons(port);
    sockaddr_.sin_addr.s_addr = inet_addr(ip.c_str());
    return errcode::ERR_OK;
}

int 
TcpServer::Server(string endpoint) {
    int ret = 0;
    ret = parseEndpoint(endpoint);
    CHECK_NOTOK(ret);
    
    makeSockAddr();
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    CHECK_ERRNO(listenfd);
    
    ret = bind(listenfd, (struct sockaddr*)&sockaddr_, sizeof(sockaddr_));
    CHECK_ERRNO(ret);
    
    ret = listen(listenfd, ACCP_QU);
    CHECK_ERRNO(ret);
	
	//TODO: preserve connectoin addr
	int fd;
	while(fd = accept(listenfd, NULL, NULL)) {
		if(fd == -1) {
			return errcode::ERR_ERRNO;
		}
		depatcher->Depatch(fd);
	}
}

