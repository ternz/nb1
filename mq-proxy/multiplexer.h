/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Multiplexer.h
 * Author: chenwz5
 *
 * Created on 2016年7月30日, 上午10:41
 */

#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H
#include <sys/types.h>
#include <sys/epoll.h>

namespace spxy {

class Multiplexer {
public:
	static const int IO_READ = EPOLLIN;
	static const int IO_WRITE = EPOLLOUT;
	static const int IO_ERR = EPOLLERR;
	static const int IO_ONESHOT = EPOLLONESHOT;
	
	Multiplexer();
	~Multiplexer();
	int Init();
	int Add(int fd, int flag, void* data);
	int Remove(int fd);
	int Wait(int timeout_ms);
	struct epoll_event* get_ev(int index) {
		return &ev_buf_[index];
	}
private:
	int fd_;
	const int MAX_EV = 1024;
	struct epoll_event *ev_buf_;
	
	static const int CTL_ADD = EPOLL_CTL_ADD;
	static const int CTL_MOD = EPOLL_CTL_MOD;
	static const int CTL_DEL = EPOLL_CTL_DEL;
};

}

#endif /* MULTIPLEXER_H */

