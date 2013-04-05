----
----  Lua script for the gametype "Boredem".
----


debug_physics(true);

bind_playerjoin(function(slot)
	show_alert_message("Welcome!")
	
	add_timer(1000, function()
		show_alert_message("You're in!")
		add_player("robot", factions.team1, slot)
	end)
end)


bind_gamestart(function()
	-- Randomly set the view mode every 5 secs
	add_interval(5000, function()
		mode = random(0,2)
		show_alert_message("Setting view mode to " .. mode)
		set_viewmode(mode)
	end)
end)
