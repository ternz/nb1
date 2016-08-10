/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   config.h
 * Author: chenwz5
 *
 * Created on 2016年8月10日, 上午11:20
 */

#ifndef CONFIG_H
#define CONFIG_H

enum FdType {
	FT_None = 0,
	FT_Listen, 
	FT_Connecting,
	FT_Sock_C,   //proxy as a client
	FT_Sock_S,   //proxy as a server
	FT_Pipe,
	FT_PipeR,
	FT_PipeW,
	FT_SockPair
};

enum Optype {
	OP_NONE=0, 
	OP_READ, 
	OP_WRITE
};

enum IOState {
	IO_HeaderErr=-2,
	IO_Errno=-1,
	IO_Done=0,
	IO_Again,
	IO_SockOff
}; 

struct EvData {
	FdType type;
	union data {
		void *ptr;
		int fd;
	};
};

#endif /* CONFIG_H */

