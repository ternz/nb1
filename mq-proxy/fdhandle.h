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
#include <unistd.h>
#include <list>
#include "packet.h"
#include "tsqueue.h"

namespace spxy {

enum FdType {
	FT_None = 0,
	Lstsock, 
	Cmnsock_C,   //proxy as a client
	Cmnsock_S,   //proxy as a server
	Pipe
};

enum IOState {
	S_None = 0,
	Readable = 1, 
	Writable = 2, 
	ReadWrite = 3,
	Error = 4,
	Connecting = 8,
	Reading = 16,
	Writing = 32
};

#define PARTNER(fh) (*((fh)->partner))

struct FdHandle {
	int fd;
	FdType type;
	IOState state;
	Packet* data_in;
	Packet* data_out;
	
	FdHandle** partner;
	
	FdHandle():fd(-1), type(FT_None), state(S_None), data(NULL), partner(NULL) {}
};

FdHandle* FHCreate();
void FHDestory(FdHandle* fh);

}

#endif /* SOCKHANDLE_H */

