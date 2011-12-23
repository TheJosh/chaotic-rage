// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <SDL_net.h>
#include "../rage.h"
#include "net.h"

using namespace std;


NetClient::NetClient(GameState * st)
{
	this->st = st;
	st->client = this;
	this->sock = NULL;
	
	this->seq = 0;
	this->seq_pred = new NetClientSeqPred(this);
	
	this->code = getRandom(0, 32700);
}

NetClient::~NetClient()
{
	if (this->sock != NULL) SDLNet_UDP_Close(this->sock);
}


/**
* Some time has passed, do stuff
**/
void NetClient::update()
{
	UDPpacket *pkt = SDLNet_AllocPacket(1000);
	
	
	while (SDLNet_UDP_Recv(this->sock, pkt)) {
		cout << setw (6) << setfill(' ') << st->game_time << " RECV ";
		dumpPacket(pkt->data, pkt->len);
		
		Uint8* ptr = pkt->data;
		int p = 0;
		
		SeqNum newseq = SDLNet_Read16(ptr);
		if (newseq > this->seq) {
			this->seq = newseq;
			cout << "       The server has sent " << newseq << ", will ACK.\n";
		}
		ptr += 2; p += 2;
		
		while (p < pkt->len) {
			unsigned int type = (*ptr);
			ptr++; p++;
			
			if (type > NOTHING && type < BOTTOM) {
				if (msg_client_recv[type] != NULL) {
					unsigned int num = ((*this).*(msg_client_recv[type]))(ptr, pkt->len - p);
					ptr += num; p += num;
				}
			}
		}
	}
	
	
	pkt->address = this->ipaddress;
	pkt->len = 0;
	
	Uint8* ptr = pkt->data;
	
	SDLNet_Write16(this->seq, ptr);
	ptr += 2; pkt->len += 2;
	
	SDLNet_Write16(this->code, ptr);
	ptr += 2; pkt->len += 2;
	
	for (list<NetMsg>::iterator it = this->messages.begin(); it != this->messages.end(); it++) {
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
	
	
	this->messages.remove_if(*this->seq_pred);
	
	//SDLNet_FreePacket(pkt);
}


/**
* Set the address and port for sending messages
**/
void NetClient::bind(string address, int port)
{
	SDLNet_ResolveHost(&this->ipaddress, address.c_str(), port);
	this->sock = SDLNet_UDP_Open(0);
}




/**
***  One method for each outgoing network message the client sends out
**/

void NetClient::addmsgInfoReq() {
	NetMsg * msg = new NetMsg(INFO_REQ, 0);
	msg->seq = this->seq;
	messages.push_back(*msg);
}

void NetClient::addmsgJoinReq() {
	NetMsg * msg = new NetMsg(JOIN_REQ, 0);
	msg->seq = this->seq;
	messages.push_back(*msg);
}

void NetClient::addmsgJoinAck() {
	NetMsg * msg = new NetMsg(JOIN_ACK, 0);
	msg->seq = this->seq;
	messages.push_back(*msg);
}

void NetClient::addmsgChat() {
	NetMsg * msg = new NetMsg(CHAT_REQ, 0);
	msg->seq = this->seq;
	messages.push_back(*msg);
}

void NetClient::addmsgKeyMouseStatus(int x, int y, int delta, Uint8 k)
{
	cout << "       addmsgKeyMouseStatus(" << x << ", " << y << ", " << delta << ")\n";
	
	NetMsg * msg = new NetMsg(CLIENT_STATE, 7);
	msg->seq = this->seq;
	
	// TODO: k should be 16-bit
	pack(msg->data, "hhhc", (Sint16)x, (Sint16)y, (Sint16)delta, k);
	
	messages.push_back(*msg);
}

void NetClient::addmsgQuit() {
	NetMsg * msg = new NetMsg(QUIT_REQ, 0);
	msg->seq = this->seq;
	messages.push_back(*msg);
}



/**
***  One method for each incoming network message from the server
**/

unsigned int NetClient::handleInfoResp(Uint8 *data, unsigned int size)
{
	cout << "       handleInfoResp()\n";
	return 0;
}

unsigned int NetClient::handleJoinAcc(Uint8 *data, unsigned int size)
{
	cout << "       handleJoinAcc()\n";
	
	int slot = SDLNet_Read16(data);
	st->curr_slot = slot;
	
	cout << "       Our slot: " << slot << "\n";
	
	this->addmsgJoinAck();
	
	return 0;
}

unsigned int NetClient::handleJoinRej(Uint8 *data, unsigned int size)
{
	cout << "       handleJoinRej()\n";
	return 0;
}

unsigned int NetClient::handleDataCompl(Uint8 *data, unsigned int size)
{
	cout << "       handleDataCompl()\n";
	return 0;
}

unsigned int NetClient::handleChat(Uint8 *data, unsigned int size)
{
	cout << "       handleChat()\n";
	return 0;
}

unsigned int NetClient::handleUnitAdd(Uint8 *data, unsigned int size)
{
	cout << "       handleUnitAdd()\n";
	
	float x,y;
	int angle;
	short slot;
	Player *p;
	
	unpack(data, "fflh", &x, &y, &angle, &slot);
	
	p = (Player*) st->findUnitSlot(slot);
	if (p != NULL) return 14;
	
	UnitType *ut = st->getDefaultMod()->getUnitType(1);
	p = new Player(ut, st, x, y, 2);
	p->slot = slot;
	
	p->pickupWeapon(st->getDefaultMod()->getWeaponType(0));
	p->pickupWeapon(st->getDefaultMod()->getWeaponType(1));
	
	if (st->curr_slot == p->slot) {
		st->local_players[0] = p;
		st->hud->hideSpawnMenu();
	}
	
	st->addUnit(p);
	
	return 14;
}

unsigned int NetClient::handleUnitUpdate(Uint8 *data, unsigned int size)
{
	cout << "       handleUnitUpdate()\n";
	
	float x,y;
	int angle;
	float speed;
	short slot;
	
	unpack(data, "fflfh", &x, &y, &angle, &speed, &slot);
	
	Player *p = (Player*) st->findUnitSlot(slot);
	if (p == NULL) return 18;
	
	// TODO: Rebuild network for new physics
	/*
	cout << "       CURRENT\n";
	cout << "       x: " << p->x << "\n";
	cout << "       y: " << p->y << "\n";
	cout << "       a: " << p->angle << "\n";
	cout << "       s: " << p->speed << "\n";
	
	cout << "       FROM PKT\n";
	cout << "       x: " << x << "\n";
	cout << "       y: " << y << "\n";
	cout << "       a: " << angle << "\n";
	cout << "       s: " << speed << "\n";
	
	if (p != st->local_players[0] || speed == 0) {
		p->x = x;
		p->y = y;
		p->angle = angle;
		p->speed = speed;
	}
	*/
	
	return 18;
}

unsigned int NetClient::handleUnitRem(Uint8 *data, unsigned int size)
{
	cout << "       handleUnitRem()\n";
	
	short slot;
	
	unpack(data, "h", &slot);
	
	Player *p = (Player*) st->findUnitSlot(slot);
	if (p == NULL) return 2;
	
	p->del = true;
	
	return 2;
}

unsigned int NetClient::handleWallUpdate(Uint8 *data, unsigned int size)
{
	cout << "       handleWallUpdate()\n";
	return 0;
}

unsigned int NetClient::handleWallRem(Uint8 *data, unsigned int size)
{
	cout << "       handleWallRem()\n";
	return 0;
}

unsigned int NetClient::handlePgAdd(Uint8 *data, unsigned int size)
{
	cout << "       handlePgAdd()\n";
	return 0;
}

unsigned int NetClient::handlePgRem(Uint8 *data, unsigned int size)
{
	cout << "       handlePgRem()\n";
	return 0;
}

unsigned int NetClient::handlePlayerDrop(Uint8 *data, unsigned int size)
{
	cout << "       handlePlayerDrop()\n";
	return 0;
}

unsigned int NetClient::handlePlayerQuit(Uint8 *data, unsigned int size)
{
	cout << "       handlePlayerQuit()\n";
	return 0;
}



