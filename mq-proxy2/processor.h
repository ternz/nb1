/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   processor.h
 * Author: chenwz5
 *
 * Created on 2016年8月11日, 下午4:10
 */

#ifndef PROCESSOR_H
#define PROCESSOR_H
#include "pthread.h"
#include "error.h"

class Processor {
public:
	Processor():run(false){}
	virtual ~Processor(){}
	errcode Run();
protected:
	virtual errcode init(){return ERR_OK;}
	virtual void loop() = 0;
	static void* threadFunc(void *arg);
	bool run;
	pthread_t tid;
};

#endif /* PROCESSOR_H */

