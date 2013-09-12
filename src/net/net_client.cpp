// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <math.h>
#include <SDL_net.h>
#include "../rage.h"
#include "../gamestate.h"
#include "../entity/player.h"
#include "../entity/ammo_round.h"
#include "../mod/weapontype.h"
#include "../mod/mod_manager.h"
#include "../util/ui_update.h"
#include "net.h"
#include "net_client.h"
#include "net_gameinfo.h"

using namespace std;


NetClient::NetClient(GameState * st)
{
	this->st = st;
	st->client = this;
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
	
	if (this->gameinfo) {
		delete(this->gameinfo);
	}
}


/**
* Some time has passed, do stuff
**/
void NetClient::update()
{
	UDPpacket *pkt = SDLNet_AllocPacket(1000);
	
	
	// Check the server is still chatting to us
	if (this->ingame && (st->game_time - this->last_ack) > 2500) {
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
		if (debug_enabled("net_pkt")) {
			cout << setw (6) << setfill(' ') << st->game_time << " SEND ";
			dumpPacket(pkt->data, pkt->len);
		}
		
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
	} while (SDL_GetTicks() - now > 2000);

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
***  One method for each outgoing network message the client sends out
**/

void NetClient::addmsgInfoReq()
{
	NetMsg * msg = new NetMsg(INFO_REQ, 0);
	msg->seq = this->seq;
	messages.push_back(*msg);
}

void NetClient::addmsgJoinReq()
{
	NetMsg * msg = new NetMsg(JOIN_REQ, 0);
	msg->seq = this->seq;
	messages.push_back(*msg);
}

void NetClient::addmsgJoinAck()
{
	NetMsg * msg = new NetMsg(JOIN_ACK, 0);
	msg->seq = this->seq;
	messages.push_back(*msg);
}

void NetClient::addmsgDataCompl()
{
	NetMsg * msg = new NetMsg(JOIN_DONE, 0);
	msg->seq = this->seq;
	messages.push_back(*msg);
}

void NetClient::addmsgChat()
{
	NetMsg * msg = new NetMsg(CHAT_REQ, 0);
	msg->seq = this->seq;
	messages.push_back(*msg);
}


void NetClient::addmsgKeyMouseStatus(int x, int y, int delta, Uint8 k)
{
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
	
	this->ingame = false;
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
	
	unsigned int slot = 0;
	char map[128];
	
	unpack(data, "hs",
		&slot, &map
	);
	
	if (st->local_players[0]->slot == 0) {
		st->local_players[0]->slot = slot;
		cout << "       Our slot: " << slot << "\n";
	}
	
	this->gameinfo = new NetGameinfo();
	this->gameinfo->map = std::string(map);
	
	this->last_ack = st->game_time;

	this->addmsgJoinAck();
	
	return 4 + strlen(map);
}

unsigned int NetClient::handleJoinRej(Uint8 *data, unsigned int size)
{
	cout << "       handleJoinRej()\n";
	return 0;
}

unsigned int NetClient::handleDataCompl(Uint8 *data, unsigned int size)
{
	cout << "       handleDataCompl()\n";
	this->ingame = true;
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
	
	unsigned int slot = 0;
	unpack(data, "h",
		&slot
	);
	
	// Were we booted?
	if (st->local_players[0]->slot == slot) {
		st->gameOver();
	}
	
	return 2;
}


unsigned int NetClient::handleUnitState(Uint8 *data, unsigned int size)
{
	//cout << "       handleUnitState()\n";
	
	
	short eid = 0, slot = 0;
	float qx, qy, qz, qw, bx, by, bz;
	float health;
	
	unpack(data, "hh ffff fff f",
		&eid, &slot,
		&qx, &qy, &qz, &qw,
		&bx, &by, &bz,
		&health
	);
	
	// TODO: unit type, faction, drive, lift, current weapon, weapons
	
	Entity* e = st->getEntity(eid);
	Unit* u = (Unit*) e;

	// If don't exist, create
	if (u == NULL) {
		cout << "       CREATE:\n";
		cout << "       eid: " << eid << "   slot: " << slot << "   our slot: " << st->local_players[0]->slot << "\n";
		
		UnitType *ut = st->mm->getUnitType("robot");
		u = new Player(ut, st, bx, bz, by);
		u->slot = slot;
		
		// If the player is this client, save in the local_players obj
		if (st->local_players[0]->slot == u->slot) {
			st->local_players[0]->p = (Player*)u;
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
	
	return 36;
}

unsigned int NetClient::handleWallState(Uint8 *data, unsigned int size)
{
	cout << "       handleWallState()\n";


	short eid = 0;
	float qx, qy, qz, qw, bx, by, bz;
	
	unpack(data, "h ffff fff",
		&eid,
		&qx, &qy, &qz, &qw,
		&bx, &by, &bz
	);
	
	Entity* e = st->getEntity(eid);
	Wall* w = (Wall*) e;

	// If don't exist, create
	if (w == NULL) {
		WallType *wt = st->mm->getWallType("brick");
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
	
	return 30;
}

unsigned int NetClient::handleObjectState(Uint8 *data, unsigned int size)
{
	cout << "       handleObjectState()\n";

	short eid = 0;
	float qx, qy, qz, qw, bx, by, bz;
	
	unpack(data, "h ffff fff",
		&eid,
		&qx, &qy, &qz, &qw,
		&bx, &by, &bz
	);
	
	Entity* e = st->getEntity(eid);
	Object* o = (Object*) e;

	// If don't exist, create
	if (o == NULL) {
		ObjectType *ot = st->mm->getObjectType("machinegun_pickup");		// TODO: support this
		o = new Object(ot, st, bx, bz, by, 0);
		
		st->addObject(o);
		o->eid = eid;
	}
	
	// Update the transform
	btTransform xform = btTransform(
		btQuaternion(qx, qy, qz, qw),
		btVector3(bx, by, bz)
	);
	o->setTransform(xform);
	
	return 30;
}

unsigned int NetClient::handleVehicleState(Uint8 *data, unsigned int size)
{
	cout << "       handleVehicleState()\n";

	short eid = 0;
	float qx, qy, qz, qw, bx, by, bz, ef, bf, s;
	
	unpack(data, "h ffff fff fff",
		&eid,
		&qx, &qy, &qz, &qw,
		&bx, &by, &bz,
		&ef, &bf, &s
	);
	
	Entity* e = st->getEntity(eid);
	Vehicle* v = (Vehicle*) e;

	btTransform trans = btTransform(btQuaternion(qx, qy, qz, qw), btVector3(bx, by, bz));

	// If don't exist, create
	if (v == NULL) {
		VehicleType *vt = st->mm->getVehicleType("tank");		// TODO: support this

		v = new Vehicle(vt, st, trans);
		
		st->addVehicle(v);
		v->eid = eid;
	}
	
	// Update the vehicle
	v->setTransform(trans);
	v->engineForce = ef;
	v->brakeForce = bf;
	v->steering = s;

	return 42;
}

unsigned int NetClient::handleAmmoroundState(Uint8 *data, unsigned int size)
{
	cout << "       handleAmmoroundState()\n";

	short eid = 0, unit_eid = 0;
	float qx, qy, qz, qw, bx, by, bz, mass;
	
	unpack(data, "hh ffff fff f",
		&eid, &unit_eid,
		&qx, &qy, &qz, &qw,
		&bx, &by, &bz,
		&mass
	);
	
	// Find existing entity, unit, and weapon
	AmmoRound* ar = (AmmoRound*) st->getEntity(eid);
	Unit* u = (Unit*) st->getEntity(unit_eid);
	WeaponType* wt = st->mm->getWeaponType("remote_mine");		// TODO: support this
	
	// Check valid
	if (u == NULL) return 36;
	if (wt == NULL) return 36;
	if (wt->model == NULL) return 36;
	
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
	
	return 36;
}

unsigned int NetClient::handlePickupState(Uint8 *data, unsigned int size)
{
	cout << "       handlePickupState()\n";

	short eid = 0;
	float qx, qy, qz, qw, bx, by, bz;
	
	unpack(data, "h ffff fff",
		&eid,
		&qx, &qy, &qz, &qw,
		&bx, &by, &bz
	);
	
	Entity* e = st->getEntity(eid);
	Pickup* p = (Pickup*) e;

	// If don't exist, create
	if (p == NULL) {
		PickupType *pt = st->mm->getPickupType("ammo_current");		// TODO: support this
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
	
	return 30;
}

unsigned int NetClient::handleEntityRem(Uint8 *data, unsigned int size)
{
	cout << "       handleEntityRem()\n";
	
	EID eid;
	
	unpack(data, "h", &eid);
	
	// Find and remove
	Entity *e = st->getEntity(eid);
	if (e) {
		e->del = true;
	}
	
	// Is it us? unset the player ref
	if (e == this->st->local_players[0]->p) {
		this->st->local_players[0]->p = NULL;
	}
	
	return 2;
}



