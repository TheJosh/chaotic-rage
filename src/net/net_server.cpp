// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "net_server.h"

#include <iostream>
#include <algorithm>
#include <SDL_net.h>

#include "net.h"
#include "../util/serverconfig.h"
#include "../rage.h"
#include "../game_engine.h"
#include "../game_state.h"
#include "../map/map.h"
#include "../lua/gamelogic.h"
#include "../entity/ammo_round.h"
#include "../entity/entity.h"
#include "../entity/object.h"
#include "../entity/pickup.h"
#include "../entity/unit.h"
#include "../entity/player.h"
#include "../entity/vehicle.h"
#include "../entity/wall.h"
#include "../mod/mod.h"
#include "../mod/mod_manager.h"
#include "../mod/unittype.h"
#include "../mod/walltype.h"
#include "../mod/objecttype.h"
#include "../mod/pickuptype.h"
#include "../mod/weapontype.h"
#include "../render_opengl/hud_label.h"

using namespace std;


NetServer::NetServer(GameState * st, ServerConfig * conf)
{
	this->st = st;
	this->conf = conf;

	this->seq = 1;
	this->seq_pred = new NetServerSeqPred(this);

	if (SDLNet_Init() < 0) {
		reportFatalError("SDL_net init failed");
	}
}


NetServer::~NetServer()
{
	if (this->sock != NULL) SDLNet_UDP_Close(this->sock);

	delete(this->seq_pred);

	this->clients.clear();
	this->messages.clear();
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
* TODO: Break out code to sub-functions
**/
bool NetServer::update()
{
	UDPpacket *pkt = SDLNet_AllocPacket(MAX_PKT_SIZE);


	// Only update seq if we have clients
	if (!this->clients.empty()) {
		this->seq++;
	}


	// We always recv. messages, so we can handle client joins
	while (SDLNet_UDP_Recv(this->sock, pkt)) {
		if (debug_enabled("net_pkt")) {
			cout << setw (6) << setfill(' ') << st->game_time << " RECV ";
			dumpPacket(pkt->data, pkt->len);
		}

		Uint8* ptr = pkt->data;
		int p = 0;

		SeqNum newseq = SDLNet_Read16(ptr);
		ptr += 2; p += 2;

		Uint16 code = SDLNet_Read16(ptr);
		ptr += 2; p += 2;

		// Handle a "browse" packet by echoing back
		if (pkt->len == 4 && newseq == 0x0000 && code == 0xFFFF) {
			SDLNet_UDP_Send(this->sock, -1, pkt);
			continue;
		}

		// Find the appropriate client
		NetServerClientInfo *client = NULL;
		for (vector<NetServerClientInfo*>::iterator cli = this->clients.begin(); cli != this->clients.end(); ++cli) {
			if ((*cli) != NULL && (*cli)->ipaddress.host == pkt->address.host && (*cli)->code == code) {
				client = (*cli);
				break;
			}
		}

		if (client == NULL) {
			// Create a new client if required
			client = new NetServerClientInfo();
			client->ipaddress.host = pkt->address.host;
			client->ipaddress.port = pkt->address.port;
			client->code = code;
			client->seq = newseq;
		} else if (newseq > client->seq) {
			// Update their seq
			client->seq = newseq;
		}

		// Handle messages
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
	


	// Send all messages to all active clients
	if (!this->clients.empty()) {
		// Check the seq of all clients
		// If they are too old, assume lost network connection
		for (vector<NetServerClientInfo*>::iterator cli = this->clients.begin(); cli != this->clients.end(); ++cli) {
			if ((*cli)->inlist && (*cli)->ingame && (this->seq - (*cli)->seq) > MAX_SEQ_LAG) {
				this->dropClient(*cli);
			}
		}

		// Scrub any dropped clients from the array
		this->clients.erase(
			std::remove_if(this->clients.begin(), this->clients.end(), ClientEraser),
			this->clients.end()
		);

		// Dump debug info
		if (debug_enabled("net_info")) {
			cout << setw (6) << setfill(' ') << st->game_time << " MSG-QUEUE" << endl;
			for (list<NetMsg>::iterator it = this->messages.begin(); it != this->messages.end(); ++it) {
				cout << "       " << setw (6) << setfill(' ') << ((*it).seq) << " " << ((*it).type);
				dumpPacket((*it).data, (*it).size);
			}

			cout << setw (6) << setfill(' ') << st->game_time << " CLIENT-INFO" << endl;
			for (vector<NetServerClientInfo*>::iterator cli = this->clients.begin(); cli != this->clients.end(); ++cli) {
				cout << "       " << setw (6) << setfill(' ') << ((*cli)->seq) << " " << ((*cli)->slot) << endl;
			}
		}

		// Send messages
		// TODO: Think up a way to handle packets larger than internet MTU.
		// The current workaround is to allow sending packages larger
		// than MTU (if needed). Leads to package fragmentation.
		for (vector<NetServerClientInfo*>::iterator cli = this->clients.begin(); cli != this->clients.end(); ++cli) {
			if ((*cli) == NULL) continue;

			pkt->address.host = (*cli)->ipaddress.host;
			pkt->address.port = (*cli)->ipaddress.port;

			pkt->len = 0;

			Uint8* ptr = pkt->data;

			SDLNet_Write16(this->seq, ptr);
			ptr += 2; pkt->len += 2;

			SDLNet_Write16(0, ptr);		// pad
			ptr += 2; pkt->len += 2;
			
			int emptyPktLen = pkt->len;

			for (list<NetMsg>::iterator it = this->messages.begin(); it != this->messages.end(); ++it) {
				if ((*cli)->seq > (*it).seq) continue;
				if ((*it).dest != NULL && (*it).dest != (*cli)) continue;

				unsigned int nextPktLen = pkt->len + 1 + (*it).size;
				if (nextPktLen >= MAX_PKT_SIZE) {
					cout << "Error: Server: Too many messages. Shutting down." << endl;
					SDLNet_FreePacket(pkt);
					return false;
				}
				assert(nextPktLen <= MAX_PKT_SIZE);

				*ptr = (*it).type;
				ptr++; pkt->len++;

				memcpy(ptr, (*it).data, (*it).size);
				ptr += (*it).size; pkt->len += (*it).size;
			}

			if (pkt->len > emptyPktLen) {
				if (debug_enabled("net_pkt")) {
					cout << setw (6) << setfill(' ') << st->game_time << " SEND ";
					dumpPacket(pkt->data, pkt->len);
				}

				SDLNet_UDP_Send(this->sock, -1, pkt);
			}
		}
	}

	this->messages.remove_if(*this->seq_pred);
	SDLNet_FreePacket(pkt);
	return true;
}


/**
* Set the port for listening for connections
**/
void NetServer::listen()
{
	SDLNet_ResolveHost(&this->ipaddress, NULL, this->conf->port);

	this->sock = SDLNet_UDP_Open(this->conf->port);
	if (this->sock == NULL) {
		reportFatalError(SDLNet_GetError());
	}
}


/**
* Drop a client
**/
void NetServer::dropClient(NetServerClientInfo *client)
{
	Unit * u = this->st->findUnitSlot(client->slot);
	if (u) {
		u->del = true;
	}

	this->st->logic->raise_playerleave(client->slot);
	this->addmsgClientDrop(client);

	client->inlist = false;
	client->ingame = false;
	client->del = true;
}



/**
***  One method for each outgoing network message the server sends out
**/

void NetServer::addmsgInfoResp()
{
	//cout << "INFO_RESP" << endl;
	string mod = GEng()->mm->getSupplOrBase()->getName();
	string map = this->st->map->getName();

	NetMsg msg(INFO_RESP, mod.length() + 2 + map.length() + 2);
	msg.seq = this->seq;

	pack(msg.data, "ss",
		mod.c_str(), map.c_str()
	);

	messages.push_back(msg);
}

void NetServer::addmsgJoinAcc(NetServerClientInfo *client)
{
	//cout << "JOIN_OKAY" << endl;
	string map = this->st->map->getName();

	NetMsg msg(JOIN_OKAY, 2 + map.length() + 2);
	msg.seq = this->seq;

	pack(msg.data, "hs",
		client->slot, map.c_str()
	);

	cout << "       Sent slot of: " << client->slot << "  map: " << map << endl;

	messages.push_back(msg);
}

void NetServer::addmsgJoinRej()
{
	// TODO: Implement
}

void NetServer::addmsgDataCompl()
{
	//cout << "JOIN_DONE" << endl;
	NetMsg msg(JOIN_DONE, 0);
	msg.seq = this->seq;
	messages.push_back(msg);
}

void NetServer::addmsgChat()
{
	// TODO: Implement
}

void NetServer::addmsgHUD(HUDLabel *l)
{
	//cout << "HUD: " << l->data << endl;
	// TODO: Implement a proper unique check
	messages.remove_if(IsTypeUniqPred(HUD_MSG, static_cast<int>(l->x + l->y)));

	NetMsg msg(HUD_MSG, 4*2 + l->data.length()+2 + 2 + 4*4);
	msg.seq = this->seq;
	msg.uniq = static_cast<int>(l->x + l->y);

	pack(msg.data, "ffs hffff",
		l->x, l->y, l->data.c_str(),
		l->align, l->r, l->g, l->b, l->a
	);

	messages.push_back(msg);
}

void NetServer::addmsgClientDrop(NetServerClientInfo *client)
{
	//cout << "PLAYER_DROP" << endl;
	messages.remove_if(IsTypeUniqPred(PLAYER_DROP, client->slot));

	NetMsg msg(PLAYER_DROP, 2);
	msg.seq = this->seq;
	msg.uniq = client->slot;

	pack(msg.data, "h",
		client->slot
	);

	cout << "       Dropped client: " << client->slot << endl;

	messages.push_back(msg);
}


/**
* A unit has been updated
**/
void NetServer::addmsgUnitState(Unit *u)
{
	//cout << "UNIT_STATE" << endl;
	messages.remove_if(IsTypeUniqPred(UNIT_STATE, u->eid));

	NetMsg msg(UNIT_STATE, 40);
	msg.seq = this->seq;
	msg.uniq = u->eid;

	btTransform trans = u->getTransform();
	btQuaternion q = trans.getRotation();
	btVector3 b = trans.getOrigin();

	pack(msg.data, "hhl ffff fff f",
		u->eid, u->slot, u->uc->id,
		q.x(), q.y(), q.z(), q.w(),
		b.x(), b.y(), b.z(),
		u->health
	);

	messages.push_back(msg);
}


/**
* A wall has been updated
**/
void NetServer::addmsgWallState(Wall *w)
{
	//cout << "WALL_STATE" << endl;
	messages.remove_if(IsTypeUniqPred(WALL_STATE, w->eid));

	NetMsg msg(WALL_STATE, 34);
	msg.seq = this->seq;
	msg.uniq = w->eid;

	btTransform trans = w->getTransform();
	btQuaternion q = trans.getRotation();
	btVector3 b = trans.getOrigin();

	pack(msg.data, "hl ffff fff",
		w->eid, w->wt->id,
		q.x(), q.y(), q.z(), q.w(),
		b.x(), b.y(), b.z()
	);

	messages.push_back(msg);
}


/**
* An object has been updated
**/
void NetServer::addmsgObjectState(Object *o)
{
	//cout << "OBJECT_STATE" << endl;
	messages.remove_if(IsTypeUniqPred(OBJECT_STATE, o->eid));

	NetMsg msg(OBJECT_STATE, 34);
	msg.seq = this->seq;
	msg.uniq = o->eid;

	btTransform trans = o->getTransform();
	btQuaternion q = trans.getRotation();
	btVector3 b = trans.getOrigin();

	pack(msg.data, "hl ffff fff",
		o->eid, o->ot->id,
		q.x(), q.y(), q.z(), q.w(),
		b.x(), b.y(), b.z()
	);

	messages.push_back(msg);
}


/**
* A vehicle has been updated
**/
void NetServer::addmsgVehicleState(Vehicle *v)
{
	//cout << "VEHICLE_STATE" << endl;
	messages.remove_if(IsTypeUniqPred(VEHICLE_STATE, v->eid));

	NetMsg msg(VEHICLE_STATE, 34);
	msg.seq = this->seq;
	msg.uniq = v->eid;

	btTransform trans = v->getTransform();
	btQuaternion q = trans.getRotation();
	btVector3 b = trans.getOrigin();

	pack(msg.data, "hl ffff fff",
		v->eid, v->vt->id,
		q.x(), q.y(), q.z(), q.w(),
		b.x(), b.y(), b.z()
	);

	messages.push_back(msg);
}


/**
* An ammoround has been updated
**/
void NetServer::addmsgAmmoRoundState(AmmoRound *ar)
{
	//cout << "AMMOROUND_STATE" << endl;
	messages.remove_if(IsTypeUniqPred(AMMOROUND_STATE, ar->eid));

	//cout << "       addmsgAmmoRoundState()" << endl;

	NetMsg msg(AMMOROUND_STATE, 40);
	msg.seq = this->seq;
	msg.uniq = ar->eid;

	btTransform trans = ar->getTransform();
	btQuaternion q = trans.getRotation();
	btVector3 b = trans.getOrigin();

	pack(msg.data, "hhl ffff fff f",
		ar->eid, ar->owner->eid, ar->wt->id,
		q.x(), q.y(), q.z(), q.w(),
		b.x(), b.y(), b.z(),
		ar->mass
	);

	messages.push_back(msg);
}


/**
* A pickup has been updated
**/
void NetServer::addmsgPickupState(Pickup *p)
{
	//cout << "PICKUP_STATE" << endl;
	messages.remove_if(IsTypeUniqPred(PICKUP_STATE, p->eid));

	NetMsg msg(PICKUP_STATE, 34);
	msg.seq = this->seq;
	msg.uniq = p->eid;

	btTransform trans = p->getTransform();
	btQuaternion q = trans.getRotation();
	btVector3 b = trans.getOrigin();

	pack(msg.data, "hl ffff fff",
		p->eid, p->getPickupType()->id,
		q.x(), q.y(), q.z(), q.w(),
		b.x(), b.y(), b.z()
	);

	messages.push_back(msg);
}


/**
* An entity has been removed
**/
void NetServer::addmsgEntityRem(Entity *e)
{
	//cout << "ENTITY_REM" << endl;
	NetMsg msg(ENTITY_REM, 2);
	msg.seq = this->seq;

	cout << "       addmsgEntityRem()  klass: " << e->klass() << "  eid: " << e->eid << endl;

	pack(msg.data, "h", e->eid);

	messages.push_back(msg);
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
	//cout << "       handleJoinReq()" << endl;

	if (client->inlist) return 0;

	client->slot = this->st->num_local + this->clients.size() + 1;
	client->inlist = true;
	client->seq = this->seq;
	this->clients.push_back(client);

	this->addmsgJoinAcc(client);

	return 0;
}

unsigned int NetServer::handleJoinAck(NetServerClientInfo *client, Uint8 *data, unsigned int size)
{
	cout << "       handleJoinAck() from slot " << client->slot << endl;

	for (list<Entity*>::iterator it = st->entities.begin(); it != st->entities.end(); ++it) {
		Entity *e = (*it);

		switch (e->klass()) {
			case UNIT:
				this->addmsgUnitState(static_cast<Unit*>(e));
				(*(messages.end()--)).dest = client;
				break;

			case WALL:
				this->addmsgWallState(static_cast<Wall*>(e));
				(*(messages.end()--)).dest = client;
				break;

			case OBJECT:
				this->addmsgObjectState(static_cast<Object*>(e));
				(*(messages.end()--)).dest = client;
				break;

			case VEHICLE:
				this->addmsgVehicleState(static_cast<Vehicle*>(e));
				(*(messages.end()--)).dest = client;
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
	cout << "       handleJoinDone() from slot " << client->slot << endl;

	client->ingame = true;

	this->st->logic->raise_playerjoin(client->slot);

	return 0;
}

unsigned int NetServer::handleChat(NetServerClientInfo *client, Uint8 *data, unsigned int size)
{
	cout << "       handleChat() from slot " << client->slot << endl;
	return 0;
}

unsigned int NetServer::handleKeyMouseStatus(NetServerClientInfo *client, Uint8 *data, unsigned int size)
{
	Sint16 x, y, delta;
	Uint8 keys;

	unpack(data, "hhhc",
		&x, &y, &delta, &keys
	);

	// Find the unit for this slot
	Player *p = static_cast<Player*>(st->findUnitSlot(client->slot));
	if (p == NULL) return 7;

	// Update the unit details
	p->angleFromMouse(x, y, delta);
	p->setKeys(keys);

	return 7;
}

unsigned int NetServer::handleQuit(NetServerClientInfo *client, Uint8 *data, unsigned int size)
{
	cout << "       handleQuit() from slot " << client->slot << endl;
	this->dropClient(client);
	return 0;
}
