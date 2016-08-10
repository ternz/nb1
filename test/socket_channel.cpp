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
		LOG_FATAL<<"epoll_create failed: "<<errstr(errcode::ERR_ERRNO)<<"\n";
		exit(1);
	}
}

SocketChannel::~SocketChannel() {
	close(epollfd_);
}

int
SocketChannel::AddConnection(int fd) {
	if(connMap_.find(fd) == connMap_.end()) {
		return errcode::ERR_FD_EXIST;
	}
	connMap_[fd] = new SocketConnection(fd);
	int err = epollAdd(fd, EPOLLONESHOT | EPOLLIN);
	return err;
}

int 
SocketChannel::RemoveConnection(int fd) {
	auto iter = connMap_.find(fd);
	int err = errcode::ERR_OK;
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
	return errcode::ERR_OK;
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
		return errcode::ERR_ERRNO;
	}
	return errcode::ERR_OK;
}

void* 
SocketChannel::handleFunc(void* arg) {
	SocketChannel* th = (SocketChannel*)arg;
	//do it
	while(1) {
		int evno = epoll_wait(th->epollfd_, th->epollev_, th->EPOLL_EVENTS, -1);
		if(evno < 0) {
			LOG_ERROR<<"epoll_wait return errno "<<errno<<": "<<errstr(errcode::ERR_ERRNO)<<"\n";
			continue;
		}
		for(int i=0; i < evno; i++) {
			SocketConnection* conn = (SocketConnection*)th->epollev_[i].data.ptr;
			if(th->epollev_[i].events & EPOLLIN) {
				//do read
				if(conn.in_pack_ == NULL) {
					conn.in_pack_ = new Packet();
				}
				int state = th->protocol.ReadPacket(conn.sockfd_, conn.in_pack_);
				switch(state) {
					case IOState::HeaderErr: {
						LOG_ERROR<<"header error "<<conn.in_pack_.header.val<<"\n";
						//TODO response a error message
						th->clearSock(conn.sockfd_);
						break;
					}
					case IOState::Errno: {
						LOG_ERROR<<"read error "<<errstr(errcode::ERR_ERRNO)<<"\n";
						//TODO response a error message
						th->clearSock(conn.sockfd_);
						break;
					}
					case IOState::Off: {
						th->clearSock(conn.sockfd_);
						break;
					}
					case IOState::Again: {
						if(epollAdd(conn.sockfd_, EPOLLIN) < 0) {
							LOG_ERROR<<"epoll add error "<<errstr(errcode::ERR_ERRNO)<<"\n"; 
							th->clearSock(conn.sockfd_);
						}
						break;
					}
					case IOState::Done: {
						//Q? epoll add
						th->in_que_->Push(conn.ReleaseInPacket());
						break;
					}
				}
				
			}
			if(th->epollev_[i].events & EPOLLOUT) {
				//do write
				int state = th->protocol.WritePacket(conn.sockfd_, conn.out_buf_);
				switch(state) {
					case IOState::Errno: {
						LOG_ERROR<<"read error "<<errstr(errcode::ERR_ERRNO)<<"\n";
						//TODO response a error message
						th->clearSock(conn.sockfd_);
						break;
					}
					case IOState::Again: {
						if(epollAdd(conn.sockfd_, EPOLLOUT) < 0) {
							LOG_ERROR<<"epoll add error "<<errstr(errcode::ERR_ERRNO)<<"\n"; 
							th->clearSock(conn.sockfd_);
						}
						break;
					}
					case IOState::Done: {
						if(epollAdd(conn.sockfd_, EPOLLIN) < 0) {
							LOG_ERROR<<"epoll add error "<<errstr(errcode::ERR_ERRNO)<<"\n"; 
							th->clearSock(conn.sockfd_);
						}
						break;
					}
				}
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
	return errcode::ERR_OK;
}

int SocketChannel::epollRemove(int fd) {
	int err = epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, NULL);
	if(err < 0) {
		if(errno != ENOENT) {
			return errcode::ERR_ERRNO;
		}
	}
	return errcode::ERR_OK;
}