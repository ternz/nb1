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
private:
    int id;
	std::map<int, boost::shared_ptr<SocketConnection> > connMap;
	common::BlockingQueue<Packet*>* in_que;
	common::BlockingQueue<Packet*>* out_que;
};
}

#endif /* SOCKET_CHANNEL_H */

