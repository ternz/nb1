/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   simple_collector.h
 * Author: chenwz5
 *
 * Created on 2016年7月8日, 下午6:26
 */

#ifndef SIMPLE_COLLECTOR_H
#define SIMPLE_COLLECTOR_H

#include "collector.h"
#include "socket_channel.h"
#include "common/blocking_queue.h"

namespace N {
class SimpleCollector:public Collector<int, SocketChannel> {
public:
	SimpleCollector();
	virtual ~SimpleCollector();
    virtual SocketChannel* Find(int* key);
    virtual void Add(SocketChannel* sc);
    virtual void Remove(int* key);
    virtual size_t Size();
    virtual SocketChannel* GetItem();
private:
    SocketChannel* sc;
	common::BlockingQueue<Packet*> in_que;
	common::BlockingQueue<Packet*> out_que;
};
}

#endif /* SIMPLE_COLLETOR_H */

