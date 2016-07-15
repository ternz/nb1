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
enum IOState {HeaderErr=-2,Errno=-1,Done=0,Again=1,Off=2}; 

class Protocol {
public:
    int ReadPacket(int fd, Packet* pack);
    int WritePacket(int fd, OutBuffer* buf);
};

}


#endif /* PROTOCOL_H */

