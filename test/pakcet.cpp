/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <cstring>
#include <string>
#include "packet.h"

using namespace std;
using namespace N;

Packet::Packet()
	:bytes_read_(0),playload(0) {
	memset(header, 0, sizeof(header));
	read_base_ = &header.bytes[0];
}

OutBuffer*  
Packet::Serialize(OutBuffer* out) {
	if(out == NULL) return NULL;
	out.buf.clear();
	out.buf.resize(header.val + HEADER_SIZE);
	out.buf.append(header.bytes, HEADER_SIZE);
	out.buf.append(&playload[0], header.val);
	out.bytes_writen = 0;
	return out;
}