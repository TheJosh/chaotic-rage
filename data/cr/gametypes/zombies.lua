--
-- Lua script for the gametype "Zombies".
--

num_zombies = 0;
num_wanted = 10;
num_dead = 0;
timer = 0;

spawn_func = function()
	add_npc("zomb", factions.team2);
	num_zombies = num_zombies + 1;
	if num_zombies >= num_wanted then remove_timer(timer) end;
end;

bind_gamestart(function()
	
	-- add player
	add_timer(2000, function()
		add_player("maniac", factions.team1);
	end);
	
	-- add zombies
	timer = add_interval(1000, spawn_func);
	
end);

bind_unitdied(function()
	-- TODO: This should only do stuff for death of zombies, not death of players
	num_dead = num_dead + 1;
	
	-- Is it time for a new round?
	if num_dead == num_wanted then 
		debug("New round!");
		
		-- give them a moment...
		add_timer(2000, function()
			num_wanted = num_wanted + 10;
			timer = add_interval(1000, spawn_func);
		end);
		
	end;
	
end);



