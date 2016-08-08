/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <string>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <iostream>

#include "proxy_handler.h"
#include "error.h"
#include "multiplexer.h"
#include "logger.h"
#include "helper.h"
#include "fdhandle.h"

using namespace std;
using namespace spxy;

ProxyHandler::ProxyHandler(TSQueue<FdHandle*> *que, Multiplexer *mtp)
	:que_(que), multiplexer_(mtp), run_(false)
{
    
}

ProxyHandler::~ProxyHandler() {
    
}



errcode 
ProxyHandler::Run(string& selfaddr, string& targaddr) {
	if(run_) return ERR_OK;
	
    int ret;
    ret = Helper::ParseEndpoint(selfaddr, &self_ip_, &self_port_);
    CHECK_NOTOK(errcode(ret));
	
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
	lst_fh->type = Lstsock;
	ret = multiplexer_->Add(listenfd, MTP_EV_READ, lst_fh);
	CHECK_ERRNO(ret);
	
	//target addr
	
	ret = Helper::ParseEndpoint(targaddr, &broker_ip_, &broker_port_);
    CHECK_NOTOK(errcode(ret));
	
    Helper::MakeSockAddr(broker_ip_, broker_port_, &broker_addr_);
	
	//create & run thread
	//pthread_attr_t attr;
	//ret = pthread_attr_init(&attr);
	//CHECK_ERRNO(ret);
	
	//ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	//CHECK_ERRNO(ret);
	
	ret = pthread_create(&tid_, /*&attr*/NULL, threadFunc_, (void*)this);
	CHECK_ERRNO(ret);
	
	//pthread_attr_destroy(&attr);
	return ERR_OK;
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
				LOG_ERROR<<errstr(ERR_ERRNO)<<"\n";
				exit(ERR_ERRNO);
			}
		} 
		for(int i=0; i<ready_fds; ++i) {
			struct epoll_event *ev = th->multiplexer_->get_ev(i);
			FdHandle *fh = (FdHandle*)ev->data.ptr;
			if(fh->type == Lstsock && (ev->events & MTP_EV_READ)) {
				LOG_DEBUG<<"new connection coming, accept it\n";
				//accept connection, ignore peer socket address
				//TODO: while accept
				//while(1) {
					int new_conn = accept(fh->fd, NULL, NULL);
					if(new_conn < 0) {
						if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno == ECONNABORTED) 
							//break;
							continue;
						LOG_ERROR<<"socket accept error: "<<errstr(ERR_ERRNO)<<"\n";
						//exit(ERRNO);
						//break;
						continue;
					}
					FdHandle* ps_fh = FHCreate();
					ps_fh->fd = new_conn;
					ps_fh->type = Cmnsock_S;

					//proxy connect to broker as a client
					int cfd = socket(AF_INET, SOCK_STREAM, 0);
					if(cfd < 0) {
						LOG_ERROR<<"create socket error: "<<errstr(ERR_ERRNO)<<"\n";
						FHDestory(ps_fh);
						continue;
					}
					int err = Helper::SetNonblock(cfd);
					if(err < 0) {
						LOG_ERROR<<"fd set nonblock error: "<<errstr(ERR_ERRNO)<<"\n";
						FHDestory(ps_fh);
						continue;
					}
					err = connect(cfd, (sockaddr*)&(th->broker_addr_), sizeof(th->broker_addr_));
					if(err < 0) {
						if(errno != EINPROGRESS) {
							LOG_ERROR<<"fd set nonblock error: "<<errstr(ERR_ERRNO)<<"\n";
							close(cfd);
							FHDestory(ps_fh);
							continue;
						}
					}
					FdHandle* pc_fh = FHCreate();
					pc_fh->fd = cfd;
					pc_fh->type = Cmnsock_C;

					ps_fh->partner = pc_fh;
					pc_fh->partner = ps_fh;

					if(0 == err) {
						//连接已完成,注册与client端连接socket可读事件
						LOG_DEBUG<<"connection establish at once\n";
						th->multiplexer_->Add(ps_fh->fd, MTP_EV_READ | MTP_EV_ONESHOT, ps_fh);
					} else {
						//LOG_DEBUG<<pc_fh->fd<<" is connecting\n";
						pc_fh->state = Connecting;
						//注册与broker端连接socket可读可写事件
						th->multiplexer_->Add(pc_fh->fd, 
							MTP_EV_READ | MTP_EV_WRITE | MTP_EV_ONESHOT, pc_fh);
					}

					/*TODO
					 * 这里使用connect自带的超时，超时时间过长。以后实现自定义超时时间
					 */
				//} //end of while(1)
			} else if(fh->state == Connecting) {
				LOG_DEBUG<<"conecting "<<fh->fd<<"\n";
				if(ev->events & MTP_EV_READ || ev->events & MTP_EV_WRITE) {
					int error;
					socklen_t len = sizeof(error);
					if(getsockopt(fh->fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0 || error != 0) {
						if(error !=0) errno = error;
						LOG_ERROR<<"connect to broker error: "<<strerror(errno)<<"\n";
						FHDestory(fh->partner);
						FHDestory(fh);
					} else {
						LOG_DEBUG<<fh->fd<<" connection ok\n";
						//connection ok
						th->multiplexer_->Add(fh->partner->fd, MTP_EV_READ | MTP_EV_ONESHOT, fh->partner);
						th->multiplexer_->Modify(fh->fd, MTP_EV_READ | MTP_EV_ONESHOT, fh);
						//state: Conneting -> None
						fh->state = S_None;
					}
				} else {
					//不太可能执行到这里
					LOG_ERROR<<"1?unknown error\n";
					FHDestory(fh->partner);
					FHDestory(fh);
				}
			} else if(ev->events & (MTP_EV_READ | MTP_EV_WRITE)) {
				LOG_DEBUG<<fh->fd<<" is readable and writable\n";
				//if(fh->state == S_None)
				fh->state = ReadWrite;
				th->que_->Push(fh);
			} else if(ev->events & MTP_EV_READ) {
				LOG_DEBUG<<fh->fd<<" is readable\n";
				//if(fh->state == S_None)
				fh->state = Readable;
				th->que_->Push(fh);
			} else if(ev->events & MTP_EV_WRITE) {
				LOG_DEBUG<<fh->fd<<" is writable\n";
				//if(fh->state == S_None)
				fh->state = Writable;
				th->que_->Push(fh);
			} else if(ev->events & MTP_EV_ERR) {
				LOG_ERROR<<"MTP_IO_ERR occur\n";
				FHDestory(fh);
			} else {
				LOG_ERROR<<"undefind handler for event"<<ev->events<<"\n";
				FHDestory(fh);
			}
		}
	}
}

void ProxyHandler::Wait() {
	int ret = pthread_join(tid_, NULL);
	if(ret < 0) {
		LOG_ERROR<<"pthread_join error: "<<errstr(ERR_ERRNO)<<"\n";
	}
}

