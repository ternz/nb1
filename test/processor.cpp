/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "processor.h"

using namespace N;

Processor::Processor(int workers, common::BlockingQueue<Packet*>* que) 
	:workers(workers),work_que_(que) {
	
}

Processor::~Processor() {}
