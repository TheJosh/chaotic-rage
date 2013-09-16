// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL_net.h>
#include <vector>
#include "net.h"
#include "net_browse.h"
#include "../util/ui_update.h"

using namespace std;


/**
* Returns a list of local server names
* Don't forget to delete() the result!
**/
vector<string>* getLocalServers(UIUpdate * ui)
{
	IPaddress ipaddress;
	UDPsocket sock;
	UDPpacket *pkt;
	vector<string>* out;
	
	if (SDLNet_Init() < 0) {
		reportFatalError("SDL_net init failed");
	}

	sock = SDLNet_UDP_Open(0);
	pkt = SDLNet_AllocPacket(512);
	out = new vector<string>();

	// Create "browse" packet
	pkt->len = 4;
	SDLNet_Write16(0x0000, pkt->data);
	SDLNet_Write16(0xFFFF, pkt->data + 2);

	// UDP broadcast
	SDLNet_ResolveHost(&ipaddress, "255.255.255.255", 17778);
	pkt->address = ipaddress;
	SDLNet_UDP_Send(sock, -1, pkt);

	// Try localhost too
	SDLNet_ResolveHost(&ipaddress, "127.0.0.1", 17778);
	pkt->address = ipaddress;
	SDLNet_UDP_Send(sock, -1, pkt);

	// Wait up to five seconds for any responses
	unsigned int now = SDL_GetTicks();
	do {
		pkt->len = 0;
		while (SDLNet_UDP_Recv(sock, pkt)) {
			const char* host = SDLNet_ResolveIP(&pkt->address);
			if (host != NULL) {
				out->push_back(host);
			}
		}

	} while (SDL_GetTicks() - now < 5000);

	SDLNet_FreePacket(pkt);
	SDLNet_UDP_Close(sock);
	SDLNet_Quit();

	return out;
}

