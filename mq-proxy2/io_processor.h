/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   io_processor.h
 * Author: chenwz5
 *
 * Created on 2016年8月11日, 下午4:27
 */

#ifndef IO_PROCESSOR_H
#define IO_PROCESSOR_H
#include <stdlib.h>
#include "processor.h"
#include "fdhandle.h"
#include "multiplexer.h"
#include "logger.h"

class IOProcessor: public Processor {
public:
	IOProcessor():r_pipe_(NULL){}
	virtual ~IOProcessor();
	
	void set_r_pipe(int fd) {
		r_pipe_ = new FdHandle();
		r_pipe_->set_fd(fd);
		r_pipe_->set_type(FT_Pipe);
	}
protected:
	virtual errcode init();
	virtual void loop();
	
private:
	Multiplexer multiplexer_;
	FdHandle *r_pipe_;
};


#endif /* IO_PROCESSOR_H */

