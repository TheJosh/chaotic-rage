----
----  Pounds the game engine by doing lots of random stuff
----
debug_physics(true)
debug_framerate(true)


-- When a player joins, spawn them in
bind_playerjoin(function(slot)
	add_timer(1000, function()
		show_alert_message("Spawning player " .. slot)
		add_player("robot", factions.team1, slot)
	end)
end)

-- If the player dies, spawn them in again
bind_playerdied(function(slot)
	add_timer(1000, function()
		show_alert_message("Spawning player " .. slot)
		add_player("robot", factions.team1, slot)
	end)
end)

-- Random viewmode every 3.5 secs
bind_gamestart(function()
	add_interval(3500, function()
		mode = random(0,2)
		show_alert_message("Setting view mode to " .. mode)
		set_viewmode(mode)
	end)
end)

-- The game only lasts for 60 seconds
-- then we move on to the next stage of the campaign
bind_gamestart(function()
	add_interval(1000 * 60, function()
		game_over(1)
	end)
end)
