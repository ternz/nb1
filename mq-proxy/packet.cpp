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
#include "packet.h"

using namespace spxy;

void Packet::set_optype(Optype type) {
	optype_ = type;
	switch(type) {
		case None:
			doio = NULL;
			break;
		case Read:
			doio = Read;
			break;
		case Write:
			doio = Write;
			break;
	}
}

int Packet::Read(int fd) {
	int ret = 0;
	while(op_size_ < std::max(HEADER_SIZE, size_)) {
		if(op_size_ < HEADER_SIZE) {
			int nread = HEADER_SIZE - op_size_;
			ret = read(fd, op_base_, nread);
			if(ret < 0) {
				if(ret == EINTR) continue;
				if(ret == EAGAIN) {
					break;
				} else {
					return State::Errno;
				}
			} else if(ret == 0) {
				return State::SockOff;
			} else {
				op_size_ += ret;
				if(ret == nread) {
					if(header_.val > MAX_PACKET_SIZE) {
						return State::HeaderErr;
					}
					size_ = header_.val + HEADER_SIZE;
					data_.resize(size_);
					memcpy(&data_[0], &header_.bytes[0], HEADER_SIZE)
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
				if(ret == EINTR) continue;
				if(ret == EAGAIN) {
					break;
				} else {
					return State::Errno;
				}
			} else if(ret == 0) {
				return State::SockOff;
			} else {
				op_size_ += ret;
				op_base_ += ret;
			}
		}
	} //end of while
	if(op_size_ < std::max(HEADER_SIZE, size_)) {
		return State::Again;
	} 
	return State::Done;
}

int Packet::Write(int fd) {
	int ret = 0;
	while(op_size_ < size_) {
		int nwrite = size_ - op_size_;
		ret = write(fd, op_base_, nwrite);
		if(ret < 0) {
			if(ret == EINTR) continue;
			if(ret == EAGAIN) {
				break;
			} else {
				return State::Errno;
			}
		}
		op_size_ += ret;
	}
	if(op_size_ < size_) {
		return State::Again;
	}
	return State::Done;
}
