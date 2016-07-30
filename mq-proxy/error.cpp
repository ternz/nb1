/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <errno.h>
#include <string.h>

#include "error.h"

const char *errstr(errcode code) {
    switch(code) {
        case OK:
            return "OK";
        case ERRNO:
            return strerror(errno);
        case NET_ADDR:
            return "net address invalid";
        default:
            return "unkonwn error";
    }
}
