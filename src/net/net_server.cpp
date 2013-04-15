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
	
	this->seq = 1;
	this->seq_pred = new NetServerSeqPred(this);

	if (SDLNet_Init() < 0) {
		reportFatalError("SDL_net init failed");
	}
}

NetServer::~NetServer()
{
	if (this->sock != NULL) SDLNet_UDP_Close(this->sock);
}


/**
* Used for filtering
**/
static bool ClientEraser(NetServerClientInfo* c)
{
	if (c->del == false) return false;
	delete c;
	return true;
}


/**
* Some time has passed, do stuff
**/
void NetServer::update()
{
	UDPpacket *pkt = SDLNet_AllocPacket(1024);
	
	
	// Only update seq if we have clients
	if (this->clients.size() > 0) {
		this->seq++;
	}
	
	
	// We always recv. messages, so we can handle client joins
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
		for (vector<NetServerClientInfo*>::iterator cli = this->clients.begin(); cli != this->clients.end(); cli++) {
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
	
	
	// Check the seq of all clients
	// If they are too old, assume lost network connection
	if (this->clients.size() > 0) {
		for (vector<NetServerClientInfo*>::iterator cli = this->clients.begin(); cli != this->clients.end(); cli++) {
			if ((*cli)->inlist && (*cli)->ingame && (this->seq - (*cli)->seq) > MAX_SEQ_LAG) {
				this->dropClient(*cli);
			}
		}
		
		this->clients.erase(
			std::remove_if(this->clients.begin(), this->clients.end(), ClientEraser),
			this->clients.end()
		);
	}
	
	
	// Only send messages if we have clients
	if (this->clients.size() > 0) {
		// Debugging
		cout << setw (6) << setfill(' ') << st->game_time << " MSG-QUEUE\n";
		for (list<NetMsg>::iterator it = this->messages.begin(); it != this->messages.end(); it++) {
			cout << "       " << setw (6) << setfill(' ') << ((*it).seq) << " " << ((*it).type);
			dumpPacket((*it).data, (*it).size);
		}
		
		// Debugging
		cout << setw (6) << setfill(' ') << st->game_time << " CLIENT-INFO\n";
		for (vector<NetServerClientInfo*>::iterator cli = this->clients.begin(); cli != this->clients.end(); cli++) {
			cout << "       " << setw (6) << setfill(' ') << ((*cli)->seq) << " " << ((*cli)->slot) << "\n";
		}
		
		// Send messages
		for (vector<NetServerClientInfo*>::iterator cli = this->clients.begin(); cli != this->clients.end(); cli++) {
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
				if ((*it).dest != NULL && (*it).dest != (*cli)) continue;
			
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
	if (this->sock == NULL) {
		reportFatalError(SDLNet_GetError());
	}
}


/**
* Drop a client
**/
void NetServer::dropClient(NetServerClientInfo *client)
{
	this->st->logic->raise_playerleave(client->slot);
	
	Unit * u = this->st->findUnitSlot(client->slot);
	if (u) {
		u->takeDamage(u->getHealth());
	}
	
	this->addmsgClientDrop(client);
	
	client->inlist = false;
	client->ingame = false;
	client->del = true;
}



/**
***  One method for each outgoing network message the server sends out
**/

NetMsg * NetServer::addmsgInfoResp()
{
	NetMsg * msg = new NetMsg(INFO_RESP, 0);
	msg->seq = this->seq;
	messages.push_back(*msg);
	return msg;
}

NetMsg * NetServer::addmsgJoinAcc(NetServerClientInfo *client)
{
	string map = this->st->map->getName();
	
	NetMsg * msg = new NetMsg(JOIN_OKAY, 4 + map.length());
	msg->seq = this->seq;
	
	pack(msg->data, "hs",
		client->slot, map.c_str()
	);
	
	cout << "       Sent slot of: " << client->slot << "\n";
	
	messages.push_back(*msg);
	return msg;
}

NetMsg * NetServer::addmsgJoinRej()
{
	return NULL;
}

NetMsg * NetServer::addmsgDataCompl()
{
	NetMsg * msg = new NetMsg(JOIN_DONE, 0);
	msg->seq = this->seq;
	messages.push_back(*msg);
	return msg;
}

NetMsg * NetServer::addmsgChat()
{
	return NULL;
}

NetMsg * NetServer::addmsgClientDrop(NetServerClientInfo *client)
{
	messages.remove_if(IsTypeUniqPred(PLAYER_DROP, client->slot));
	
	NetMsg * msg = new NetMsg(PLAYER_DROP, 2);
	msg->seq = this->seq;
	msg->uniq = client->slot;
	
	pack(msg->data, "h",
		client->slot
	);
	
	cout << "       Dropped client: " << client->slot << "\n";
	
	messages.push_back(*msg);
	return msg;
}


/**
* A unit has been updated
**/
NetMsg * NetServer::addmsgUnitState(Unit *u)
{
	messages.remove_if(IsTypeUniqPred(UNIT_STATE, u->eid));
	
	NetMsg * msg = new NetMsg(UNIT_STATE, 36);
	msg->seq = this->seq;
	msg->uniq = u->eid;
	
	btTransform trans = u->getTransform();
	btQuaternion q = trans.getRotation();
	btVector3 b = trans.getOrigin();
	
	pack(msg->data, "hh ffff fff f",
		u->eid, u->slot,
		q.x(), q.y(), q.z(), q.w(),
		b.x(), b.y(), b.z(),
		u->health
	);
	
	messages.push_back(*msg);
	return msg;
}


/**
* A wall has been updated
**/
NetMsg * NetServer::addmsgWallState(Wall *w)
{
	messages.remove_if(IsTypeUniqPred(WALL_STATE, w->eid));
	
	NetMsg * msg = new NetMsg(WALL_STATE, 30);
	msg->seq = this->seq;
	msg->uniq = w->eid;
	
	btTransform trans = w->getTransform();
	btQuaternion q = trans.getRotation();
	btVector3 b = trans.getOrigin();
	
	pack(msg->data, "h ffff fff",
		w->eid,
		q.x(), q.y(), q.z(), q.w(),
		b.x(), b.y(), b.z()
	);
	
	messages.push_back(*msg);
	return msg;
}


/**
* An object has been updated
**/
NetMsg * NetServer::addmsgObjectState(Object *o)
{
	messages.remove_if(IsTypeUniqPred(OBJECT_STATE, o->eid));
	
	NetMsg * msg = new NetMsg(OBJECT_STATE, 30);
	msg->seq = this->seq;
	msg->uniq = o->eid;
	
	btTransform trans = o->getTransform();
	btQuaternion q = trans.getRotation();
	btVector3 b = trans.getOrigin();
	
	pack(msg->data, "h ffff fff",
		o->eid,
		q.x(), q.y(), q.z(), q.w(),
		b.x(), b.y(), b.z()
	);
	
	messages.push_back(*msg);
	return msg;
}


/**
* A vehicle has been updated
**/
NetMsg * NetServer::addmsgVehicleState(Vehicle *v)
{
	messages.remove_if(IsTypeUniqPred(VEHICLE_STATE, v->eid));
	
	NetMsg * msg = new NetMsg(VEHICLE_STATE, 42);
	msg->seq = this->seq;
	msg->uniq = v->eid;
	
	btTransform trans = v->getTransform();
	btQuaternion q = trans.getRotation();
	btVector3 b = trans.getOrigin();
	
	pack(msg->data, "h ffff fff fff",
		v->eid,
		q.x(), q.y(), q.z(), q.w(),
		b.x(), b.y(), b.z(),
		v->engineForce, v->brakeForce, v->steering
	);
	
	messages.push_back(*msg);
	return msg;
}


/**
* An entity has been removed
**/
NetMsg * NetServer::addmsgEntityRem(Entity *e)
{
	NetMsg * msg = new NetMsg(ENTITY_REM, 2);
	msg->seq = this->seq;
	
	cout << "       addmsgEntityRem()\n";
	
	pack(msg->data, "h", e->eid);
	
	messages.push_back(*msg);
	return msg;
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
	client->seq = this->seq;
	this->clients.push_back(client);
	
	this->addmsgJoinAcc(client);

	return 0;
}

unsigned int NetServer::handleJoinAck(NetServerClientInfo *client, Uint8 *data, unsigned int size)
{
	cout << "       handleJoinAck()\n";

	NetMsg* msg;
	for (list<Entity*>::iterator it = st->entities.begin(); it != st->entities.end(); it++) {
		Entity *e = (*it);
		
		switch (e->klass()) {
			case UNIT:
				msg = this->addmsgUnitState((Unit*)e);
				msg->dest = client;
				break;
				
			case WALL:
				msg = this->addmsgWallState((Wall*)e);
				msg->dest = client;
				break;

			case OBJECT:
				msg = this->addmsgObjectState((Object*)e);
				msg->dest = client;
				break;

			case VEHICLE:
				msg = this->addmsgVehicleState((Vehicle*)e);
				msg->dest = client;
				break;

			default:
				break;
		}
	}
	
	this->addmsgDataCompl();

	return 0;
}

unsigned int NetServer::handleJoinDone(NetServerClientInfo *client, Uint8 *data, unsigned int size)
{
	client->ingame = true;

	this->st->logic->raise_playerjoin(client->slot);

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
	cout << "       " << client->slot << "\n";
	
	Sint16 x, y, delta;
	Uint8 keys;
	
	unpack(data, "hhhc",
		&x, &y, &delta, &keys
	);
	
	// Find the unit for this slot
	Player *p = (Player*) st->findUnitSlot(client->slot);
	if (p == NULL) return 7;
	
	// Update the unit details
	p->angleFromMouse(x, y, delta);
	p->setKeys(keys);
	
	return 7;
}

unsigned int NetServer::handleQuit(NetServerClientInfo *client, Uint8 *data, unsigned int size)
{
	cout << "       handleQuit()\n";
	return 0;
}



