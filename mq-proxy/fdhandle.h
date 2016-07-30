/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   fdhandle.h
 * Author: chenwz5
 *
 * Created on 2016年7月30日, 下午2:35
 */

#ifndef FDHANDLE_H
#define FDHANDLE_H
#include <stdlib.h>

namespace spxy {

enum FdType {
	None = 0,
	Lstsock, 
	Cmnsock, 
	Pipe
};

enum IOState {
	None = 0,
	Read, 
	Write, 
	Error
};

struct FdHandle {
	int fd;
	FdType type;
	IOState state;
	void* data;
	FdHandle* partner;
	
	FdHandle():fd(-1), type(0), state(0), data(NULL), partner(NULL) {}
	
	friend FdHandle* FHCreate() {
		return new FdHandle();
	}
	friend void FHDestory(FdHandle* fh) {
		close(fh);
		delete fh;
	}
};

}

#endif /* SOCKHANDLE_H */

