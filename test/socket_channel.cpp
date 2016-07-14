/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>
#include "socket_channel.h"
#include "error.h"
#include "logging/logging.h"

using namespace N;

SocketChannel::SocketChannel(common::BlockingQueue<Packet*>* in_que, common::BlockingQueue<Packet*>* out_que) 
	:in_que_(in_que), out_que_(out_que) {
	epollfd_ = epoll_create(EPOLL_EVENTS);
	if(epollfd_ == -1) {
		LOG_FATAL<<"epoll_create failed: "<<errstr(errcode::ERRNO)<<"\n";
		exit(1);
	}
}

SocketChannel::~SocketChannel() {
	close(epollfd_);
}

int
SocketChannel::AddConnection(int fd) {
	if(connMap_.find(fd) == connMap_.end()) {
		return errcode::FD_EXIST;
	}
	connMap_[fd] = new SocketConnection(fd);
	int err = epollAdd(fd, EPOLLONESHOT | EPOLLIN);
	return err;
}

int 
SocketChannel::RemoveConnection(int fd) {
	auto iter = connMap_.find(fd);
	int err = errcode::OK;
	if(iter != connMap_.end()) {
		if(iter->second != NULL) {
			delete iter->second;
		}
		err = epollRemove(fd);
		connMap_.erase(iter);
	}
	return err;
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
	return errcode::OK;
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
		return errcode::ERRNO;
	}
	return errcode::OK;
}

void* 
SocketChannel::handleFunc(void* arg) {
	SocketChannel* th = (SocketChannel*)arg;
	//do it
	while(1) {
		int evno = epoll_wait(th->epollfd_, th->epollev_, th->EPOLL_EVENTS, -1);
		if(evno < 0) {
			LOG_ERROR<<"epoll_wait return errno "<<errno<<": "<<errstr(errcode::ERRNO)<<"\n";
			continue;
		}
		for(int i=0; i < evno; i++) {
			SocketConnection* conn = (SocketConnection*)th->epollev_[i].data.ptr;
			if(th->epollev_[i].events & EPOLLIN) {
				//do read
			}
			if(th->epollev_[i].events & EPOLLOUT) {
				//do write
			}
		}
	}
}

int 
SocketChannel::epollAdd(int fd, int flag) {
	struct epoll_event ev;
	ev.events = flag | EPOLLONESHOT;
	ev.data.ptr = (void*)connMap_[fd];
	int err = epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &ev);
	CHECK_ERRNO(err);
	return errcode::OK;
}

int SocketChannel::epollRemove(int fd) {
	int err = epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, NULL);
	if(err < 0) {
		if(errno != ENOENT) {
			return errcode::ERRNO;
		}
	}
	return errcode::OK;
}