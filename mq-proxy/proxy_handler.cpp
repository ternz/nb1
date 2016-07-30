/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <string>
#include <cstring>
#include <stdlib.h>
#include <sys/socket.h>
#include <errno.h>
#include <boost/shared_ptr.hpp>

#include "proxy_handler.h"
#include "error.h"
#include "multiplexer.h"
#include "logger.h"

using namespace std;
using namespace spxy;

ProxyHandler::ProxyHandler(TSQueue<FdHandle*> que, Multiplexer* mtp)
	:que_(que), multiplexer_(mtp), run_(false)
{
    
}

ProxyHandler::~ProxyHandler() {
    
}



int 
ProxyHandler::Run(string& selfaddr, string& targaddr) {
    int ret = 0;
    ret = Helper::ParseEndpoint(selfaddr, &self_ip_, &self_port_);
    CHECK_NOTOK(ret);
	
    Helper::MakeSockAddr(self_ip_, self_port_, &self_addr_);
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    CHECK_ERRNO(listenfd);
	
    ret = bind(listenfd, (struct sockaddr*)&self_addr_, sizeof(self_addr_));
    CHECK_ERRNO(ret);
	
    ret = listen(listenfd, ACCP_QU);
    CHECK_ERRNO(ret);
	
	ret = Helper::SetNonblock(listenfd);
	CHECK_ERRNO(ret);
	
	FdHandle* lst_fh = FHCreate();
	lst_fh->fd = listenfd;
	lst_fh->type = FdType::Lstsock;
	ret = multiplexer_->Add(listenfd, Multiplexer::IO_READ, lst_fh);
	CHECK_ERRNO(ret);
	
	//target addr
	
	ret = Helper::ParseEndpoint(targaddr, &targ_ip_, &targ_port_);
    CHECK_NOTOK(ret);
	
    Helper::MakeSockAddr(targ_ip_, targ_port_, &targ_addr_);
	
	//create & run thread
	pthread_attr_t attr;
	ret = pthread_attr_init(&attr);
	CHECK_ERRNO(ret);
	
	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	CHECK_ERRNO(ret);
	
	ret = pthread_create(&tid_, &attr, threadFunc_, (void*)this);
	CHECK_ERRNO(ret);
	
	pthread_attr_destroy(&attr);
	return errcode::OK;
}

void* ProxyHandler::threadFunc_(void* arg) {
	ProxyHandler *th = (ProxyHandler*)arg;
	int ready_fds = 0;
	th->run_ = true;
	while(th->run_) {
		ready_fds = th->multiplexer_->Wait(1000);
		if(ready_fds == -1) {
			if(errno == EINTR) continue;
			else {
				LOG_ERROR<<errstr(errcode::ERRNO)<<"\n";
				exit(errcode::ERRNO);
			}
		} 
		for(int i=0; i<ready_fds; ++i) {
			struct epoll_event *ev = th->multiplexer_->get_ev(i);
			FdHandle *fh = (FdHandle*)ev.data.ptr;
			if(fh->type == FdType::Lstsock && (ev->events & Multiplexer::IO_READ)) {
				
			} else if(ev->events & (Multiplexer::IO_READ | Multiplexer::IO_WRITE)) {
				
			} else if(ev->events & Multiplexer::IO_ERR) {
				LOG_ERROR<<"Multiplexer::IO_ERR occur\n";
				FHDestory(fh);
			} else {
				LOG_ERROR<<"undefind handler for event"<<ev->events<<"\n";
				FHDestory(fh);
			}
		}
	}
}
