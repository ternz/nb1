/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   socket_connection.h
 * Author: chenwz5
 *
 * Created on 2016年7月12日, 上午11:25
 */

#ifndef SOCKET_CONNECTION_H
#define SOCKET_CONNECTION_H
#include "packet.h"
#include "socket_channel.h"

namespace N {
class SocketConnection {
public:
	SocketConnection(int fd);
	~SocketConnection();
	Packet* ReleaseInPacket();
	void PrepareOutPacket(Packet* pack);
	void ReleaseOutBuffer();
	friend class SocketChannel;
private:
	int sockfd_;
	Packet* in_pack_;
	OutBuffer* out_buf_;
};
}

#endif /* SOCKET_CONNECTION_H */

