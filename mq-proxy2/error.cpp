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