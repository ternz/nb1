/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   proxy_handler.h
 * Author: chenwz5
 *
 * Created on 2016年8月9日, 下午5:41
 */

#ifndef PROXY_HANDLER_H
#define PROXY_HANDLER_H

#include <pthread.h>
#include <sys/types.h>
#include <string>
#include <arpa/inet.h>
#include <queue>
#include <vector>

#include "error.h"
#include "tsqueue.h"
#include "fdhandle.h"
#include "multiplexer.h"
#include "ptrbuf.h"

#define ACCP_QU 32
#define 

class ProxyHandler {
public:
    ProxyHandler();
    ~ProxyHandler();
    
    errcode Init(std::string& selfaddr, std::string& targaddr);
	void Loop();
	errcode AddPipe(int pipe);
	
private:
    std::string self_ip_;
    uint16_t self_port_;
    struct sockaddr_in self_addr_;
	std::string broker_ip_;
    uint16_t broker_port_;
	struct sockaddr_in broker_addr_;
    
	std::vector<int> w_pipes_;
	std::vector<PtrBuf*> ptrbufs_;
	std::queue<void*> ptr_que_;
	
	bool run_;
	
	Multiplexer multiplexer_;
	
	//static void* threadFunc_(void* arg);
	size_t chosePipe(void* ptr) {
		return uint64_t(ptr) % w_pipes.size();
	}
};



#endif /* PROXY_HANDLER_H */

