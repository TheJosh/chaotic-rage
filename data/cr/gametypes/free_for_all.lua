----
----  Lua script for the gametype "Free For All".
----


bind_playerjoin(function(slot)
	show_alert_message("raise_playerjoin(" .. slot .. ")");
	
	game.addPlayer("robot", factions.individual, slot);
end);


bind_playerdied(function(slot)
	show_alert_message("bind_playerdied(" .. slot .. ")");

	add_timer(2000, function()
		game.addPlayer(get_selected_unittype(), factions.individual, slot);
	end);
end);
