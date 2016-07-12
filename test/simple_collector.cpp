/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdlib.h>
#include "simple_collector.h"

using namespace N;

SimpleCollector::SimpleCollector() {
	this->sc = new SocketChannel(&in_que, &out_que);
}

SimpleCollector::~SimpleCollector() {
	delete sc;
}

void
SimpleCollector::Add(SocketChannel* sc) {
	return;
    if(sc == NULL) {
        return;
    }
    this->sc = sc;
}

SocketChannel* 
SimpleCollector::Find(int* key) {
    if(sc->get_id() == &key) {
        return sc;
    }
    return NULL;
}

void 
SimpleCollector::Remove(int* key) {
    return;
}

SocketChannel* 
SimpleCollector::GetItem() {
    return sc;
}

size_t
SimpleCollector::Size() {
    if(sc != NULL) 
        return 1;
    else 
        return 0;
}