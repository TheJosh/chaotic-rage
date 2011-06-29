// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <SDL_net.h>
#include "rage.h"
#include "net.h"

using namespace std;


NetMsg::NetMsg (NetMsgType type, unsigned int size)
{
	this->type = type;
	this->size = size;
	
	this->data = NULL;
	if (size > 0) {
		this->data = (Uint8*) malloc(size);
		memset(this->data, 0, size);
	}
}

NetMsg::~NetMsg()
{
	free(data);
}


void dumpPacket(Uint8* data, int size)
{
	cout << "{ ";
	for (int i = 0; i < size; i++) {
		cout << hex << setw (2) << setfill('0') << ((int)data[i]) << " ";
	}
	cout << dec << "}" << endl;
}

