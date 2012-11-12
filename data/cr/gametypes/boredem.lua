----
----  Lua script for the gametype "Boredem".
----


bind_playerjoin(function(slot)
	show_alert_message("Player Join; " .. slot);
	
	add_timer(1000, function()
		show_alert_message("Timer done, add player; " .. slot);
		add_player("robot", factions.team1, slot);
	end);
end);



