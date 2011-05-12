--
-- Lua script for the gametype "Zombies".
--

bind_gamestart(function()
	
	-- create some zombies
	for variable = 1, 10 do
		add_npc("zomb", factions.team2);
	end
	
end);



