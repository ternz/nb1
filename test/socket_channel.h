/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   socket_channel.h
 * Author: chenwz5
 *
 * Created on 2016年7月8日, 下午6:30
 */

#ifndef SOCKET_CHANNEL_H
#define SOCKET_CHANNEL_H
#include <pthread.h>
#include <map>
#include <sys/epoll.h>
#include "common/blocking_queue.h"
#include "packet.h"
#include "socket_connection.h"
#include "protocol.h"

namespace N {
class SocketChannel {
public:
	SocketChannel(common::BlockingQueue<Packet*>*, common::BlockingQueue<Packet*>*);
	~SocketChannel();
	int AddConnection(int fd);
	int RemoveConnection(int fd);
    int get_id();
	int RunThread();
private:
    int id_;
	std::map<int, SocketConnection* > connMap_;
	common::BlockingQueue<Packet*>* in_que_;
	common::BlockingQueue<Packet*>* out_que_;
	int epollfd_;
	const int EPOLL_EVENTS = 100;
	struct epoll_event epollev_[EPOLL_EVENTS];
	
	pthread_t tid_;
	
	Protocol protocol;
	
	int setNonblock(int fd);
	
	static handleFunc(void* arg);
	
	int epollAdd(int fd, int flag);
	int epollRemove(int fd);
	
	void clearSock(int fd) {
		close(fd);
		RemoveConnection(fd);
	}
};
}

#endif /* SOCKET_CHANNEL_H */

