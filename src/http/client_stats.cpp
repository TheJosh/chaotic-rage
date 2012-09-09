// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <iostream>
#include "../rage.h"
#include "happyhttp.h"


using namespace std;


/* Converts an integer value to its hex character*/
char to_hex(char code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}

/* Returns a url-encoded version of str */
/* IMPORTANT: be sure to free() the returned string after use */
char *url_encode(char *str) {
  char *pstr = str, *buf = (char*)malloc(strlen(str) * 3 + 1), *pbuf = buf;
  while (*pstr) {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
      *pbuf++ = *pstr;
    else if (*pstr == ' ') 
      *pbuf++ = '+';
    else 
      *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}


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
	
	char * params = (char*)malloc(50 + strlen(gl_vendor) + strlen(gl_renderer) + strlen(gl_version));
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
