/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   processor.h
 * Author: chenwz5
 *
 * Created on 2016年7月12日, 下午4:03
 */

#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <pthread.h>
#include "common/blocking_queue.h"

namespace N {
class Processor {
public:
	Processor(int workers, common::BlockingQueue<Packet*>* que);
	~Processor();
	int RunThreads();
private:
	common::BlockingQueue<Packet*>* work_que_;
	int workers;
	pthread_t* tids;
	static handleFunc(void* arg);
};
}

#endif /* PROCESSOR_H */

