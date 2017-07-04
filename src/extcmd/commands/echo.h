// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../cmd.h"


class CmdEcho : public Cmd
{
    public:
        CmdEcho(std::string url):
            Cmd(), url(url)
            {};

        virtual std::string doWork() {
            return "URL: " + url;
        };

    private:
        std::string url;
};
