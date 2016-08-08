/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: chenwz5
 *
 * Created on 2016年7月29日, 下午7:40
 */

#include <cstdlib>
#include <string>
#include <signal.h>

#include "logger.h"
#include "multiplexer.h"
#include "processor.h"
#include "proxy_handler.h"
#include "tsqueue.h"
#include "fdhandle.h"
#include "error.h"

using namespace std;
using namespace spxy;

void signal_handler(int sig) {
	switch(sig) {
		case SIGINT:
		case SIGTERM:
			LOG_INFO<<"exit\n";
			exit(0);
		default:
			LOG_ERROR<<"unhandle signal "<<sig<<"\n";
	}
}
/*
 * 
 */
int main(int argc, char** argv) {
	/*sigset_t newmask, oldmask;
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGHUP);
	sigaddset(&newmask, SIGINT);
	sigaddset(&newmask, SIGTERM);
	if(sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
		LOG_ERROR<<"sigprocmask error: "<<errstr(ERR_ERRNO)<<"\n";
		exit(ERR_ERRNO);
	}
	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);*/
	
	TSQueue<FdHandle*> que(10000);
	Multiplexer mtp;
	ProxyHandler ph(&que, &mtp);
	Processor processor(&que, &mtp);
	errcode ret;
	string s1 = "127.0.0.1:9990";
	string s2 = "127.0.0.1:10009";
	ret = ph.Run(s1, s2);
	if(ret != ERR_OK) {
		LOG_ERROR<<"proxy handler run error: "<<errstr(ret);
		exit(ret);
	}
	ret = processor.Run(4);
	if(ret != ERR_OK) {
		LOG_ERROR<<"processor run error: "<<errstr(ret);
		exit(ret);
	}
	ph.Wait();
	return 0;
}

