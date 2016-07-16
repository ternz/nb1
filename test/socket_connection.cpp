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

SocketConnection::~SocketConnection() {
	if(in_pack_ != NULL) {
		delete in_pack_;
		in_pack_ = NULL;
	}
	if(out_buf_ != NULL) {
		delete out_buf_;
		out_buf_ = NULL;
	}
}

Packet* 
SocketConnection::ReleaseInPacket() {
	Packet *tmp = in_pack_;
	in_pack_ = NULL;
	return tmp;
}

Packet* 
SocketConnection::PrepareOutPacket(Packet* pack) {
	if(out_buf_ == NULL) {
		out_buf_ = new OutBuffer();
	}
	pack->Serialize(out_buf_);
}

void 
SocketConnection::ReleaseOutBuffer() {
	delete out_buf_;
	out_buf_ = NULL;
}
