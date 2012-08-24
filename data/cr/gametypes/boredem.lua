----
----  Lua script for the gametype "Boredem".
----


debug_physics(true);


bind_playerjoin(function(slot)
	show_alert_message("raise_playerjoin(" .. slot .. ")");
	debug("raise_playerjoin(" .. slot .. ")");
	
	add_player("robot", factions.team1, slot);
end);



