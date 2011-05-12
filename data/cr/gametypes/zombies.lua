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
	
end);



