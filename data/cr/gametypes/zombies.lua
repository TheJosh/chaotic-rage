--
-- Lua script for the gametype "Zombies".
--

bind_gamestart(function()
	
	-- add player
	add_player("maniac", factions.team1);
	
	-- create some zombies
	for variable = 1, 10 do
		add_npc("zomb", factions.team2);
	end
	
	-- timer to create zombies
	timer_id = add_interval(1000, function()
		
		debug("Zombie interval tick");
		
	end);
	debug("Created timer, id = ", timer_id);
	
	-- timer to create zombies
	timer_id = add_timer(2000, function()
		
		debug("Zombie timer tick");
		
	end);
	debug("Created timer, id = ", timer_id);
	
end);



