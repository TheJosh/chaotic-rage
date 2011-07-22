----
----  Lua script for the gametype "Boredem".
----


bind_playerjoin(function(slot)
	
	add_timer(1000, function()
		add_player("maniac", factions.team1, slot);
	end);
	
end);



