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

	if (SDLNet_Init() < 0) {
		reportFatalError("SDL_net init failed");
	}
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
		
		SDLNet_Read16(ptr);		// pad
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
* Try to join a game
**/
bool NetClient::attemptJoinGame(string address, int port)
{
	this->bind(address, port);
	this->addmsgJoinReq();

	// Wait up to two seconds to be allocated a slot
	for (int i = 0; i < 20; i++) {
		this->update();
		if (st->local_players[0]->slot != 0) return true;
		SDL_Delay(100);
	}

	return false;
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
	
	pack(msg->data, "hhhc",
		(Sint16)x, (Sint16)y, (Sint16)delta, k
	);
	
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
	
	int slot;
	unpack(data, "h",
		&slot
	);
	
	if (st->local_players[0]->slot == 0) {
		st->local_players[0]->slot = slot;
		cout << "       Our slot: " << slot << "\n";
	}
	
	this->addmsgJoinAck();
	
	return 2;
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


unsigned int NetClient::handleUnitState(Uint8 *data, unsigned int size)
{
	cout << "       handleUnitState()\n";
	
	
	short eid, slot;
	float qx, qy, qz, qw, bx, by, bz;
	
	unpack(data, "hh ffff fff",
		&eid, &slot,
		&qx, &qy, &qz, &qw,
		&bx, &by, &bz
	);
	
	Unit* u = (Unit*) st->getEntity(eid);
	
	// If don't exist, create
	if (u == NULL) {
		UnitType *ut = st->mm->getUnitType("robot");
		u = new Player(ut, st, bx, bz, by);
		u->slot = slot;
		
		// Give them some weapons
		for (unsigned int i = 0; i < ut->spawn_weapons.size(); i++) {
			u->pickupWeapon(ut->spawn_weapons.at(i));
		}
		
		// If the player is this client, save in the local_players obj
		if (st->local_players[0]->slot == u->slot) {
			st->local_players[0]->p = (Player*)u;
		}
		
		st->addUnit(u);
		u->eid = eid;
	}
	
	// Update the transform
	u->body->getMotionState()->setWorldTransform(btTransform(
		btQuaternion(qx, qy, qz, qw),
		btVector3(bx, by, bz)
	));
	
	return 32;
}

unsigned int NetClient::handleWallState(Uint8 *data, unsigned int size)
{
	cout << "       handleWallState()\n";
	return 0;
}

unsigned int NetClient::handleObjectState(Uint8 *data, unsigned int size)
{
	cout << "       handleObjectState()\n";
	return 0;
}

unsigned int NetClient::handleVehicleState(Uint8 *data, unsigned int size)
{
	cout << "       handleVehicleState()\n";
	return 0;
}

unsigned int NetClient::handleEntityRem(Uint8 *data, unsigned int size)
{
	cout << "       handleEntityRem()\n";
	
	short slot;
	
	unpack(data, "h", &slot);
	
	Player *p = (Player*) st->findUnitSlot(slot);
	if (p == NULL) return 2;
	
	p->del = true;
	
	return 2;
}



