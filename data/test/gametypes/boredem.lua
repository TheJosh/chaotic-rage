----
----  Lua script for the gametype "Boredem".
----


debug_physics(true);

bind_playerjoin(function(slot)
	show_alert_message("Player Join; " .. slot);
	
	add_timer(1000, function()
		show_alert_message("Timer done, add player; " .. slot);
		game.addPlayer("robot", factions.team1, slot);
	end);
end);



