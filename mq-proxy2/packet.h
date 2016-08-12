/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   packet.h
 * Author: chenwz5
 *
 * Created on 2016年8月10日, 上午11:48
 */

#ifndef PACKET_H
#define PACKET_H

#include <vector>
#include <sys/types.h>
#include <stdint.h>

#include "error.h"
#include "config.h"
#include "ibuffer.h"

#define MAX_PACKET_SIZE 65535
#define HEADER_SIZE 4

class IOProcessor;

class Packet:public IBuffer {
public:
	explicit Packet(Optype type=OP_Read);
	//static const char* Errstr(IOState s);
	virtual ~Packet(){}
	
	void set_optype(Optype type);
	Optype optype() {return optype_;}
	int size() {return size_;}
	
	
	//出错返回-1, IO全部完成返回0, 部分完成返回1
	virtual IOState Read(int fd);
	virtual IOState Write(int fd);
	
	friend class IOProcessor;
	
private:
	union Header {
		char bytes[HEADER_SIZE];
		uint32_t val;
	} header_;
	std::vector<char> data_; 
	
	int size_;
	
	Optype optype_;
	
	char *op_base_;
	int op_size_;

	void resetBaseAndSize_() {
		op_size_ = 0;
		if(optype() == OP_Read) 
			op_base_ = &(header_.bytes[0]);
		else 
			op_base_ = &(data_[0]);
	}
};


#endif /* PACKET_H */

