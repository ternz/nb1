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
#include <queue>
#include <memory>
#include "packet.h"

#define OUT_QUE_SIZE 32

#define DELETE_PAIR(fh) delete (fh)->partner(); delete (fh)

class FdHandle {
public:
	FdHandle();
	~FdHandle();
	
	void set_fd(int fd) {fd_ = fd;}
	int fd(){return fd_;}
	void set_type(FdType t){type_ = t;}
	FdType type(){return type_;}
	void set_partner(FdHandle** ppfh) {partner_ = ppfh;}
	FdHandle* partner() {return *partner_;}
	bool IsOutable() {return out_que_.size() < max_out_que_size_;}
	void AddOutPacket(Packet* pack) {out_que_.push(pack);}
	
	//读完一个完整的数据包返回IO_Done, 否则返回IO_Again
	IOState DoRead();
	Packet* ReleaseReadPacket();
	
	//写完队列里全部数据返回IO_Done, 否则返回IO_Again
	IOState DoWrite();
	
private:
	int fd_;
	FdType type_;
	Packet* data_in_save_;
	Packet* data_out_save_;
	std::queue<Packet* > out_que_;
	FdHandle** partner_;
	const int max_out_que_size_;
};

struct FHPair {
	FdHandle *psfh;
	FdHandle *pcfh;
	
	FHPair(FdHandle *apsfh=NULL, FdHandle *apcfh=NULL):psfh(apsfh),pcfh(apcfh){}
};

#endif /* FDHANDLE_H */

