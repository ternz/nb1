/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <unistd.h>
#include <errno.h>
#include "ptrbuf.h"
#include "config.h"

PtrBuf::PtrBuf(void* ptr) {
	if(ptr == NULL) {
		memset(buf_, 0, PTRBUF_SIZE);
		type_ = OP_Read;
	} else {
		memcpy(buf_, &ptr, PTRBUF_SIZE);
		type_ = OP_Write;
	}
	op_base_ = buf_;
	op_size_ = 0;
}

IOState PtrBuf::Read(int fd) {
	int n, ret;
	while((n = remainSize()) > 0) {
		ret = read(fd, op_base_, n);
		if(ret < 0) {
			if(errno == EINTR) continue;
			if(errno == EAGAIN) {
				break;
			} else {
				return IO_Errno;
			}
		} else if(ret == 0) {
			return IO_SockOff;
		} else {
			op_size_ += ret;
			op_base_ += ret;
		}
	}
	if(remainSize() > 0) {
		return IO_Again;
	} 
	return IO_Done;
}

IOState PtrBuf::Write(int fd) {
	int n, ret;
	while((n = remainSize()) > 0) {
		ret = write(fd, op_base_, n);
		if(ret < 0) {
			if(errno == EINTR) continue;
			if(errno == EAGAIN) {
				break;
			} else {
				return IO_Errno;
			}
		} else {
			op_size_ += ret;
			op_base_ += ret;
		}
	}
	if(remainSize() > 0) {
		return IO_Again;
	} 
	return IO_Done;
}