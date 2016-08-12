/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "error.h"

const char *errstr(errcode code) {
    switch(code) {
        case ERR_OK:
            return "OK";
        case ERR_ERRNO:
            return strerror(errno);
        case ERR_NET_ADDR:
            return "net address invalid";
		case ERR_NULL_FH:
			return "fh destoryed";
		case ERR_REFUSED:
			return "request refused";
        default:
            return "unkonwn error";
    }
}

const char * ioerrstr(IOState state) {
	switch(state) {
        case IO_HeaderErr:
            return "header error";
        case IO_Errno:
            return strerror(errno);
		case IO_Done:
			return "ok, done";
		case IO_Again:
			return "ok, again";
		case IO_SockOff:
			return "ok, socket off";
        default:
            return "unkonwn error";
    }
}