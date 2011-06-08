----
----  Test scripting features & gametype concepts
----

bind_gamestart(function()
	
	add_interval(1000, function()
		add_player("maniac", factions.team1);
	end);
	
end);



