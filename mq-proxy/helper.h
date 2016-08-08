/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   helper.h
 * Author: chenwz5
 *
 * Created on 2016年7月30日, 下午4:19
 */

#ifndef HELPER_H
#define HELPER_H

#include <sys/types.h>
#include <string>
#include <arpa/inet.h>

#include "error.h"

namespace spxy {

class Helper {
public:
	static errcode ParseEndpoint(std::string endpoint, std::string* ip, uint16_t* port);
    static errcode MakeSockAddr(std::string& ip, uint16_t port, struct sockaddr_in* addr);
	static errcode SetNonblock(int fd);
};

}


#endif /* HELPER_H */

