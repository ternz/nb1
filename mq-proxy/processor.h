/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   processor.h
 * Author: CHENWZ5
 *
 * Created on 2016年8月1日, 下午5:51
 */
#include "fdhandle.h"
#include "multiplexer.h"
#include "tsqueue.h"
#include "error.h"

#ifndef PROCESSOR_H
#define PROCESSOR_H

namespace spxy {
class Processor {
public:
	Processor(TSQueue<FdHandle*> *que, Multiplexer *mtp);
	~Processor();
	errcode Run(int threads);
private:
	TSQueue<FdHandle*> *que_;
	Multiplexer* multiplexer_;
	int thread_num_;
	pthread_t *threads_;
	
	bool run_;
	
	static void* threadFunc_(void *arg);
	void doDataIn(FdHandle *fh);
	void doDataOut(FdHandle *fh);
	Packet * doDataConv(Packet *pack);
};
}

#endif /* PROCESSOR_H */

