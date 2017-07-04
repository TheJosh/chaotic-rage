// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;

#pragma once
#include "../cmd.h"


class CmdDayNightAnimate : public Cmd
{
    public:
        CmdDayNightAnimate(float target_val, float total_time):
            Cmd(),
            target_val(target_val), total_time(total_time)
            {};

        virtual std::string doWork(GameState* st);

    protected:
        float target_val;
        float total_time;
};
