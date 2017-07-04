// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include <string>

class Cmd
{
    public:
        Cmd(std::string x): x(x) {
            printf("Cmd::Cmd '%s'\n", x.c_str());
        };

        void exec() {
            printf("Cmd::exec '%s'\n", x.c_str());
        }

    private:
        std::string x;
};
