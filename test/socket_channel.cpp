/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "socket_channel.h"
#include "error.h"

using namespace N;

SocketChannel::SocketChannel(common::BlockingQueue<Packet*>* in_que, common::BlockingQueue<Packet*>* out_que) 
	:in_que(in_que), out_que(out_que) {
	
}

SocketChannel::~SocketChannel() {
	
}

int
SocketChannel::AddConnection(int fd) {
	if(connMap.find(fd) == connMap.end()) {
		return errorcode::FD_EXIST;
	}
	connMap[fd] = boost::shared_ptr<SocketConnection>(new SocketConnection(fd));
	return errorcode::OK;
}

int 
SocketChannel::get_id() {
	return id;
}
