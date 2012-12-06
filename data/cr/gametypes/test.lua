----
----  Test scripting features & gametype concepts
----


debug_physics(true);


bind_playerjoin(function(slot)
	add_timer(2000, function()
		add_player("robot", factions.team1, slot);
	end);

	add_timer(5000, function() 
		add_npc("zomb", "zombie", factions.team2);
	end);
end);


bind_playerdied(function(slot)
	add_timer(2000, function()
		add_player("robot", factions.team1, slot);
	end);
end);



