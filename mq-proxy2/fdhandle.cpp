/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <unistd.h>

#include "fdhandle.h"

FdHandle::FdHandle()
		:fd_(-1), type_(FT_None), partner_(NULL), /*out_que_(OUT_QUE_SIZE),*/
		data_in_save_(NULL), data_out_save_(NULL), max_out_que_size_(OUT_QUE_SIZE) {
	}
		
FdHandle::~FdHandle() {
	if(fd_ != -1)
		close(fd_);
	if(data_in_save_ != NULL)
		delete data_in_save_;
	if(data_out_save_ != NULL)
		delete data_out_save_;
	while(!out_que_.empty()) {
		IBuffer *p = out_que_.front();
		out_que_.pop();
		delete p;
	}
}

IOState 
FdHandle::DoRead() {
	if(data_in_save_ == NULL) {
		if(type() == FT_Sock_C || type() == FT_Sock_S)
			data_in_save_ = new Packet();
		else 
			data_in_save_ = new PtrBuf();
	}
	return data_in_save_->Read(fd_);
}

IBuffer* FdHandle::ReleaseReadBuffer() {
	IBuffer* temp = data_in_save_;
	data_in_save_ = NULL;
	return temp;
}

IOState FdHandle::DoWrite() {
	while(data_out_save_ != NULL || (!out_que_.empty())) {
		if(data_out_save_ == NULL) {
			data_out_save_ = out_que_.front();
			out_que_.pop();
		}
		IOState state = data_out_save_->Write(fd_);
		if(state != IO_Done)
			return state;
		delete data_out_save_;
		data_out_save_ = NULL;
	}
	return IO_Done;
}

void ClearFHPair(FdHandle* fh) {
	if(fh == NULL)
		return;
	/*if(fh->partner_ != NULL && *(fh->partner_) != NULL) {
		delete *(fh->partner_);
		*(fh->partner_) = NULL;
	}*/
	delete fh->partner_;
	delete fh;
	fh = NULL;
}

FdHandle* FdHandle::partner2() {
	return this->partner_;
}