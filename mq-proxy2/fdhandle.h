/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fdhandle.h
 * Author: chenwz5
 *
 * Created on 2016年8月10日, 上午11:25
 */

#ifndef FDHANDLE_H
#define FDHANDLE_H
#include <stdlib.h>
#include <string.h>
#include <queue>
#include <memory>
#include "config.h"
#include "error.h"
#include "packet.h"
#include "ptrbuf.h"
#include "ibuffer.h"

#define OUT_QUE_SIZE 32

class FdHandle {
public:
	FdHandle();
	~FdHandle();
	
	void set_fd(int fd) {fd_ = fd;}
	int fd(){return fd_;}
	
	void set_type(FdType t){type_ = t;}
	FdType type(){return type_;}
	
	void set_partner(FdHandle* ppfh) {partner_ = ppfh;}
	FdHandle* partner() {
		//if(partner_ == NULL) return NULL;
		return partner_;
	}
	FdHandle* partner2();
	
	bool IsOutable() {return out_que_.size() < max_out_que_size_;}
	bool hasDataOut() { return !out_que_.empty(); }
	void AddOutBuffer(IBuffer* pack) {out_que_.push(pack);}
	
	//读完一个完整的数据包返回IO_Done, 否则返回IO_Again
	IOState DoRead();
	IBuffer* ReleaseReadBuffer();
	
	//写完队列里全部数据返回IO_Done, 否则返回IO_Again
	IOState DoWrite();
	
	friend void ClearFHPair(FdHandle* fh);
	
	
private:
	int fd_;
	FdType type_;
	IBuffer* data_in_save_;
	IBuffer* data_out_save_;
	FdHandle* partner_;
	const int max_out_que_size_;
	std::queue<IBuffer* > out_que_;
};

struct FHPair {
	FdHandle *psfh;
	FdHandle *pcfh;
	
	FHPair(FdHandle *apsfh=NULL, FdHandle *apcfh=NULL):psfh(apsfh),pcfh(apcfh){}
};

#endif /* FDHANDLE_H */

