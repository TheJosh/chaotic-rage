// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include <sstream>
#include <string>
#include "stats.h"
#include "../../game_state.h"
#include "../../map/map.h"


std::string CmdStats::doWork(GameState* st)
{
    std::ostringstream out;

    out << "<meta http-equiv=\"refresh\" content=\"1\">";

    out << "<table>";
    out << "<tr><th>Map:</th><td>" << st->map->getName() << "</td></tr>";
    out << "<tr><th>Time:</th><td>" << st->game_time << "</td></tr>";
    out << "</table>";

    return out.str();
}
