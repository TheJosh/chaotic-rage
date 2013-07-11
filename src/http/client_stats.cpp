// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <iostream>

#include <GL/glew.h>
#include <GL/gl.h>
#if defined(__WIN32__)
	#include <GL/glext.h>
#endif

#include "../rage.h"
#include "happyhttp.h"
#include "url.h"


using namespace std;


/**
* Send a small amount of info to our servers, so we have some idea
* what kinds of hardware we are running.
* Kind of like the steam survey
**/
void sendClientStats()
{
	char * version = url_encode((char*)VERSION);
	char * gl_vendor = url_encode((char*)glGetString(GL_VENDOR));
	char * gl_renderer = url_encode((char*)glGetString(GL_RENDERER));
	char * gl_version = url_encode((char*)glGetString(GL_VERSION));
	
	char * params = (char*)malloc(50 + strlen(version) + strlen(gl_vendor) + strlen(gl_renderer) + strlen(gl_version));
	sprintf(params,
		"version=%s&glvendor=%s&glrenderer=%s&glversion=%s",
		version,
		gl_vendor,
		gl_renderer,
		gl_version
	);
	int l = strlen(params);
	
	
	try {
		happyhttp::Connection conn("api.chaoticrage.com", 80);
		conn.setcallbacks(0, 0, 0, 0);
	
		conn.putrequest("POST", "/client_stats");
		conn.putheader("Connection", "close");
		conn.putheader("Content-Length", l);
		conn.putheader("Content-type", "application/x-www-form-urlencoded");
		conn.putheader("Accept", "text/plain");
		conn.endheaders();
		conn.send((const unsigned char*)params, l);
	
		while (conn.outstanding()) {
			conn.pump();
		}
	} catch (happyhttp::Wobbly w) {}
	
	free(version);
	free(gl_vendor);
	free(gl_renderer);
	free(gl_version);
	free(params);
}
