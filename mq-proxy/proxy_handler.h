/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   server.h
 * Author: chenwz5
 *
 * Created on 2016年7月30日, 下午2:51
 */

#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>
#include <sys/types.h>
#include <string>
#include <arpa/inet.h>
#include "error.h"
#include "tsqueue.h"
#include "fdhandle.h"

namespace spxy {
    
class Multiplexer;

class ProxyHandler {
public:
    ProxyHandler(TSQueue<FdHandle*> que, Multiplexer* mtp);
    ~ProxyHandler();
    
    int Run(std::string& selfaddr, std::string& targaddr);
	
private:
    std::string self_ip_;
    uint16_t self_port_;
    struct sockaddr_in self_addr_;
	std::string targ_ip_;
    uint16_t targ_port_;
	struct sockaddr_in targ_addr_;
    const int ACCP_QU = 32;
	
	pthread_t tid_;
	
	bool run_;
	
	TSQueue<FdHandle*> que_;
	Multiplexer* multiplexer_;
	
	static void* threadFunc_(void* arg);
};

#endif /* SERVER_H */

