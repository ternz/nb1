/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "processor.h"

errcode Processor::Run() {
	if(run) return ERR_REFUSED;
	run = true;
	
	int ret = 0;
	
	ret = init();
	if(ret != ERR_OK) 
		return errcode(ret);
	
	pthread_attr_t attr;
	ret = pthread_attr_init(&attr);
	CHECK_ERRNO(ret);
	ret = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	CHECK_ERRNO(ret);
	
	ret = pthread_create(&tid, &attr, threadFunc, (void*)this);
	
	pthread_attr_destroy(&attr);
	return ERR_OK;
}

void* Processor::threadFunc(void *arg) {
	Processor* th = (Processor*)arg;
	while(th->run) {
		th->loop();
	}
}