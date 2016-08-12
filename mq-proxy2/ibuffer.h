/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IBuffer.h
 * Author: chenwz5
 *
 * Created on 2016年8月11日, 上午10:27
 */

#ifndef IBUFFER_H
#define IBUFFER_H
#include "config.h"

class IBuffer {
public:
	virtual ~IBuffer(){}
	virtual IOState Read(int fd) = 0;
	virtual IOState Write(int fd) = 0;
};

#endif /* IBUFFER_H */

