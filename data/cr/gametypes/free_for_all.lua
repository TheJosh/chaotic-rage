----
----  Lua script for the gametype "Boredem".
----


bind_playerjoin(function(slot)
	show_alert_message("raise_playerjoin(" .. slot .. ")");
	
	add_player("robot", factions.individual, slot);
end);


bind_playerdied(function(slot)
	show_alert_message("bind_playerdied(" .. slot .. ")");

	add_timer(2000, function()
		add_player(get_selected_unittype(), factions.individual, slot);
	end);
end);
