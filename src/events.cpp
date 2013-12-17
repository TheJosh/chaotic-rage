// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#include <iostream>
#include <SDL.h>
#include <math.h>
#include "rage.h"
#include "game.h"
#include "gamestate.h"
#include "entity/player.h"
#include "render_opengl/hud.h"
#include "render_opengl/render_opengl.h"
#include "gui/dialog.h"

using namespace std;


/**
* Handles local events (keyboard, mouse)
**/
void handleEvents(GameState *st)
{
	SDL_Event event;
	
	static int screenshot_num = 0;
	
	
	
	while (SDL_PollEvent(&event)) {
		// General keys
		if (event.type == SDL_QUIT) {
			st->gameOver();
			
		} else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_ESCAPE:
				case SDLK_AC_BACK:
				case SDLK_MENU:
					if (! st->hasDialog("quit")) {
						st->addDialog(new DialogQuit(st));
					}
					break;

				case SDLK_PRINTSCREEN:
					{
						screenshot_num++;
						char buf[50];
						sprintf(buf, "%i", screenshot_num);
						
						string filename = getUserDataDir();
						filename.append("screenshot");
						filename.append(buf);
						filename.append(".bmp");
						((Render3D*) st->render)->saveScreenshot(filename);
						
						filename = "screenshot";
						filename.append(buf);
						filename.append(".bmp");
						st->addHUDMessage(ALL_SLOTS, "Saved ", filename);
					}
					break;
					
				case SDLK_F5:
					st->addHUDMessage(ALL_SLOTS, "Shaders reload: ", ((RenderOpenGL*)st->render)->reloadShaders() ? "success" : "failure");
					break;
					
				case SDLK_F6:
					st->setMouseGrab(! st->getMouseGrab());
					st->addHUDMessage(ALL_SLOTS, "Reset-mouse ", st->getMouseGrab() ? "on" : "off");
					break;
					
				case SDLK_F7:
					st->render->setPhysicsDebug(! st->render->getPhysicsDebug());
					st->addHUDMessage(ALL_SLOTS, "Physics debug ", st->render->getPhysicsDebug() ? "on" : "off");
					break;
					
				case SDLK_F8:
					st->render->setSpeedDebug(! st->render->getSpeedDebug());
					st->addHUDMessage(ALL_SLOTS, "Speed debug ", st->render->getSpeedDebug() ? "on" : "off");
					break;
					
				default: break;
			}
		}


		if (st->hasDialogs()) {
			st->guiinput->pushInput(event);
			continue;
		}


		// TODO: More dynamic
		
		// One player, player one
		if (st->num_local == 1 && st->local_players[0]->p != NULL) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_W: st->local_players[0]->p->keyPress(Player::KEY_UP); break;
					case SDL_SCANCODE_A: st->local_players[0]->p->keyPress(Player::KEY_LEFT); break;
					case SDL_SCANCODE_S: st->local_players[0]->p->keyPress(Player::KEY_DOWN); break;
					case SDL_SCANCODE_D: st->local_players[0]->p->keyPress(Player::KEY_RIGHT); break;
					case SDL_SCANCODE_E: st->local_players[0]->p->keyPress(Player::KEY_USE); break;
					case SDL_SCANCODE_Q: st->local_players[0]->p->keyPress(Player::KEY_LIFT); break;
					case SDL_SCANCODE_T: st->local_players[0]->p->keyPress(Player::KEY_SPECIAL); break;
					default: break;
				}
			
			} else if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_W: st->local_players[0]->p->keyRelease(Player::KEY_UP); break;
					case SDL_SCANCODE_A: st->local_players[0]->p->keyRelease(Player::KEY_LEFT); break;
					case SDL_SCANCODE_S: st->local_players[0]->p->keyRelease(Player::KEY_DOWN); break;
					case SDL_SCANCODE_D: st->local_players[0]->p->keyRelease(Player::KEY_RIGHT); break;
					case SDL_SCANCODE_E: st->local_players[0]->p->keyRelease(Player::KEY_USE); break;
					case SDL_SCANCODE_Q: st->local_players[0]->p->keyRelease(Player::KEY_LIFT); break;
					case SDL_SCANCODE_T: st->local_players[0]->p->keyRelease(Player::KEY_SPECIAL); break;
					default: break;
				}
			
			} else if (event.type == SDL_FINGERMOTION) {
				char buf[100];
				sprintf(buf, "TOUCH  %lli  at %f %f   diff %f %f   pressure %f", event.tfinger.fingerId, event.tfinger.x, event.tfinger.y, event.tfinger.dx, event.tfinger.dy, event.tfinger.pressure);
				displayMessageBox(std::string(buf));
				
			} else if (event.type == SDL_MOUSEMOTION) {
				game_x[0] += event.motion.xrel;
				net_x[0] += event.motion.xrel;
				game_y[0] += event.motion.yrel;
				net_y[0] += event.motion.yrel;
			
			} else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
				st->local_players[0]->hud->eventClick();
				st->local_players[0]->p->keyPress(Player::KEY_FIRE);
			
			} else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
				st->local_players[0]->p->keyRelease(Player::KEY_FIRE);

			} else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
				st->local_players[0]->p->keyPress(Player::KEY_MELEE);
			
			} else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT) {
				st->local_players[0]->p->keyRelease(Player::KEY_MELEE);

			} else if (event.type == SDL_MOUSEWHEEL && event.wheel.y > 0) {
				st->local_players[0]->hud->eventUp();
			
			} else if (event.type == SDL_MOUSEWHEEL && event.wheel.y < 0) {
				st->local_players[0]->hud->eventDown();

			}

		} // end one player, one


		// Two players, player one
		if (st->num_local == 2 && st->local_players[0]->p != NULL) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_W: st->local_players[0]->p->keyPress(Player::KEY_UP); break;
					case SDL_SCANCODE_A: st->local_players[0]->p->keyPress(Player::KEY_LEFT); break;
					case SDL_SCANCODE_S: st->local_players[0]->p->keyPress(Player::KEY_DOWN); break;
					case SDL_SCANCODE_D: st->local_players[0]->p->keyPress(Player::KEY_RIGHT); break;
					case SDL_SCANCODE_E: st->local_players[0]->p->keyPress(Player::KEY_USE); break;
					case SDL_SCANCODE_Q: st->local_players[0]->p->keyPress(Player::KEY_LIFT); break;
					case SDL_SCANCODE_T: st->local_players[0]->p->keyPress(Player::KEY_SPECIAL); break;
					case SDL_SCANCODE_K: mk_down_x[0] = -10; break;
					case SDL_SCANCODE_P: mk_down_x[0] = 10; break;
					case SDL_SCANCODE_9: mk_down_y[0] = -10; break;
					case SDL_SCANCODE_L: mk_down_y[0] = 10; break;
					case SDL_SCANCODE_O: st->local_players[0]->hud->eventClick(); st->local_players[0]->p->keyPress(Player::KEY_FIRE); break;
					case SDL_SCANCODE_I: st->local_players[0]->p->keyPress(Player::KEY_MELEE); break;
					case SDL_SCANCODE_J: st->local_players[0]->hud->eventUp(); break;
					case SDL_SCANCODE_M: st->local_players[0]->hud->eventDown(); break;
					default: break;
				}
			
			} else if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_W: st->local_players[0]->p->keyRelease(Player::KEY_UP); break;
					case SDL_SCANCODE_A: st->local_players[0]->p->keyRelease(Player::KEY_LEFT); break;
					case SDL_SCANCODE_S: st->local_players[0]->p->keyRelease(Player::KEY_DOWN); break;
					case SDL_SCANCODE_D: st->local_players[0]->p->keyRelease(Player::KEY_RIGHT); break;
					case SDL_SCANCODE_E: st->local_players[0]->p->keyRelease(Player::KEY_USE); break;
					case SDL_SCANCODE_Q: st->local_players[0]->p->keyRelease(Player::KEY_LIFT); break;
					case SDL_SCANCODE_T: st->local_players[0]->p->keyRelease(Player::KEY_SPECIAL); break;
					case SDL_SCANCODE_K: mk_down_x[0] = 0; break;
					case SDL_SCANCODE_P: mk_down_x[0] = 0; break;
					case SDL_SCANCODE_9: mk_down_y[0] = 0; break;
					case SDL_SCANCODE_L: mk_down_y[0] = 0; break;
					case SDL_SCANCODE_O: st->local_players[0]->p->keyRelease(Player::KEY_FIRE); break;
					case SDL_SCANCODE_I: st->local_players[0]->p->keyRelease(Player::KEY_MELEE); break;
					default: break;
				}
			}

		} // Two players, player one


		// Two players, player two
		// Keypad: Move = 8, 4, 5, 6; Action = 9; Lift = 7
		if (st->num_local == 2 && st->local_players[1]->p != NULL) {
			if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_KP_8: st->local_players[1]->p->keyPress(Player::KEY_UP); break;
					case SDL_SCANCODE_KP_4: st->local_players[1]->p->keyPress(Player::KEY_LEFT); break;
					case SDL_SCANCODE_KP_5: st->local_players[1]->p->keyPress(Player::KEY_DOWN); break;
					case SDL_SCANCODE_KP_6: st->local_players[1]->p->keyPress(Player::KEY_RIGHT); break;
					case SDL_SCANCODE_KP_9: st->local_players[1]->p->keyPress(Player::KEY_USE); break;
					case SDL_SCANCODE_KP_7: st->local_players[1]->p->keyPress(Player::KEY_LIFT); break;
					case SDL_SCANCODE_KP_1: st->local_players[1]->p->keyPress(Player::KEY_SPECIAL); break;
					default: break;
				}
			
			} else if (event.type == SDL_KEYUP) {
				switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_KP_8: st->local_players[1]->p->keyRelease(Player::KEY_UP); break;
					case SDL_SCANCODE_KP_4: st->local_players[1]->p->keyRelease(Player::KEY_LEFT); break;
					case SDL_SCANCODE_KP_5: st->local_players[1]->p->keyRelease(Player::KEY_DOWN); break;
					case SDL_SCANCODE_KP_6: st->local_players[1]->p->keyRelease(Player::KEY_RIGHT); break;
					case SDL_SCANCODE_KP_9: st->local_players[1]->p->keyRelease(Player::KEY_USE); break;
					case SDL_SCANCODE_KP_7: st->local_players[1]->p->keyRelease(Player::KEY_LIFT); break;
					case SDL_SCANCODE_KP_1: st->local_players[1]->p->keyRelease(Player::KEY_SPECIAL); break;
					default: break;
				}
			
			} else if (event.type == SDL_MOUSEMOTION) {
				game_x[1] += event.motion.xrel;
				net_x[1] += event.motion.xrel;
				game_y[1] += event.motion.yrel;
				net_y[1] += event.motion.yrel;
			
			} else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
				st->local_players[1]->hud->eventClick();
				st->local_players[1]->p->keyPress(Player::KEY_FIRE);
			
			} else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
				st->local_players[1]->p->keyRelease(Player::KEY_FIRE);

			} else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT) {
				st->local_players[1]->p->keyPress(Player::KEY_MELEE);
			
			} else if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_RIGHT) {
				st->local_players[1]->p->keyRelease(Player::KEY_MELEE);

			} else if (event.type == SDL_MOUSEWHEEL && event.wheel.y > 0) {
				st->local_players[1]->hud->eventUp();
			
			} else if (event.type == SDL_MOUSEWHEEL && event.wheel.y < 0) {
				st->local_players[1]->hud->eventDown();

			}

		} // Two players, player two


	} // end while


	// Process simulated mouse movements
	if (st->num_local == 2 && st->local_players[0]->p != NULL) {
		game_x[0] += mk_down_x[0];
		net_x[0] += mk_down_x[0];
		game_y[0] += mk_down_y[0];
		net_y[0] += mk_down_y[0];
	}
}


