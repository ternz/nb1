/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   packet.h
 * Author: CHENWZ5
 *
 * Created on 2016年8月1日, 下午6:15
 */

#ifndef PACKET_H
#define PACKET_H
#include <vector>
#include <sys/types.h>
#include <stdint.h>

#include "error.h"

namespace spxy {
class Processor;

#define MAX_PACKET_SIZE 65535
#define HEADER_SIZE 4

class Packet {
public:
	enum Optype {NONE=0, READ, WRITE};
	enum State {HeaderErr=-2,Errno=-1,Done=0,Again=1,SockOff=2}; 
	
	Packet();
	static const char* Errstr(State s);
	
	void set_optype(Optype type);
	Optype get_optype() {return optype_;}
	int size() {return size_;}
	
	//出错返回-1, IO全部完成返回0, 部分完成返回1
	//int (*doio)(int fd);
	State Read(int fd);
	State Write(int fd);
	
	friend class Processor;
private:
	union Header {
		char bytes[HEADER_SIZE];
		uint32_t val;
	} header_;
	std::vector<char> data_; 
	//char *header_base_;
	//char *playload_base_;
	int size_;
	
	Optype optype_;
	
	char *op_base_;
	int op_size_;
};
}

#endif /* PACKET_H */

