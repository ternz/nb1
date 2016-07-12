/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include "socket_connection.h"

using namespace N;

SocketConnection::SocketConnection(int fd) 
	:sockfd(fd), in_pack(NULL), out_pack(NULL) {

}

SocketConnection::~SocketConnection() {}