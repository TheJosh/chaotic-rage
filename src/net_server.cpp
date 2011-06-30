// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <SDL_net.h>
#include "rage.h"
#include "net.h"

using namespace std;


NetServer::NetServer(GameState * st)
{
	this->st = st;
	st->server = this;
	
	this->seq = 0;
	this->seq_pred = new NetServerSeqPred(this);
	
	this->client_seq = 0; // todo: client seq array
}

NetServer::~NetServer()
{
	if (this->sock != NULL) SDLNet_UDP_Close(this->sock);
}


/**
* Some time has passed, do stuff
**/
void NetServer::update()
{
	UDPpacket *pkt = SDLNet_AllocPacket(1000);
	
	this->seq++;
	
	
	while (SDLNet_UDP_Recv(this->sock, pkt)) {
		cout << setw (6) << setfill(' ') << st->game_time << " RECV ";
		dumpPacket(pkt->data, pkt->len);
		
		Uint8* ptr = pkt->data;
		int p = 0;
		
		unsigned int newseq = SDLNet_Read16(ptr);
		if (newseq > this->client_seq) {
			this->client_seq = newseq;	// todo: client seq array
			cout << "       The client has ACKed " << newseq << "\n";
		}
		ptr += 2; p += 2;
		
		while (p < pkt->len) {
			unsigned int type = (*ptr);
			ptr++; p++;
			
			cout << "       Got data: " << hex << setw (2) << (type) << dec << endl;
			
			if (type > NOTHING && type < BOTTOM) {
				cout << "       It's even valid!\n";
				
				if (msg_server_recv[type] != NULL) {
					cout << "       Firing handler\n";
					unsigned int num = ((*this).*(msg_server_recv[type]))(ptr, pkt->len - p);
					ptr += num; p += num;
				}
			}
		}
		
		//this->addmsgInfoResp();
	}
	
	
	// TODO: ----- loop over clients -----
	
	// TODO: fill pkt->address with client address
	pkt->len = 0;
	
	Uint8* ptr = pkt->data;
	
	SDLNet_Write16(this->seq, ptr);
	ptr += 2; pkt->len += 2;
	
	for (list<NetMsg>::iterator it = this->messages.begin(); it != this->messages.end(); it++) {
		
		// client_seq
		
		// TODO: only items with a seq for that client
		
		*ptr = (*it).type;
		ptr++; pkt->len++;
		
		memcpy(ptr, (*it).data, (*it).size);
		ptr += (*it).size; pkt->len += (*it).size;
	}
	
	if (pkt->len > 0) {
		cout << setw (6) << setfill(' ') << st->game_time << " SEND ";
		dumpPacket(pkt->data, pkt->len);
		
		SDLNet_UDP_Send(this->sock, -1, pkt);
	}
	
	// TODO: ----- end loop over clients -----
	
	
	this->messages.remove_if(*this->seq_pred);
	
	//SDLNet_FreePacket(pkt);
}


/**
* Set the port for listening for connections
**/
void NetServer::listen(int port)
{
	SDLNet_ResolveHost(&this->ipaddress, NULL, port);
	this->sock = SDLNet_UDP_Open(port);
}



/**
***  One method for each outgoing network message the server sends out
**/

void NetServer::addmsgInfoResp() {
	NetMsg * msg = new NetMsg(INFO_RESP, 0);
	msg->seq = this->seq;
	messages.push_back(*msg);
}

void NetServer::addmsgJoinAcc() {
}

void NetServer::addmsgJoinRej() {
}

void NetServer::addmsgDataCompl() {
}

void NetServer::addmsgChat() {
}

void NetServer::addmsgUnitAdd() {
}

void NetServer::addmsgUnitUpdate() {
}

void NetServer::addmsgUnitRem() {
}

void NetServer::addmsgWallUpdate() {
}

void NetServer::addmsgWallRem() {
}

void NetServer::addmsgPgAdd() {
}

void NetServer::addmsgPgRem() {
}

void NetServer::addmsgPlayerDrop() {
}

void NetServer::addmsgPlayerQuit() {
}



/**
***  One method for each incoming network message from the client
**/

unsigned int NetServer::handleInfoReq(Uint8 *data, unsigned int size)
{
	cout << "       handleInfoReq()\n";
	return 0;
}

unsigned int NetServer::handleJoinReq(Uint8 *data, unsigned int size)
{
	cout << "       handleJoinReq()\n";
	return 0;
}

unsigned int NetServer::handleJoinAck(Uint8 *data, unsigned int size)
{
	cout << "       handleJoinAck()\n";
	return 0;
}

unsigned int NetServer::handleChat(Uint8 *data, unsigned int size)
{
	cout << "       handleChat()\n";
	return 0;
}

unsigned int NetServer::handleKeyMouseStatus(Uint8 *data, unsigned int size)
{
	cout << "       handleKeyMouseStatus()\n";
	return 0;
}

unsigned int NetServer::handleQuit(Uint8 *data, unsigned int size)
{
	cout << "       handleQuit()\n";
	return 0;
}



