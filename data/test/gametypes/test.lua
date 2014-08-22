----
----  Test scripting features & gametype concepts
----

bind_gamestart(function(slot)
	set_viewmode(2)
	debug_physics(true)
	game.addNpc("zomb", "dumbo", factions.team2)
end);

bind_playerjoin(function(slot)
	game.addPlayer(get_selected_unittype(), factions.team1, slot)
end);

