/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   error.h
 * Author: chenwz5
 *
 * Created on 2016年8月9日, 下午5:53
 */

#ifndef ERROR_H
#define ERROR_H

#define CHECK_ERRNO(ret) \
     do { if((ret) < 0) {return ERR_ERRNO;} } while(0)

#define CHECK_NOTOK(ret) \
    do { if((ret) != ERR_OK) {return (ret);} } while(0)

enum errcode {
    ERR_OK = 0,
    ERR_ERRNO,
    ERR_NET_ADDR,
	ERR_NULL_FH,
	ERR_REFUSED
};

const char * errstr(errcode code);

#endif /* ERROR_H */

