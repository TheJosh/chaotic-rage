// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include "net_client.h"

#include <iostream>
#include <SDL_net.h>

#include "../rage.h"
#include "../game_state.h"
#include "../game_engine.h"
#include "../entity/ammo_round.h"
#include "../entity/entity.h"
#include "../entity/helicopter.h"
#include "../entity/object.h"
#include "../entity/pickup.h"
#include "../entity/unit.h"
#include "../entity/player.h"
#include "../entity/vehicle.h"
#include "../entity/wall.h"
#include "../mod/weapontype.h"
#include "../mod/mod_manager.h"
#include "../render_opengl/hud_label.h"
#include "../util/ui_update.h"
#include "net.h"
#include "net_gameinfo.h"

using namespace std;


NetClient::NetClient(GameState * st)
{
	this->st = st;
	this->sock = NULL;
	this->gameinfo = NULL;
	this->ingame = false;

	this->seq = 0;
	this->seq_pred = new NetClientSeqPred(this);

	this->code = getRandom(0, 32700);
	this->last_ack = st->game_time;

	if (SDLNet_Init() < 0) {
		reportFatalError("SDL_net init failed");
	}
}

NetClient::~NetClient()
{
	if (this->sock != NULL) SDLNet_UDP_Close(this->sock);

	delete(this->gameinfo);
	delete(this->seq_pred);

	this->messages.clear();
}


/**
* Some time has passed, do stuff
* TODO: Break out code to sub-functions
**/
void NetClient::update()
{
	UDPpacket *pkt = SDLNet_AllocPacket(MAX_PKT_SIZE);


	// Check the server is still chatting to us
	if (this->ingame && (st->game_time - this->last_ack) > 2500) {
		GEng()->setMouseGrab(false);
		displayMessageBox("Disconnected from server.");
		st->gameOver();
	}

	// Handle packets coming in
	while (SDLNet_UDP_Recv(this->sock, pkt)) {
		if (debug_enabled("net_pkt")) {
			cout << setw (6) << setfill(' ') << st->game_time << " RECV ";
			dumpPacket(pkt->data, pkt->len);
		}

		Uint8* ptr = pkt->data;
		int p = 0;

		SeqNum newseq = SDLNet_Read16(ptr);
		if (newseq > this->seq) {
			this->seq = newseq;
			this->last_ack = st->game_time;
		} else {
			// ignore old packages
			//cout << "old package" << endl;
			//continue;
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


	// Send messages
	pkt->address = this->ipaddress;
	pkt->len = 0;

	Uint8* ptr = pkt->data;

	SDLNet_Write16(this->seq, ptr);
	ptr += 2; pkt->len += 2;

	SDLNet_Write16(this->code, ptr);
	ptr += 2; pkt->len += 2;

	for (list<NetMsg>::iterator it = this->messages.begin(); it != this->messages.end(); ++it) {
		unsigned int futurePktLen = pkt->len + 1 + (*it).size;
		assert(futurePktLen <= MAX_PKT_SIZE);

		*ptr = (*it).type;
		ptr++; pkt->len++;

		memcpy(ptr, (*it).data, (*it).size);
		ptr += (*it).size; pkt->len += (*it).size;
	}

	if (pkt->len > 0) {
		if (debug_enabled("net_pkt")) {
			cout << setw (6) << setfill(' ') << st->game_time << " SEND ";
			dumpPacket(pkt->data, pkt->len);
		}

		SDLNet_UDP_Send(this->sock, -1, pkt);
	}

	this->messages.remove_if(*this->seq_pred);

	SDLNet_FreePacket(pkt);
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
* Try to join a game.
* Returns a GameInfo object, or NULL on failure.
**/
NetGameinfo * NetClient::attemptJoinGame(string address, int port, UIUpdate *ui)
{
	if (this->gameinfo) {
		delete(this->gameinfo);
		this->gameinfo = NULL;
	}

	this->bind(address, port);
	this->addmsgJoinReq();

	// Wait up to two seconds to be allocated a slot
	unsigned int now = SDL_GetTicks();
	do {
		this->update();
		if (this->gameinfo != NULL) break;
		if (ui) ui->updateUI();
	} while (SDL_GetTicks() - now < 2000);

	return this->gameinfo;
}


/**
* Downloads the game state from the server.
* Returns true on success or false on failure.
**/
bool NetClient::downloadGameState()
{
	// Wait for game data to arrive
	for (int i = 0; i < 20; i++) {
		this->update();
		if (this->ingame) break;
		SDL_Delay(100);
	}

	if (! this->ingame) return false;

	this->addmsgDataCompl();

	return true;
}


/**
* Called at engine start time
**/
void NetClient::preGame()
{
	this->last_ack = st->game_time;
}


/**
* Handle errors
* At the moment it just logs to stderr but could possibly be more proactive
**/
void NetClient::error(string msg)
{
	cerr << msg << endl;
}




/**
***  One method for each outgoing network message the client sends out
**/

void NetClient::addmsgInfoReq()
{
	DEBUGSTR("net", "INFO_REQ");
	NetMsg msg(INFO_REQ, 0);
	msg.seq = this->seq;
	messages.push_back(msg);
}

void NetClient::addmsgJoinReq()
{
	DEBUGSTR("net", "JOIN_REQ");
	NetMsg msg(JOIN_REQ, 0);
	msg.seq = this->seq;
	messages.push_back(msg);
}

void NetClient::addmsgJoinAck()
{
	DEBUGSTR("net", "JOIN_ACK");
	NetMsg msg(JOIN_ACK, 0);
	msg.seq = this->seq;
	messages.push_back(msg);
}

void NetClient::addmsgDataCompl()
{
	DEBUGSTR("net", "JOIN_DONE");
	NetMsg msg(JOIN_DONE, 0);
	msg.seq = this->seq;
	messages.push_back(msg);
}

void NetClient::addmsgChat()
{
	DEBUGSTR("net", "CHAT_REQ");
	NetMsg msg(CHAT_REQ, 0);
	msg.seq = this->seq;
	messages.push_back(msg);
}

void NetClient::addmsgKeyMouseStatus(int x, int y, int delta, Uint16 k)
{
	DEBUGSTR("net", "CLIENT_STATE");
	NetMsg msg(CLIENT_STATE, 8);
	msg.seq = this->seq;

	pack(msg.data, "hhhh",
		(Sint16)x, (Sint16)y, (Sint16)delta, k
	);

	messages.push_back(msg);
}

void NetClient::addmsgQuit()
{
	DEBUGSTR("net", "QUIT_REQ");
	NetMsg msg(QUIT_REQ, 0);
	msg.seq = this->seq;
	messages.push_back(msg);

	this->ingame = false;
}



/**
***  One method for each incoming network message from the server
**/

unsigned int NetClient::handleInfoResp(Uint8 *data, unsigned int size)
{
	DEBUGSTR("net", "handleInfoResp()");

	char mod[128];
	char map[128];

	unpack(data, "ss",
		&mod, &map
	);

	return strlen(mod) + 2 + strlen(map) + 2;
}

unsigned int NetClient::handleJoinAcc(Uint8 *data, unsigned int size)
{
	DEBUGSTR("net", "handleJoinAcc()");

	unsigned int slot = 0;
	char map[128];

	unpack(data, "hs",
		&slot, &map
	);

	if (st->local_players[0]->slot == 0) {
		st->local_players[0]->slot = slot;
		DEBUG("net", "  Allocated slot: %i", slot);
	}

	this->gameinfo = new NetGameinfo();
	this->gameinfo->map = std::string(map);

	this->last_ack = st->game_time;

	this->addmsgJoinAck();

	return 4 + strlen(map);
}

unsigned int NetClient::handleJoinRej(Uint8 *data, unsigned int size)
{
	DEBUGSTR("net", "handleJoinRej()");
	return 0;
}

unsigned int NetClient::handleDataCompl(Uint8 *data, unsigned int size)
{
	DEBUGSTR("net", "handleDataCompl()");
	this->ingame = true;
	return 0;
}

unsigned int NetClient::handleChat(Uint8 *data, unsigned int size)
{
	DEBUGSTR("net", "handleChat()");
	return 0;
}

unsigned int NetClient::handlePlayerDrop(Uint8 *data, unsigned int size)
{
	DEBUGSTR("net", "handlePlayerDrop()");

	unsigned int slot;
	unpack(data, "h",
		&slot
	);

	DEBUG("net", "  Slot: %i", slot);

	// Were we booted?
	if (st->local_players[0]->slot == slot) {
		st->gameOver();
		DEBUGSTR("net", "  It's us :(");
	}

	return 2;
}

unsigned int NetClient::handleHUD(Uint8 *data, unsigned int size)
{
	DEBUGSTR("net", "handleHUD()");

	int x, y;
	float r, g, b, a;
	HUDLabelAlign align;
	char buffer[512];

	unpack(data, "lls hffff",
		&x, &y, &buffer,
		&align, &r, &g, &b, &a
	);

	HUDLabel * l = new HUDLabel(x, y, string(buffer), align, r, g, b, a);
	st->addHUDLabel(st->local_players[0]->slot, x, y, "", l);

	return 4*2 + strlen(buffer)+2 + 2 + 4*4;
}


unsigned int NetClient::handleUnitState(Uint8 *data, unsigned int size)
{
	DEBUGSTR("net", "handleUnitState()");


	Uint16 eid, slot;
	CRC32 type;
	float qx, qy, qz, qw, bx, by, bz;
	float health;

	unpack(data, "hhl ffff fff f",
		&eid, &slot, &type,
		&qx, &qy, &qz, &qw,
		&bx, &by, &bz,
		&health
	);

	// TODO: drive, lift, current weapon, weapons

	Entity* e = st->getEntity(eid);
	Unit* u = (Unit*) e;

	// If don't exist, create
	if (u == NULL) {
		DEBUGSTR("net", "  Create unit");
		DEBUG("net", "  eid: %i  slot: %i", eid, slot);

		UnitType *ut = GEng()->mm->getUnitType(type);
		if (! ut) {
			this->error("Invalid unit type " + type);
			return 40;
		}

		u = new Player(ut, st, FACTION_INDIVIDUAL, slot, bx, by, bz);

		// If the player is this client, save in the local_players obj
		if (st->local_players[0]->slot == u->slot) {
			st->local_players[0]->p = (Player*)u;
			DEBUGSTR("net", "  It's us :)");
		}

		st->addUnit(u);
		u->eid = eid;
	}

	// Update the transform
	btTransform xform = btTransform(
		btQuaternion(qx, qy, qz, qw),
		btVector3(bx, by, bz)
	);
	u->setTransform(xform);

	u->health = health;

	return 40;
}

unsigned int NetClient::handleWallState(Uint8 *data, unsigned int size)
{
	DEBUGSTR("net", "handleWallState()");


	Uint16 eid;
	CRC32 type;
	float qx, qy, qz, qw, bx, by, bz;

	unpack(data, "hl ffff fff",
		&eid, &type,
		&qx, &qy, &qz, &qw,
		&bx, &by, &bz
	);

	Entity* e = st->getEntity(eid);
	Wall* w = (Wall*) e;

	// If don't exist, create
	if (w == NULL) {
		WallType *wt = GEng()->mm->getWallType(type);
		if (! wt) {
			this->error("Invalid wall type " + type);
			return 34;
		}

		w = new Wall(wt, st, bx, bz, by, 0);

		st->addWall(w);
		w->eid = eid;
	}

	// Update the transform
	btTransform xform = btTransform(
		btQuaternion(qx, qy, qz, qw),
		btVector3(bx, by, bz)
	);
	w->setTransform(xform);

	return 34;
}

unsigned int NetClient::handleObjectState(Uint8 *data, unsigned int size)
{
	DEBUGSTR("net", "handleObjectState()");

	Uint16 eid;
	CRC32 type;
	float qx, qy, qz, qw, bx, by, bz;

	unpack(data, "hl ffff fff",
		&eid, &type,
		&qx, &qy, &qz, &qw,
		&bx, &by, &bz
	);

	Entity* e = st->getEntity(eid);
	Object* o = (Object*) e;

	// If don't exist, create
	if (o == NULL) {
		ObjectType *ot = GEng()->mm->getObjectType(type);
		if (ot == NULL) {
			this->error("Invalid object type " + type);
			return 34;
		}

		o = new Object(ot, st, bx, by, bz);

		st->addObject(o);
		o->eid = eid;
	}

	// Update the transform
	btTransform xform = btTransform(
		btQuaternion(qx, qy, qz, qw),
		btVector3(bx, by, bz)
	);
	o->setTransform(xform);

	return 34;
}

unsigned int NetClient::handleVehicleState(Uint8 *data, unsigned int size)
{
	DEBUGSTR("net", "handleVehicleState()");

	Uint16 eid;
	CRC32 type;
	float qx, qy, qz, qw, bx, by, bz;

	unpack(data, "hl ffff fff",
		&eid, &type,
		&qx, &qy, &qz, &qw,
		&bx, &by, &bz
	);

	Entity* e = st->getEntity(eid);
	Vehicle* v = (Vehicle*) e;

	btTransform trans = btTransform(btQuaternion(qx, qy, qz, qw), btVector3(bx, by, bz));

	// If don't exist, create
	if (v == NULL) {
		VehicleType *vt = GEng()->mm->getVehicleType(type);
		if (vt == NULL) {
			this->error("Invalid vehicle type " + type);
			return 34;
		}

		if (vt->helicopter) {
			v = new Helicopter(vt, st, trans);
		} else {
			v = new Vehicle(vt, st, trans);
		}

		st->addVehicle(v);
		v->eid = eid;
	}

	// Update the vehicle
	v->setTransform(trans);

	return 34;
}

unsigned int NetClient::handleAmmoroundState(Uint8 *data, unsigned int size)
{
	DEBUGSTR("net", "handleAmmoroundState()");

	Uint16 eid, unit_eid;
	CRC32 type;
	float qx, qy, qz, qw, bx, by, bz, mass;

	unpack(data, "hhl ffff fff f",
		&eid, &unit_eid, &type,
		&qx, &qy, &qz, &qw,
		&bx, &by, &bz,
		&mass
	);

	// Find existing entity, unit, and weapon
	AmmoRound* ar = (AmmoRound*) st->getEntity(eid);
	Unit* u = (Unit*) st->getEntity(unit_eid);
	WeaponType* wt = GEng()->mm->getWeaponType(type);

	// Check valid
	if (u == NULL || wt == NULL || wt->model == NULL) {
		this->error("Invalid ammoround type " + type);
		return 40;
	}
	
	// Construct transform obj
	btTransform xform = btTransform(
		btQuaternion(qx, qy, qz, qw),
		btVector3(bx, by, bz)
	);

	// Create or update
	if (ar == NULL) {
		ar = new AmmoRound(st, xform, wt, wt->model, u, mass);
		st->addAmmoRound(ar);
		ar->eid = eid;
	} else {
		ar->setTransform(xform);
	}

	return 40;
}

unsigned int NetClient::handlePickupState(Uint8 *data, unsigned int size)
{
	DEBUGSTR("net", "handlePickupState()");

	Uint16 eid;
	CRC32 type;
	float qx, qy, qz, qw, bx, by, bz;

	unpack(data, "hl ffff fff",
		&eid, &type,
		&qx, &qy, &qz, &qw,
		&bx, &by, &bz
	);

	Entity* e = st->getEntity(eid);
	Pickup* p = (Pickup*) e;

	// If don't exist, create
	if (p == NULL) {
		PickupType *pt = GEng()->mm->getPickupType(type);
		if (pt == NULL) {
			this->error("Invalid pickup type " + type);
			return 34;
		}

		p = new Pickup(pt, st, bx, bz, by);

		st->addPickup(p);
		p->eid = eid;
	}

	// Update the transform
	btTransform xform = btTransform(
		btQuaternion(qx, qy, qz, qw),
		btVector3(bx, by, bz)
	);
	p->setTransform(xform);

	return 34;
}

unsigned int NetClient::handleEntityRem(Uint8 *data, unsigned int size)
{
	DEBUGSTR("net", "handleEntityRem()");

	EID eid;

	unpack(data, "h", &eid);

	DEBUG("net", "  eid: %i", eid);

	// Find and remove
	Entity *e = st->getEntity(eid);
	if (e != NULL) {
		e->del = true;
	}

	// Is it us? unset the player ref
	if (e == this->st->local_players[0]->p) {
		this->st->local_players[0]->p = NULL;
		DEBUGSTR("net", "  It's us :(");
	}

	return 2;
}
