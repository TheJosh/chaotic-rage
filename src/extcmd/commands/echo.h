// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../cmd.h"


class CmdEcho : public Cmd
{
    public:
        CmdEcho(std::string msg):
            Cmd(), msg(msg)
            {};

        virtual std::string doWork(GameState* st) {
            return msg;
        };

    private:
        std::string msg;
};