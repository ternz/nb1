/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "fdhandle.h"
namespace spxy {

FdHandle* FHCreate() {
	return new FdHandle();
}

void FHDestory(FdHandle* fh) {
	if(fh == NULL) return;
	if(fh->fd != -1)
		close(fh->fd);
	if(fh->data != NULL)
		delete fh->data;
	delete fh;
	//TODO: handle data poiter
}

}
