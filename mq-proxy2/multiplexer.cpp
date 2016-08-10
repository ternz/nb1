/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include "multiplexer.h"
#include "error.h"

using namespace std;

Multiplexer::Multiplexer()
	:fd_(-1), ev_buf_(NULL) {
	errcode ret = Init();
	if(ret != ERR_OK) {
		cerr<<errstr(ret)<<endl;
		exit(ret);
	}
}

Multiplexer::~Multiplexer() {
	if(fd_ != -1) 
		close(fd_);
	if(ev_buf_ != NULL) 
		delete[] ev_buf_;
}

errcode  
Multiplexer::Init() {
	fd_ = epoll_create(MAX_EV);
	CHECK_ERRNO(fd_);
	ev_buf_ = new struct epoll_event[MAX_EV];
	return ERR_OK;
}

errcode 
Multiplexer::Add(int fd, int flag, void* data) {
	return ctl(EPOLL_CTL_ADD, fd, flag, data);
}

errcode Multiplexer::Remove(int fd) {
	return ctl(EPOLL_CTL_DEL, fd, 0, NULL);
}

errcode 
Multiplexer::Modify(int fd, int flag, void* data) {
	return ctl(EPOLL_CTL_MOD, fd, flag, data);
}

errcode 
Multiplexer::ctl(int op, int fd, int flag, void* data) {
	struct epoll_event ev;
	ev.events = flag;
	ev.data.ptr = data;
	
	struct epoll_event *pev = &ev;
	if(op == EPOLL_CTL_DEL) pev = NULL;
	
	int ret;
	ret = epoll_ctl(fd_, op, fd, pev);
	CHECK_ERRNO(ret);
	return ERR_OK;
}

int 
Multiplexer::Wait(int timeout_ms) {
	return epoll_wait(fd_, ev_buf_, MAX_EV, timeout_ms);
}
