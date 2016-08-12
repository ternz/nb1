/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "config.h"
#include "error.h"
#include "io_processor.h"
#include "fdhandle.h"
#include "packet.h"
#include "ptrbuf.h"
#include "multiplexer.h"

IOProcessor::~IOProcessor() {
	if(r_pipe_ != NULL) {
		delete r_pipe_;
		r_pipe_ = NULL;
	}
}

errcode IOProcessor::init() {
	multiplexer_.Add(r_pipe_->fd(), MTP_EV_READ, r_pipe_);
	return ERR_OK;
}

void IOProcessor::loop() {
	int ready_fds = multiplexer_.Wait(1000);
	//LOG_DEBUG<<"ready fds: "<<ready_fds<<"\n";
	
	for(int i=0; i<ready_fds; ++i) {
		struct epoll_event *ev = multiplexer_.get_ev(i);
		if(ev->data.ptr == NULL) continue;
		
		FdHandle *fh = (FdHandle*)ev->data.ptr;
		if(fh->type() == FT_Sock_C || fh->type() == FT_Sock_S) {
			LOG_DEBUG<<fh->fd()<<" socket event occur\n";
			IOState state;
			
			int issockoff = false;
			if(ev->events & MTP_EV_READ) {
				LOG_DEBUG<<fh->fd()<<" read\n";
				while(1) { //handle read
					state = fh->DoRead();
					if(state == IO_HeaderErr || state == IO_Errno) {
						LOG_ERROR<<fh->fd()<<" io error "<<state<<"\n";
						ClearFHPair(fh);
						issockoff = true;
						break;
					} else if(state == IO_SockOff) {
						LOG_INFO<<fh->fd()<<" connection off\n";
						ClearFHPair(fh);
						issockoff = true;
						break;
					} else if(state == IO_Done) {
						//TODO
						Packet *pack = (Packet *)(fh->ReleaseReadBuffer());
						pack->set_optype(OP_Write);
						pack->resetBaseAndSize_();
						fh->partner()->AddOutBuffer(pack);
					} else {
						break;
					}
				}
			}
			if(issockoff) continue;
			
			if(fh->partner()->hasDataOut()) {
				multiplexer_.Modify(fh->partner()->fd(), MTP_EV_READ | MTP_EV_WRITE, fh->partner());
			}
				
			if(ev->events & MTP_EV_WRITE) {
				LOG_DEBUG<<fh->fd()<<" write\n";
				state = fh->DoWrite();
				if(state == IO_Errno) {
					LOG_ERROR<<fh->fd()<<" io error "<<state<<"\n";
					ClearFHPair(fh);
				} else if(state == IO_Done) {
					multiplexer_.Modify(fh->fd(), MTP_EV_READ, fh);
				} else if(state == IO_Again) {
					//
				}
			}

		} else if(fh->type() == FT_Pipe || fh->type() == FT_SockPair) {
			LOG_DEBUG<<fh->fd()<<" pipe event occur!!\n";
			while(1) { //handle read
				//LOG_DEBUG<<"pipe "<<fh->fd()<<" read\n";
				IOState state = fh->DoRead();
				if(state == IO_Errno) {
					LOG_ERROR<<fh->fd()<<" io error: "<<ioerrstr(state)<<"\n";
					ClearFHPair(fh);
					break;
				} else if(state == IO_Done) {
					//TODO
					PtrBuf *pb = (PtrBuf*) (fh->ReleaseReadBuffer());
					FHPair *pair = (FHPair*)pb->Unwarp();
					bool err_flag;
					if(multiplexer_.Add(pair->psfh->fd(), MTP_EV_READ, pair->psfh)
						!= ERR_OK) {
						LOG_ERROR<<"multiplexer Add "<<pair->psfh->fd()<<" error: "<<errstr(ERR_ERRNO)<<"\n";
						err_flag = true;
					}
					if(multiplexer_.Add(pair->pcfh->fd(), MTP_EV_READ, pair->pcfh)
						!= ERR_OK) {
						LOG_ERROR<<"multiplexer Add "<<pair->pcfh->fd()<<" error: "<<errstr(ERR_ERRNO)<<"\n";
						err_flag = true;
					}
					if(err_flag) ClearFHPair(pair->psfh);
					delete pair;
					delete pb;
				} else {
					break;
				}
			}
		}
		
	} //end of for
}
