// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <string>
#include <iostream>
#include <errno.h>
#include <glob.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef NOGUI
	#include <SDL.h>
	#include <SDL_syswm.h>
	#include <X11/Xlib.h>
	#include <X11/Xutil.h>
	#include <X11/Xos.h>
#endif

#include "../gamestate.h"

using namespace std;



/**
* Returns the path for a directory which we can put some user data.
* The returned path includes a trailing slash.
*
* Example return value:
*    /home/josh/.chaoticrage/
**/
string getUserDataDir()
{
	char * env;
	
	env = getenv ("HOME");
	if (! env) {
		reportFatalError("Environment variable $HOME is not set");
	}
	
	string home(env);
	home.append("/.chaoticrage/");
	
	int status = mkdir(home.c_str(), 0777);
	if (status == -1 && errno != EEXIST) {
		reportFatalError("Unable to create directory " + home);
	}
	
	return home;
}


/**
* Reports a fatal error, and then exits.
* The message should not include a trailing newline.
**/
void reportFatalError(string msg)
{
	string m = "FATAL ERROR: ";
	m.append(msg);
	displayMessageBox(m);
	exit(1);
}


/**
* Shows a message box (like a non-fatal error)
**/
void displayMessageBox(string msg)
{
#ifdef NOGUI
	cout << msg << "\n";
#else
	SDL_SysWMinfo wm;
	SDL_VERSION(&wm.version);
	if (! SDL_GetWMInfo(&wm)) {
		return;
	}
	
	cout << msg << "\n";
	
	const char *charmsg = msg.c_str();
	
	Display *d;
	Window w;
	XEvent e;
	int s;
	
	d = wm.info.x11.display;
	
	// Create window
	wm.info.x11.lock_func();
	s = DefaultScreen(d);
	w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, 50 + msg.length() * 10, 50, 0, BlackPixel(d, s), WhitePixel(d, s));
	XSetStandardProperties(d,w,"Fatal Error","Fatal Error",None,NULL,0,NULL);
	XSelectInput(d, w, ExposureMask | ButtonPressMask | KeyPressMask);
	XMapWindow(d, w);
	Atom wmDeleteMessage = XInternAtom(d, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(d, w, &wmDeleteMessage, 1);
	wm.info.x11.unlock_func();
	
	// Event loop
	bool running = 1;
	while (running) {
		SDL_Delay(10);
		
		// Fetch event and handle draw from within WM lock
		wm.info.x11.lock_func();
		XNextEvent(d, &e);
		if (e.type == Expose) {
			XDrawString(d, w, DefaultGC(d, s), 10, 20, charmsg, strlen(charmsg));
		}
		wm.info.x11.unlock_func();
		
		// Other events can be handled without lock
		switch (e.type) {
			case ButtonPress:
			case KeyPress:
				running = 0;
				break;
				
			case ClientMessage:
				if (e.xclient.data.l[0] == (long)wmDeleteMessage) {
					running = false;
				}
				break;
			
			default:
				break;
		}
	}
	
	wm.info.x11.lock_func();
	XDestroyWindow(d, w);
	wm.info.x11.unlock_func();
#endif
}


/**
* Returns an array of names of system mods
*
* Example return value:
*    <
*    cr
*    debug
*    >
*
* Please free the result when you are done.
**/
list<string> * getSystemModNames()
{
	// TODO: Actually code this!
	list<string> * out = new list<string>();
	out->push_back("cr");
	out->push_back("australia_day");
	out->push_back("test");
	return out;
}


/**
* Returns an array of full paths to user modules
* The returned paths include the ".crk" part.
*
* Example return value:
*    <
*    /home/josh/.chaoticrage/mods/hey.crk
*    /home/josh/.chaoticrage/mods/whoo.crk
*    >
*
* Please free the result when you are done.
**/
vector<string> * getUserModFilenames()
{
	vector<string> * ret = new vector<string>();
	
	string pat = getUserDataDir();
	pat.append("mods/*.crk");
	
	glob_t glob_result;
	glob(pat.c_str(), GLOB_TILDE, NULL, &glob_result);
	for (unsigned int i = 0; i < glob_result.gl_pathc; ++i) {
		ret->push_back(string(glob_result.gl_pathv[i]));
	}
	globfree(&glob_result);
	
	return ret;
}


