// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <SDL_net.h>
#include "rage.h"
#include "net.h"

using namespace std;



// The lengths of all of the messages
int msgtype_len [] = {
	0,		//<-- 0x00
	0,		//<-- 0x01
	0,		//<-- 0x02
	0,		//<-- 0x03
	0,		//<-- 0x04
	0,		//<-- 0x05
	0,		//<-- 0x06
	0,		//<-- 0x07
	0,		//<-- 0x08
	0,		//<-- 0x09
	0,		//<-- 0x0A
	0,		//<-- 0x0B
	0,		//<-- 0x0C
	0,		//<-- 0x0D
	0,		//<-- 0x0E
	0,		//<-- 0x0F
	0,		//<-- 0x10
	0,		//<-- 0x11
	0,		//<-- 0x12
	0,		//<-- 0x13
	0,		//<-- 0x14
};



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

