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

#include "error.h"

namespace spxy {

#define MTP_EV_READ		EPOLLIN
#define MTP_EV_WRITE	EPOLLOUT
#define MTP_EV_ERR		EPOLLERR
#define MTP_EV_ONESHOT  EPOLLONESHOT
#define MTP_EV_ET		EPOLLET

#define MAX_EV 1024

class Multiplexer {
public:
	Multiplexer();
	~Multiplexer();
	errcode Init();
	errcode Add(int fd, int flag, void* data);
	errcode Remove(int fd);
	errcode Modify(int fd, int flag, void* data);
	int Wait(int timeout_ms);
	struct epoll_event* get_ev(int index) {
		return &ev_buf_[index];
	}
private:
	int fd_;

	struct epoll_event *ev_buf_;
	errcode ctl(int op, int fd, int flag, void* data);
};

}

#endif /* MULTIPLEXER_H */

