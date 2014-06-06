----
----  Lua script for the gametype "Zombies".
----


--
-- Spwans zombies
--
spawn_ais = function()
	for i = 0, 25, 1 do
		game.addNpc("robot", "human", factions.individual);
	end;
end;


--
-- Game start
--
bind_gamestart(function()
	-- Show msg
	lab = add_label(0, 350, "Three")
	lab.align = 2
	lab.r = 0.7
	lab.g = 0.1
	lab.b = 0.1
	
	-- Animation
	add_timer(1000, function()
		lab.data = "Two"
		lab.r = 0.8
	end);
	add_timer(2000, function()
		lab.data = "One"
		lab.r = 0.9
	end);
	add_timer(3000, function()
		lab.data = "Fight!"
		lab.r = 1.0
		spawn_ais()			-- bring in the AIs
	end);
	add_timer(4000, function()
		lab.visible = false
	end);
end);
