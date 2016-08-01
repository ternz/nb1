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
	if(run_) return;
	
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
	
	ret = Helper::ParseEndpoint(targaddr, &broker_ip_, &broker_port_);
    CHECK_NOTOK(ret);
	
    Helper::MakeSockAddr(broker_ip_, broker_port_, &broker_addr_);
	
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
				//accept connection, ignore peer socket address
				int new_conn = accept(fh->fd, NULL, NULL);
				if(new_conn < 0) {
					if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno == ECONNABORTED) 
						continue;
					LOG_ERROR<<"socket accept error: "<<errstr(errcode::ERRNO)<<"\n";
					//exit(errcode::ERRNO);
					continue;
				}
				FdHandle* ps_fh = FHCreate();
				ps_fh->fd = new_conn;
				ps_fh->type = FdType::Cmnsock_S;
				
				//proxy connect to broker as a client
				int cfd = socket(AF_INET, SOCK_STREAM, 0);
				if(cfd < 0) {
					LOG_ERROR<<"create socket error: "<<errstr(errcode::ERRNO)<<"\n";
					FHDestory(ps_fh);
					continue;
				}
				int err = Helper::SetNonblock(cfd);
				if(err < 0) {
					LOG_ERROR<<"fd set nonblock error: "<<errstr(errcode::ERRNO)<<"\n";
					FHDestory(ps_fh);
					continue;
				}
				err = connect(cfd, th->broker_addr_, sizeof(th->broker_addr_));
				if(err < 0) {
					if(errno == EINPROGRESS) continue;
					LOG_ERROR<<"fd set nonblock error: "<<errstr(errcode::ERRNO)<<"\n";
					FHDestory(ps_fh);
					continue;
				}
				FdHandle* pc_fh = FHCreate();
				pc_fh->fd = cfd;
				pc_fh->type = FdType::Cmnsock_C;
				
				ps_fh->partner = pc_fh;
				pc_fh->partner = ps_fh;
				
				if(0 == err) {
					//连接已完成,注册与client端连接socket可读事件
					th->multiplexer_->Add(ps_fh->fd, Multiplexer::IO_READ | Multiplexer::IO_ONESHOT, ps_fh);
				} else {
					pc_fh->state = IOState::Connecting;
					//注册与broker端连接socket可读可写事件
					th->multiplexer_->Add(pc_fh->fd, 
						Multiplexer::IO_READ | Multiplexer::IO_WRITE | Multiplexer::IO_ONESHOT, pc_fh);
				}
				
				/*TODO
				 * 这里使用connect自带的超时，超时时间过长。以后实现自定义超时时间
				 */
				
			} else if(fh->state == IOState::Connecting) {
				if(ev->events & Multiplexer::IO_READ || ev->events & Multiplexer::IO_WRITE) {
					int error;
					socklen_t len = sizeof(error);
					if(getsockopt(fh->fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0 || error != 0) {
						if(error !=0) errno = error;
						LOG_ERROR<<"connect to broker error: "<<strerror(errno)<<"\n";
						FHDestory(fh->partner);
						FHDestory(fh);
					} else {
						//connection ok
						th->multiplexer_->Add(fh->partner->fd, Multiplexer::IO_READ | Multiplexer::IO_ONESHOT, fh->partner);
						//state: Conneting -> None
						fh->state = IOState::None;
					}
				} else {
					//不太可能执行到这里
					LOG_ERROR<<"1?unknown error\n";
					FHDestory(fh->partner);
					FHDestory(fh);
				}
			} else if(ev->events & Multiplexer::IO_READ) {
				if(fh->state == IOState::None)
					fh->state = IOState::Readable;
				th->que_.Push(fh);
			} else if(ev->events & Multiplexer::IO_WRITE) {
				if(fh->state == IOState::None)
					fh->state = IOState::Writable;
				th->que_.Push(fh);
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
