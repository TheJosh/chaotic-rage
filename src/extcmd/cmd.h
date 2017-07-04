// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <string>
#include <SDL2/SDL.h>

class GameState;


class Cmd
{
    public:
        Cmd() {
            lock = SDL_CreateMutex();
            complete = SDL_CreateCond();
        };

        virtual ~Cmd()
        {
            SDL_DestroyMutex(lock);
            SDL_DestroyCond(complete);
        }

        void exec(GameState* st) {
            SDL_LockMutex(lock);
            this->resp = this->doWork(st);
            SDL_CondBroadcast(complete);
            SDL_UnlockMutex(lock);
        };

        void cancel() {
            SDL_LockMutex(lock);
            this->resp = "";
            SDL_CondBroadcast(complete);
            SDL_UnlockMutex(lock);
        }

        virtual std::string doWork(GameState* st) = 0;

        std::string waitDone() {
            SDL_LockMutex(lock);
            SDL_CondWait(complete, lock);
            SDL_UnlockMutex(lock);
            return this->resp;
        };

    private:
        SDL_mutex* lock;
        SDL_cond* complete;
        std::string resp;
};
