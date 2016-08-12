/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   pointerbuf.h
 * Author: chenwz5
 *
 * Created on 2016年8月10日, 下午4:02
 */

#ifndef PTRBUF_H
#define PTRBUF_H
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "ibuffer.h"

#define PTRBUF_SIZE (sizeof(void*))

class PtrBuf: public IBuffer {
public:
	PtrBuf(void* ptr=NULL);
	virtual ~PtrBuf(){}
	IOState Doio(int fd){
		if(type_ == OP_Read) 
			return Read(fd);
		else
			return Write(fd);
	}
	virtual IOState Read(int fd);
	virtual IOState Write(int fd);
	void* Unwarp() { 
		void* ptr = NULL; 
		memcpy(&ptr, buf_, PTRBUF_SIZE); 
		return ptr;
	}
private:
	Optype type_;
	char buf_[PTRBUF_SIZE];
	char *op_base_;
	int op_size_;
	
	int remainSize() { return PTRBUF_SIZE-op_size_;}
};


#endif /* POINTERBUF_H */

