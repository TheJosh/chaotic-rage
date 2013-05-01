----
----  Lua script for the gametype "Boredem".
----


debug_physics(true);

bind_playerjoin(function(slot)
	show_alert_message("Welcome!")
	set_viewmode(2)
	
	add_timer(1000, function()
		add_player("robot", factions.team1, slot)
		add_npc("zomb", "dumbo", factions.team2);
	end)
end)
