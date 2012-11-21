// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <algorithm>
#include <SDL_net.h>
#include "../rage.h"
#include "net.h"

using namespace std;


NetServer::NetServer(GameState * st)
{
	this->st = st;
	st->server = this;
	
	this->seq = 0;
	this->seq_pred = new NetServerSeqPred(this);
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
	UDPpacket *pkt = SDLNet_AllocPacket(1024);
	
	this->seq++;
	
	
	while (SDLNet_UDP_Recv(this->sock, pkt)) {
		cout << setw (6) << setfill(' ') << st->game_time << " RECV ";
		dumpPacket(pkt->data, pkt->len);
		
		Uint8* ptr = pkt->data;
		int p = 0;
		
		SeqNum newseq = SDLNet_Read16(ptr);
		ptr += 2; p += 2;
		
		Uint16 code = SDLNet_Read16(ptr);
		ptr += 2; p += 2;
		
		NetServerClientInfo *client = NULL;
		for (list<NetServerClientInfo*>::iterator cli = this->clients.begin(); cli != this->clients.end(); cli++) {
			if ((*cli) != NULL && (*cli)->ipaddress.host == pkt->address.host && (*cli)->code == code) {
				client = (*cli);
				break;
			}
		}
		
		if (client != NULL && newseq > client->seq) {
			client->seq = newseq;
			cout << "       The client has ACKed " << newseq << "\n";
		}
		
		if (client == NULL) {
			client = new NetServerClientInfo();
			client->ipaddress.host = pkt->address.host;
			client->ipaddress.port = pkt->address.port;
			client->code = code;
		}
		
		while (p < pkt->len) {
			unsigned int type = (*ptr);
			ptr++; p++;
			
			if (type > NOTHING && type < BOTTOM) {
				if (msg_server_recv[type] != NULL) {
					unsigned int num = ((*this).*(msg_server_recv[type]))(client, ptr, pkt->len - p);
					ptr += num; p += num;
				}
			}
		}
	}
	
	
	for (list<NetServerClientInfo*>::iterator cli = this->clients.begin(); cli != this->clients.end(); cli++) {
		if ((*cli) == NULL) continue;
		
		pkt->address.host = (*cli)->ipaddress.host;
		pkt->address.port = (*cli)->ipaddress.port;
		
		pkt->len = 0;
		
		Uint8* ptr = pkt->data;
		
		SDLNet_Write16(this->seq, ptr);
		ptr += 2; pkt->len += 2;
		
		SDLNet_Write16(0, ptr);		// pad
		ptr += 2; pkt->len += 2;
		
		for (list<NetMsg>::iterator it = this->messages.begin(); it != this->messages.end(); it++) {
			if ((*cli)->seq > (*it).seq) continue;
			
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
	}
	
	
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

void NetServer::addmsgInfoResp()
{
	NetMsg * msg = new NetMsg(INFO_RESP, 0);
	msg->seq = this->seq;
	messages.push_back(*msg);
}

void NetServer::addmsgJoinAcc(NetServerClientInfo *client)
{
	NetMsg * msg = new NetMsg(JOIN_OKAY, 2);
	msg->seq = this->seq;
	
	Uint8* ptr = msg->data;
	SDLNet_Write16((Uint16) client->slot, ptr); ptr += 2;
	
	messages.push_back(*msg);
}

void NetServer::addmsgJoinRej()
{
}

void NetServer::addmsgDataCompl()
{
}

void NetServer::addmsgChat()
{
}

/**
* A unit has been added
**/
void NetServer::addmsgUnitAdd(Unit *u)
{
	NetMsg * msg = new NetMsg(UNIT_ADD, 30);
	msg->seq = this->seq;
	msg->uniq = u->slot;
	
	cout << "       addmsgUnitAdd()\n";
	cout << "       slot: " << u->slot << "\n";
	
	btTransform trans;
	u->body->getMotionState()->getWorldTransform(trans);
	btQuaternion q = trans.getRotation();
	btVector3 b = trans.getOrigin();
	
	pack(msg->data, "h ffff fff",
		u->slot,
		q.x(), q.y(), q.z(), q.w(),
		b.x(), b.y(), b.z()
	);
	
	messages.push_back(*msg);
}

/**
* A unit has been updated
**/
void NetServer::addmsgUnitUpdate(Unit *u)
{
	messages.remove_if(IsTypeUniqPred(UNIT_UPDATE, u->slot));
	
	cout << "       addmsgUnitUpdate()\n";
	cout << "       slot: " << u->slot << "\n";
	
	NetMsg * msg = new NetMsg(UNIT_UPDATE, 30);
	msg->seq = this->seq;
	msg->uniq = u->slot;
	
	btTransform trans;
	u->body->getMotionState()->getWorldTransform(trans);
	btQuaternion q = trans.getRotation();
	btVector3 b = trans.getOrigin();
	
	pack(msg->data, "h ffff fff",
		u->slot,
		q.x(), q.y(), q.z(), q.w(),
		b.x(), b.y(), b.z()
	);
	
	messages.push_back(*msg);
}

/**
* A unit has been removed
**/
void NetServer::addmsgUnitRem(Unit *u)
{
	NetMsg * msg = new NetMsg(UNIT_REM, 2);
	msg->seq = this->seq;
	
	cout << "       addmsgUnitRem()\n";
	cout << "       slot: " << u->slot << "\n";
	
	pack(msg->data, "h", u->slot);
	
	messages.push_back(*msg);
}

void NetServer::addmsgWallUpdate()
{
}

void NetServer::addmsgWallRem()
{
}

void NetServer::addmsgPlayerDrop()
{
}

void NetServer::addmsgPlayerQuit()
{
}



/**
***  One method for each incoming network message from the client
**/

unsigned int NetServer::handleInfoReq(NetServerClientInfo *client, Uint8 *data, unsigned int size)
{
	this->addmsgInfoResp();
	return 0;
}

unsigned int NetServer::handleJoinReq(NetServerClientInfo *client, Uint8 *data, unsigned int size)
{
	cout << "       handleJoinReq()\n";
	
	if (client->inlist) return 0;
	
	client->slot = this->clients.size() + 1;
	client->inlist = true;
	this->clients.push_back(client);
	
	this->addmsgJoinAcc(client);
	
	this->st->logic->raise_playerjoin(client->slot);
	
	return 0;
}

unsigned int NetServer::handleJoinAck(NetServerClientInfo *client, Uint8 *data, unsigned int size)
{
	cout << "       handleJoinAck()\n";
	return 0;
}

unsigned int NetServer::handleChat(NetServerClientInfo *client, Uint8 *data, unsigned int size)
{
	cout << "       handleChat()\n";
	return 0;
}

unsigned int NetServer::handleKeyMouseStatus(NetServerClientInfo *client, Uint8 *data, unsigned int size)
{
	cout << "       handleKeyMouseStatus()\n";
	
	Player *p = (Player*) st->findUnitSlot(client->slot);
	if (p == NULL) return 7;
	
	Sint16 x, y, delta;
	Uint8 keys;
	
	unpack(data, "hhhc", &x, &y, &delta, &keys);
	
	cout << "       x: " << x << "\n";
	cout << "       y: " << y << "\n";
	cout << "       delta: " << delta << "\n";
	p->angleFromMouse(x, y, delta);
	
	for (int i = 0; i < 8; i++) {
		cout << "       bit " << i << ": " << (keys & (1 << i)) << "\n";
	}
	
	p->setKeys(keys);	// TODO: Use 16-bits
	
	return 7;
}

unsigned int NetServer::handleQuit(NetServerClientInfo *client, Uint8 *data, unsigned int size)
{
	cout << "       handleQuit()\n";
	return 0;
}



