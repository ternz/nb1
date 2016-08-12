/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <algorithm>
#include <vector>
#include "packet.h"
#include "error.h"
#include "logger.h"

Packet::Packet(Optype type)
	:size_(0), data_(std::vector<char>(0)), optype_(type) 
{
	resetBaseAndSize_();
}

/*const char * Packet::Errstr(State s) {
	switch(s) {
		case HeaderErr:
			return "packet header error";
		case Errno:
			return errstr(ERR_ERRNO);
		default:
			return "no error";
	}
}*/

void Packet::set_optype(Optype type) {
	optype_ = type;
}

//fd必须nonblock
IOState Packet::Read(int fd) {
	int ret = 0;
	while(op_size_ < std::max(HEADER_SIZE, size_)) {
		if(op_size_ < HEADER_SIZE) {
			int nread = HEADER_SIZE - op_size_;
			ret = read(fd, op_base_, nread);
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
				if(ret == nread) {
					if(header_.val > MAX_PACKET_SIZE) {
						LOG_ERROR<<"error header: "<<std::hex
							<<header_.bytes[0]<<header_.bytes[1]<<header_.bytes[2]<<header_.bytes[3]<<"\n";
						return IO_HeaderErr;
					}
					size_ = header_.val + HEADER_SIZE;
					data_.resize(size_);
					memcpy(&data_[0], &header_.bytes[0], HEADER_SIZE);
					op_base_ = &data_[HEADER_SIZE];
				} else {
					op_base_ += ret;
				}
			}
		}
		if(op_size_ >= HEADER_SIZE) {
			int ret = 0;
			int nread = size_ - op_size_;
			ret = read(fd, op_base_, nread);
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
	} //end of while
	if(op_size_ < std::max(HEADER_SIZE, size_)) {
		return IO_Again;
	} 
	return IO_Done;
}

//fd必须nonblock
IOState Packet::Write(int fd) {
	int ret = 0;
	while(op_size_ < size_) {
		int nwrite = size_ - op_size_;
		ret = write(fd, op_base_, nwrite);
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
	if(op_size_ < size_) {
		return IO_Again;
	}
	return IO_Done;
}
