/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <iostream>

#include "config.h"
#include "proxy_handler.h"
#include "error.h"
#include "multiplexer.h"
#include "logger.h"
#include "helper.h"
#include "fdhandle.h"

using namespace std;

ProxyHandler::ProxyHandler()
	:run_(false)
{
    
}

ProxyHandler::~ProxyHandler() {
    
}



errcode 
ProxyHandler::Init(string& selfaddr, string& targaddr) {
	if(run_) return ERR_REFUSED;
	
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
	
	FdHandle* lst_fh = new FdHandle();
	lst_fh->set_fd(listenfd);
	lst_fh->set_type(FT_Listen);
	ret = multiplexer_.Add(listenfd, MTP_EV_READ, lst_fh);
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
	
	//ret = pthread_create(&tid_, /*&attr*/NULL, threadFunc_, (void*)this);
	//CHECK_ERRNO(ret);
	
	//pthread_attr_destroy(&attr);
	return ERR_OK;
}

errcode ProxyHandler::AddPipe(int pipe) {
	if(run_) return ERR_REFUSED;
	FdHandle *pipe_fh = new FdHandle();
	pipe_fh->set_fd(pipe);
	pipe_fh->set_type(FT_Pipe);
	w_pipes_.push_back(pipe_fh);
	//ptrbufs_.push_back(NULL);
	return multiplexer_.Add(pipe, MTP_EV_WRITE | MTP_EV_ET | MTP_EV_ONESHOT, NULL);
}

void ProxyHandler::addInPipeQue(void* ptr) {
	int index = uint64_t(ptr) % w_pipes_.size();
	w_pipes_[index]->AddOutBuffer(new PtrBuf(ptr));
}

void ProxyHandler::Loop() {
	int ready_fds = 0;
	run_ = true;
	while(run_) {
		ready_fds = multiplexer_.Wait(1000);
		if(ready_fds == -1) {
			if(errno == EINTR) continue;
			else {
				LOG_ERROR<<errstr(ERR_ERRNO)<<"\n";
				exit(ERR_ERRNO);
			}
		} 
		for(int i=0; i<ready_fds; ++i) {
			struct epoll_event *ev = multiplexer_.get_ev(i);
			if(ev->data.ptr == NULL) continue;
			
			FdHandle *fh = (FdHandle*)ev->data.ptr;
			if(fh->type() == FT_Listen && (ev->events & MTP_EV_READ)) {
				LOG_DEBUG<<"new connection coming, accept it\n";
				//accept connection, ignore peer socket address
				//TODO: while accept
				//while(1) {
					int new_conn = accept(fh->fd(), NULL, NULL);
					if(new_conn < 0) {
						if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR || errno == ECONNABORTED) 
							//break;
							continue;
						LOG_ERROR<<"socket accept error: "<<errstr(ERR_ERRNO)<<"\n";
						//exit(ERRNO);
						//break;
						continue;
					}
					
					int err;
					
					err = Helper::SetNonblock(new_conn);
					if(err < 0) {
						LOG_ERROR<<"fd "<<new_conn<<" set nonblock error: "<<errstr(ERR_ERRNO)<<"\n";
						continue;
					}
					
					FdHandle* ps_fh = new FdHandle();
					ps_fh->set_fd(new_conn);
					ps_fh->set_type(FT_Sock_S);

					//proxy connect to broker as a client
					int cfd = socket(AF_INET, SOCK_STREAM, 0);
					if(cfd < 0) {
						LOG_ERROR<<"fd "<<cfd<<" set nonblock error: "<<errstr(ERR_ERRNO)<<"\n";
						delete ps_fh;
						continue;
					}
					err = Helper::SetNonblock(cfd);
					if(err < 0) {
						LOG_ERROR<<"fd set nonblock error: "<<errstr(ERR_ERRNO)<<"\n";
						delete ps_fh;
						continue;
					}
					err = connect(cfd, (sockaddr*)&(broker_addr_), sizeof(broker_addr_));
					if(err < 0) {
						if(errno != EINPROGRESS) {
							LOG_ERROR<<"fd set nonblock error: "<<errstr(ERR_ERRNO)<<"\n";
							close(cfd);
							delete ps_fh;
							continue;
						}
					}
					FdHandle* pc_fh = new FdHandle();
					pc_fh->set_fd(cfd);
					pc_fh->set_type(FT_Sock_C);

					//互粉
					ps_fh->set_partner(pc_fh);
					pc_fh->set_partner(ps_fh);

					if(0 == err) { 
						//连接已完成,注册与client端连接socket可读事件
						LOG_DEBUG<<"connection establish at once\n";
						FHPair *pair = new FHPair(ps_fh, pc_fh);
						addInPipeQue(pair);
					} else {
						//LOG_DEBUG<<pc_fh->fd<<" is connecting\n";
						pc_fh->set_type(FT_Connecting);
						//注册与broker端连接socket可读可写事件
						multiplexer_.Add(pc_fh->fd(), 
							MTP_EV_READ | MTP_EV_WRITE | MTP_EV_ONESHOT, pc_fh);
					}

					/*TODO
					 * 这里使用connect自带的超时，超时时间过长。以后实现自定义超时时间
					 */
				//} //end of while(1)
			} else if(fh->type() == FT_Connecting) {
				LOG_DEBUG<<"conecting "<<fh->fd()<<"\n";
				if(ev->events & MTP_EV_READ || ev->events & MTP_EV_WRITE) {
					int error;
					socklen_t len = sizeof(error);
					if(getsockopt(fh->fd(), SOL_SOCKET, SO_ERROR, &error, &len) < 0 || error != 0) {
						if(error !=0) errno = error;
						LOG_ERROR<<"connect to broker error: "<<strerror(errno)<<"\n";
						ClearFHPair(fh);
					} else {
						LOG_DEBUG<<fh->fd()<<" connection ok\n";
						//connection ok
						multiplexer_.Remove(fh->fd());
						fh->set_type(FT_Sock_C);
						FHPair *pair = new FHPair(fh->partner(), fh);
						//加入pipe队列等待写入pipe
						addInPipeQue(pair);
					}
				} else {
					//不太可能执行到这里
					LOG_ERROR<<"1?unknown error\n";
					ClearFHPair(fh);
				}
			} else if(fh->type() == FT_Pipe) {
				LOG_DEBUG<<"pipe event occur\n";
				continue;
				//TODO what s the better solution
			
			} else {
				LOG_ERROR<<"undefind handler for event"<<ev->events<<"\n";
				ClearFHPair(fh);
			}
		}
		writeAllPipes();
	}
}

void ProxyHandler::writeAllPipes() {
	IOState state;
	for(int i=0; i<w_pipes_.size(); ++i) {
		FdHandle *fh = w_pipes_[i];
		state = fh->DoWrite();
		if(state == IO_Again) {
			multiplexer_.Modify(fh->fd(), MTP_EV_WRITE | MTP_EV_ONESHOT, fh);
		}
	}
}

