/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <iostream>
#include "multiplexer.h"
#include "error.h"

using namespace std;
using namespace spxy;

Multiplexer::Multiplexer()
	:fd_(-1), ev_buf_(NULL) {
	int ret = Init();
	if(ret != errcode::OK) {
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

int  
Multiplexer::Init() {
	fd_ = epoll_create(0);
	CHECK_ERRNO(fd);
	ev_buf_ = new struct epoll_event[MAX_EV];
	return 0;
}

int 
Multiplexer::Add(int fd, int flag, void* data) {
	struct epoll_event ev;
	ev.events = flag;
	ev.data.ptr = data;
	
	int ret;
	ret = epoll_ctl(fd_, CTL_ADD, fd, &ev);
	CHECK_ERRNO(ret);
	return 0;
}

int Multiplexer::Remove(int fd) {
	int ret;
	ret = epoll_ctl(fd_, CTL_DEL, fd, NULL);
	CHECK_ERRNO(ret);
	return 0;
}

int 
Multiplexer::Wait(int timeout_ms) {
	return epoll_wait(fd_, ev_buf_, MAX_EV, timeout_ms);
}