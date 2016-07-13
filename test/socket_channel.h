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
#include "common/blocking_queue.h"
#include "packet.h"
#include "socket_connection.h"
#include <boost/shared_ptr.hpp>

namespace N {
class SocketChannel {
public:
	SocketChannel(common::BlockingQueue<Packet*>*, common::BlockingQueue<Packet*>*);
	~SocketChannel();
	int AddConnection(int fd);
    int get_id();
	int RunThread();
private:
    int id_;
	std::map<int, boost::shared_ptr<SocketConnection> > connMap_;
	common::BlockingQueue<Packet*>* in_que_;
	common::BlockingQueue<Packet*>* out_que_;
	int epoll_fd_;
	const int EPOLL_EVENTS = 100;
	
	pthread_t tid_;
	
	int setNonblock(int fd);
	
	static handleFunc(void* arg);
};
}

#endif /* SOCKET_CHANNEL_H */

