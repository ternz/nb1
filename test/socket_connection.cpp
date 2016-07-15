/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include "socket_connection.h"

using namespace N;

SocketConnection::SocketConnection(int fd) 
	:sockfd_(fd), in_pack_(NULL), out_buf_(NULL) {

}

SocketConnection::~SocketConnection() {}

Packet* 
	SocketConnection::ReleaseInPacket() {
	Packet *tmp = in_pack_;
	in_pack_ = NULL;
	return tmp;
}

