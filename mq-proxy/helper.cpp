/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <fcntl.h>
#include "helper.h"
#include "error.h"

using namespace std;
using namespace spxy;

int 
Helper::ParseEndpoint(string endpoint, string* ip, uint16_t* port) {
    size_t n = endpoint.find(":");
    if(n == string::npos) {
        return errcode::NET_ADDR;
    }
    *ip = endpoint.substr(0, n);
    string port_str = endpoint.substr(n, string::npos);
    *port = uint16_t(atoi(port_str.c_str()));
    return errcode::OK;
}

int 
Helper::MakeSockAddr(string& ip, uint16_t port, struct sockaddr_in* addr) {
    memset(addr, 0, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = inet_addr(ip.c_str());
    return errcode::OK;
}

int 
Helper::SetNonblock(int fd) {
	int val = fcntl(fd, F_GETFL, 0);
	CHECK_ERRNO(val);
	
	if(fcntl(fd, F_SETFL, val | O_NONBLOCK) < 0) {
		return errcode::ERRNO;
	}
	return errcode::OK;
}
