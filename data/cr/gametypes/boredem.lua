----
----  Lua script for the gametype "Boredem".
----


bind_playerjoin(function(slot)
	
	add_timer(1000, function()
		add_player("maniac", factions.team1, slot);
	end);
	
	add_timer(2000, function()
		add_npc("zomb", factions.team2);
		add_npc("zomb", factions.team2);
		add_npc("zomb", factions.team2);
	end);
	
end);



