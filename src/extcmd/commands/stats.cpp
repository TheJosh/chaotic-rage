// This file is part of Chaotic Rage (c) 2010 Josh Heidenreich
//
// kate: tab-width 4; indent-width 4; space-indent off; word-wrap off;


#include <sstream>
#include <string>
#include "stats.h"
#include "../../game_state.h"
#include "../../map/map.h"
#include "../../entity/unit/player.h"


std::string CmdStats::doWork(GameState* st)
{
    std::ostringstream out;

    out << "<meta http-equiv=\"refresh\" content=\"1\">";

    out << "<table>";
    out << "<tr><th>Map:</th><td>" << st->map->getName() << "</td></tr>";
    out << "<tr><th>Time:</th><td>" << st->game_time << "</td></tr>";
    out << "</table>";

    out << "<table>";
    out << "<tr>";
    out << "<th>Slot</th><th>X</th><th>Y</th><th>Z</th>";
    out << "</tr>";
    for (unsigned int i = 0; i < st->num_local; ++i) {
        PlayerState* ps = st->local_players[i];
        out << "<tr>";
        out << "<td>" << ps->slot << "</td>";

        if (ps->p != NULL) {
            btTransform xform = ps->p->getTransform();
            out << "<td>" << xform.getOrigin().x() << "</td>";
            out << "<td>" << xform.getOrigin().y() << "</td>";
            out << "<td>" << xform.getOrigin().z() << "</td>";
        } else {
            out << "<td>-</td>";
            out << "<td>-</td>";
            out << "<td>-</td>";
        }

        out << "</tr>";
    }
    out << "</table>";

    return out.str();
}
