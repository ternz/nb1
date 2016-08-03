/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <pthread.h>
#include "processor.h"
#include "error.h"
#include "tsqueue.h"
#include "logger.h"
#include "fdhandle.h"

using namespace spxy;

Processor::Processor(TSQueue<FdHandle*> que, Multiplexer* mtp) 
	:que_(que), multiplexer_(mtp), thread_num_(0), threads_(NULL), run_(false) {
	
}

Processor::~Processor() {
	if(threads_ != NULL) 
		delete[] threads_;
}

int 
Processor::Run(int threads) {
	if(run_) return;
	run_ = true;
	thread_num_ = threads;
	threads_ = new pthread_t[thread_num_];
	int ret = 0;
	pthread_attr_t attr;
	ret = pthread_attr_init(&attr);
	CHECK_ERRNO(ret);
	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	CHECK_ERRNO(ret);
	
	for(int i=0; i < thread_num_; ++i) {
		ret = pthread_create(&threads_[i], &attr, threadFunc_, (void*)this);
		if((ret) < 0) {
			run_ = false;
			return errcode::ERRNO;
		}
	}
	pthread_attr_destroy(&attr);
	return errcode::OK;
}

void* Processor::threadFunc_(void *arg) {
	Processor *th = (Processor*)arg;
	while(th->run_) {
		FdHandle fh = th->que_.Pop();
		if(fh->state == IOState::Readable) {
			th->doDataIn(fh);
		}
	}
}

#define DESTORY_FH(fh) \
	do { FHDestory(fh->partner);FHDestory(fh);fh = NULL;} while (0)

void Processor::doDataIn(FdHandle *fh) {
	if(fh->data == NULL) {
		fh->data = new Packet();
		fh->data->set_optype(Packet::Optype::Read);
	}
	
	int ret = fh->data.Read(fh->fd);
	
	if(ret < 0) {
		LOG_ERROR<<Packet::Errstr(ret)<<"\n";
		//delete fh->data;
		//fh->data = NULL;
		DESTORY_FH(fh);
	} else if(ret == Packet::State::SockOff) {
		LOG_DEBUG<<"socket close\n";
		DESTORY_FH(fh);
	} else if(ret == Packet::State::Again) {
		//fh->state |= IOState::Reading; //no used
		this->multiplexer_->Add(fh->fd, Multiplexer::IO_READ | Multiplexer::IO_ONESHOT, fh);
	} else { //Done
		if(fh->partner->data != NULL) {
			LOG_ERROR<<"internal error\n";
			DESTORY_FH(fh);
			return;
		}
		Packet* new_pack = doDataConv(fh->data);
		fh->data = NULL;
		fh->state = IOState::None;
		fh->partner->data = new_pack;
		fh->partner->state = IOState::Writing; // no used
		this->multiplexer_->Add(fh->partner->fd, Multiplexer::IO_WRITE | Multiplexer::IO_ONESHOT, fh->partner);
	}
}

void Processor::doDataOut(FdHandle* fh) {
	int ret = fh->data->Write(fh->fd);
	
	if(ret < 0) {
		LOG_ERROR<<Packet::Errstr(ret)<<"\n";
		//delete fh->data;
		//fh->data = NULL;
		DESTORY_FH(fh);
	} else if(ret == Packet::State::Again) {
		this->multiplexer_->Add(fh->fd, Multiplexer::IO_WRITE | Multiplexer::IO_ONESHOT, fh);
	} else { //Done
		fh->data = NULL;
		fh->state = IOState::None;
		this->multiplexer_->Add(fh->fd, Multiplexer::IO_READ | Multiplexer::IO_ONESHOT, fh);
	}
}

Packet * Processor::doDataConv(Packet *pack) {
	pack->set_optype(Packet::Write);
	pack->op_size_ = 0;
	pack->op_base_ = &pack->data_[0];
	return pack;
}
