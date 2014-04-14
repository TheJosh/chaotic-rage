----
----  Pounds the game engine by doing lots of random stuff
----
debug_physics(true)
debug_framerate(true)

g_slot = 0

-- When a player joins, spawn them in
bind_playerjoin(function(slot)
	add_timer(1000, function()
		show_alert_message("Spawning player " .. slot)
		add_player("robot", factions.team1, slot)
		g_slot = slot
	end)
end)

-- we don't let them survive too long!
bind_gamestart(function()
	add_interval(3000, function()
		if g_slot ~= 0 then 
			show_alert_message("Killing off player " .. g_slot)
			kill_player(g_slot)
		end
	end)
end)
	
-- When the player dies, spawn them in again
bind_playerdied(function(slot)
	add_timer(500, function()
		show_alert_message("Player " .. slot .. " died, spawning again")
		add_player("robot", factions.team1, slot)
		g_slot = slot
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

-- The game only lasts for 15 seconds
-- then we move on to the next stage of the campaign
bind_gamestart(function()
	add_interval(1000 * 15, function()
		game_over(1)
	end)
end)

-- bring in some npcs
bind_gamestart(function()
	add_interval(2000, function()
		t = random(1, 3)
		show_alert_message("Adding an AI, type " .. t)
		
		if (t == 1) then
			add_npc("robot", "zombie", factions.team2);
			
		elseif (t == 2) then
			t = random_arg("zomb", "zomb_fast", "zomb_health", "zomb_strong")
			add_npc(t, "zombie", factions.team1);
			
		elseif (t == 3) then
			t = random_arg("zomb", "zomb_fast", "robot")
			add_npc(t, "human", factions.team2);
			
		end
	end)
end)

-- randomly toggle physics debugging every 10 secs
bind_gamestart(function()
	add_interval(10000, function()
		t = random(1,2)
		if t == 1 then
			show_alert_message("Turning on physics debugging")
			debug_physics(true)
		else
			show_alert_message("Turning off physics debugging")
			debug_physics(false)
		end
	end)
end)
