/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   protocol.h
 * Author: chenwz5
 *
 * Created on 2016年7月12日, 上午11:02
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

namespace N {
    
template<typename T>
class Protocal {
public:
    int errno;
    enum ReadState {Error=-1,Done=0,Partly=1}; 
    virtual ReadState ReadPacket(int sockfd, T* pack) = 0;
    virtual int SerializePacket(T* pack, void* buf) = 0;
};

}


#endif /* PROTOCOL_H */

