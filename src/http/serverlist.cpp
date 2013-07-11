// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include "../rage.h"
#include "happyhttp.h"
#include "url.h"
#include "../util/ui_update.h"

using namespace std;


int len = 0;
string resp;


void OnBegin( const happyhttp::Response* r, void* userdata )
{
	len = 0;
	resp = "";
}

void OnData( const happyhttp::Response* r, void* userdata, const unsigned char* data, int n )
{
	resp.append((const char *) data, n);
	len += n;
}


/**
* Get a list of registered game servers
**/
vector<string> * getServerList(UIUpdate * ui)
{
	if (ui) ui->updateUI();
	
	// Grab the server list
	try {
		happyhttp::Connection conn("servers.chaoticrage.com", 80);
		conn.setcallbacks(OnBegin, OnData, 0, 0);
		
		conn.request("GET", "/list");
		
		while (conn.outstanding()) {
			if (ui) ui->updateUI();
			conn.pump();
		}
		
	} catch (happyhttp::Wobbly w) {
		return NULL;
	}
	
	if (ui) ui->updateUI();
	
	// Split on newline
	vector<string> * elems = new vector<string>();
	stringstream ss(resp);
	string item;
	while(std::getline(ss, item, '\n')) {
		elems->push_back(item);
	}
	
	return elems;
}
