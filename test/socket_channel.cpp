/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <fcntl.h>
#include <sys/epoll.h>
#include "socket_channel.h"
#include "error.h"
#include "logging/logging.h"

using namespace N;

SocketChannel::SocketChannel(common::BlockingQueue<Packet*>* in_que, common::BlockingQueue<Packet*>* out_que) 
	:in_que_(in_que), out_que_(out_que) {
	epoll_fd_ = epoll_create(EPOLL_EVENTS);
	if(epoll_fd_ == -1) {
		LOG_FATAL<<"epoll_create failed: "<<errstr(errorcode::ERRNO)<<"\n";
		exit(1);
	}
}

SocketChannel::~SocketChannel() {
	close(epoll_fd_);
}

int
SocketChannel::AddConnection(int fd) {
	if(connMap_.find(fd) == connMap_.end()) {
		return errorcode::FD_EXIST;
	}
	connMap_[fd] = boost::shared_ptr<SocketConnection>(new SocketConnection(fd));
	return errorcode::OK;
}

int 
SocketChannel::RunThread() {
	pthread_attr_t attr;
	int err = pthread_attr_init(&attr);
	CHECK_ERRNO(err);
	
	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	CHECK_ERRNO(err);
	
	err = pthread_create(&tid_, &attr, handleFunc, (void*)this);
	CHECK_ERRNO(err);
	
	pthread_attr_destroy(&attr);
	return errorcode::OK;
}

int 
SocketChannel::get_id() {
	return id_;
}

int 
SocketChannel::setNonblock(int fd) {
	int val = fcntl(fd, F_GETFL, 0);
	CHECK_ERRNO(val);
	
	if(fcntl(fd, F_SETFL, val | O_NONBLOCK) < 0) {
		return errorcode::ERRNO;
	}
	return errorcode::OK;
}

void* 
SocketChannel::handleFunc(void* arg) {
	SocketChannel* th = (SocketChannel*)arg;
	//do it
}
