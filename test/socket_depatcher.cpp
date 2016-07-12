/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "socket_depatcher.h"

using namespace N;

SocketDepatcher::SocketDepatcher(boost::shared_ptr<Collector>& collector) {
    this->socket_collector = collector;
}

void
SocketDepatcher::Depatch(int sockfd) {
    SocketChannel* p_sc = socket_collector->GetItem();
	p_sc->AddConnection(sockfd);
}

