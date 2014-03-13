----
----  Lua script for the gametype "Zombies".
----


--
-- Spwans zombies
--
spawn_ais = function()
	for i = 0, 25, 1 do
		add_npc("robot", "human", factions.individual);
	end;
end;


--
-- Game start
--
bind_gamestart(function()
	add_timer(1500, spawn_ais)
	
	-- Show msg
	lab = add_label(0, 350, "3");
	lab.align = 2;
	lab.r = 0.9; lab.g = 0.1; lab.b = 0.1
	
	-- Animation
	add_timer(500, function()
		lab.data = "2"
	end);
	add_timer(1000, function()
		lab.data = "1"
	end);
	add_timer(1500, function()
		lab.data = "Fight"
	end);
	add_timer(2000, function()
		lab.a = 0
	end);
end);
