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
#include <list>
#include "packet.h"

namespace spxy {

enum FdType {
	None = 0,
	Lstsock, 
	Cmnsock_C,   //proxy as a client
	Cmnsock_S,   //proxy as a server
	Pipe
};

enum IOState {
	None = 0,
	Readable = 1, 
	Writable = 2, 
	Error = 4,
	Connecting = 8,
	Reading = 16,
	Writing = 32
};

struct FdHandle {
	int fd;
	FdType type;
	IOState state;
	Packet* data;
	FdHandle* partner;
	
	FdHandle():fd(-1), type(0), state(0), data(NULL), partner(NULL) {}
	
	friend FdHandle* FHCreate() {
		return new FdHandle();
	}
	friend void FHDestory(FdHandle* fh) {
		if(fh == NULL) return;
		if(fh->fd != -1)
			close(fh->fd);
		if(fh->data != NULL)
			delete fh->data;
		delete fh;
		//TODO: handle data poiter
	}
};

}

#endif /* SOCKHANDLE_H */

