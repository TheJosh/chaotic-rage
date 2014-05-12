----
----  Test scripting features & gametype concepts
----


bind_playerjoin(function(slot)
	add_timer(2000, function()
		add_player("robot", factions.team1, slot);
		add_npc("zomb", "dumbo", factions.team2);
		
		add_vehicle("ute", vector3(10, 10, 10));
	end);
end);


bind_playerdied(function(slot)
	add_timer(2000, function()
		add_player("robot", factions.team1, slot);
	end);
end);



