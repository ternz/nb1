/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: chenwz5
 *
 * Created on 2016年8月9日, 下午5:18
 */

#include <cstdlib>
#include <unistd.h>

#include "config.h"
#include "error.h"
#include "io_processor.h"
#include "proxy_handler.h"
#include "helper.h"

using namespace std;

const int IOPSR_NUM = 2;

ProxyHandler g_proxyhandler;
IOProcessor g_processors[IOPSR_NUM];

int pipes[IOPSR_NUM][2];

int main(int argc, char** argv) {
	
	string s1 = "127.0.0.1:9990";
	string s2 = "127.0.0.1:10009";
	
	errcode err = g_proxyhandler.Init(s1, s2);
	if(err != ERR_OK) {
		LOG_ERROR<<"proxy handler init error: "<<errstr(err)<<"\n";
		exit(int(err));
	}
		
	int ret;
	for(int i=0; i<IOPSR_NUM; i++) {
		ret = pipe(pipes[i]);
		if(ret < 0) {
			LOG_ERROR<<"create pipe error"<<errstr(ERR_ERRNO)<<"\n";
			exit(ERR_ERRNO);
		}
		
		for(int j=0; j<2; j++) {
			ret = Helper::SetNonblock(pipes[i][j]);
			if(ret < 0) {
				LOG_ERROR<<"fd "<<pipes[i][j]<<" set nonblock error: "<<errstr(ERR_ERRNO)<<"\n";
				exit(ret);
			}
		}
		
		g_proxyhandler.AddPipe(pipes[i][1]);
		g_processors[i].set_r_pipe(pipes[i][0]);
	}
	
	for(int i=0; i<IOPSR_NUM; i++) {
		g_processors[i].Run();
	}
	
	g_proxyhandler.Loop();
	
	return 0;
}

