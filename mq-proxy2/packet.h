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

#define MAX_PACKET_SIZE 65535
#define HEADER_SIZE 4

class Packet {
	IOState Read(int fd) = 0;
	IOState Write(int fd) = 0;
};

class Packet {
public:
	Packet();
	//static const char* Errstr(IOState s);
	
	void set_optype(Optype type);
	Optype optype() {return optype_;}
	int size() {return size_;}
	
	//出错返回-1, IO全部完成返回0, 部分完成返回1
	IOState Read(int fd);
	IOState Write(int fd);
	
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
};


#endif /* PACKET_H */

