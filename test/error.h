/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   error.h
 * Author: chenwz5
 *
 * Created on 2016年7月8日, 上午10:37
 */

#ifndef ERROR_H
#define ERROR_H

#define CHECK_ERRNO(ret) \
     do { if((ret) < 0) {return errcode::ERRNO;} } while(0)

#define CHECK_NOTOK(ret) \
    do { if((ret) != errcode::OK) {return (ret);} } while(0)

enum errcode {
    OK,
    ERRNO,
    NET_ADDR,
	FD_EXIST,
};

const char * errstr(int code);

#endif /* ERROR_H */

