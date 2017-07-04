// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include <string>
#include "day_night_animate.h"
#include "../../game_state.h"


std::string CmdDayNightAnimate::doWork(GameState* st)
{
    if (target_val < 0.0f) target_val = 0.0f;
    if (target_val > 1.0f) target_val = 1.0f;

    if (total_time < 0.1f) total_time = 0.1f;
    if (total_time > 100.0f) total_time = 100.0f;

    float diff = abs(st->time_of_day - target_val);
    st->tod_target_val = target_val;
    st->tod_target_psec = diff / total_time;

    return "OK";
}
