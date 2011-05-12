--
-- Lua script for the gametype "Zombies".
--

bind_gamestart(function()
	num_zombies = 0;
	num_wanted = 10;

	-- add player
	add_timer(2000, function()
		add_player("maniac", factions.team1);
	end);
	
	-- add zombies
	timer_id = add_interval(1000, function()
		add_npc("zomb", factions.team2);
		num_zombies = num_zombies + 1;
		if num_zombies >= num_wanted then remove_timer(timer_id) end;
	end);
	
end);



