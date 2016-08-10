/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "fdhandle.h"

FdHandle::FdHandle()
		:fd(-1), type(FT_None), partner(NULL), out_que_(OUT_PACK_NUM),
		data_in_save_(NULL), data_out_save_(NULL), max_out_que_size_(OUT_QUE_SIZE) {
	}
		
FdHandle::~FdHandle() {
	if(fd != -1)
		close(fd);
	if(data_in_save_ != NULL)
		delete data_in_save_;
	if(data_out_save_ != NULL)
		delete data_out_save_;
	while(!out_que_.empty())
		delete out_que_.pop();
}

IOState 
FdHandle::DoRead() {
	if(data_in_save_ != NULL) {
		data_in_save_ = new Packet();
	}
	return data_in_save_->Read(fd_);
}

Packet* FdHandle::ReleaseReadPacket() {
	Packet* temp = data_in_save_;
	data_in_save_ = NULL;
	return temp;
}

IOState FdHandle::DoWrite() {
	while(data_out_save_ != NULL || (!out_que_.empty())) {
		if(data_out_save_ == NULL) 
			data_out_save_ = out_que_.pop();
		IOState state = data_out_save_->Write(fd_);
		if(state != IO_Done)
			return state;
		delete data_out_save_;
		data_out_save_ = NULL;
	}
	return IO_Done;
}