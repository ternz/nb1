/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   packet.h
 * Author: chenwz5
 *
 * Created on 2016年7月12日, 上午11:35
 */

#ifndef PACKET_H
#define PACKET_H
#include <sys/types.h>
#include <vector>
#include "socket_connection.h"

namespace N {
class Packet {
public:
	SocketConnection* conn_;
	const int HEADER_SIZE = 4;
	union Header {
		char* bytes[HEADER_SIZE];
		uint32_t val;
	} header;
	std::vector<char> playload;
	int cur_read_len_;
	int cur_write_len_;
};
}

#endif /* PACKET_H */

