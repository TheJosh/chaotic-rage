----
----  Lua script for the gametype "Boredem".
----

bind_gamestart(function()
	
	add_timer(2000, function()
		add_player("maniac", factions.team1, 1);
	end);
	
	add_timer(4000, function()
		add_player("maniac", factions.team2, 2);
	end);
	
end);



