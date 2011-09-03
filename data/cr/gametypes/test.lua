----
----  Test scripting features & gametype concepts
----

bind_gamestart(function()
	
	add_interval(1000, function()
		add_player("robot", factions.team1, 1);
	end);
	
end);



