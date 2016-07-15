/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <errno.h>
#include "protocol.h"
#include "packet.h"

using namespace N;

int 
Protocol::ReadPacket(int fd, Packet* pack) {
	if(pack->bytes_read_ < pack->HEADER_SIZE) {
		int ret = 0;
		int nread = pack->HEADER_SIZE - pack->bytes_read_;
		ret = read(fd, pack->read_base_, nread);
		if(ret < 0) {
			if(ret == EINTR || ret == EAGAIN) {
				return IOState::Again;
			} else {
				return IOState::Errno;
			}
		} else if(ret == 0) {
			return IOState::Off;
		} else {
			pack->bytes_read_ += ret;
			if(ret == nread) {
				if(pack->header.val > pack->MAX_PACKET_SIZE) {
					return IOState::HeaderErr;
				}
				pack->playload.resize(pack->header.val);
				pack->bytes_read_ = &pack->playload[0];
			} else {
				pack->read_base_ += ret;
			}
		}
	}
	if(pack->bytes_read_ >= pack->HEADER_SIZE) {
		int ret = 0;
		int nread = pack->HEADER_SIZE + pack->header.val - pack->bytes_read_;
		ret = read(fd, pack->read_base_, nread);
		if(ret < 0) {
			if(ret == EINTR || ret == EAGAIN) {
				return IOState::Again;
			} else {
				return IOState::Errno;
			}
		} else if(ret == 0) {
			return IOState::Off;
		} else {
			pack->bytes_read_ += ret;
			pack->read_base_ += ret;
		}
	}
	if(pack->bytes_read_ < pack->HEADER_SIZE + pack->header.val) {
		return IOState::Again;
	} 
	return IOState::Done;
}

int 
WritePacket(int fd, OutBuffer* out) {
	int ret = 0;
	int nwrite = out->buf.size() - out->bytes_writen;
	ret = write(fd, out->buf[out->bytes_writen], nwrite);
	if(ret < 0) {
		if(ret == EINTR || ret == EAGAIN) {
			return IOState::Again;
		} else {
			return IOState::Errno;
		}
	}
	out->bytes_writen += ret;
	if(out->bytes_writen < out->buf.size()) {
		return IOState::Again;
	}
	return IOState::Done;
}
