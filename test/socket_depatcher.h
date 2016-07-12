/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   socket_depatcher.h
 * Author: chenwz5
 *
 * Created on 2016年7月8日, 下午3:10
 */

#ifndef SOCKET_DEPATCHER_H
#define SOCKET_DEPATCHER_H

#include <boost/shared_ptr.hpp>
#include "collector.h"

namespace N {
    
class SocketDepatcher {
public:
    SocketDepatcher(boost::shared_ptr<Collector> &collector);
    void Depatch(int sockfd);
private:
    boost::shared_ptr<Collector> socket_collector;
};
    
}

#endif /* SOCKET_CHANNEL_LIST_H */

