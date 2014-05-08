----
----  Lua script for the gametype "Zombies".
----


--
-- Spawn a player
--
bind_playerjoin(function(slot)
	add_player(get_selected_unittype(), factions.team1, slot);
end);


--
-- Game start
--
bind_gamestart(function()
	add_interval(5000, doPick);
end);


function doPick()
	mouse_pick(function(coord)
		add_vehicle("tank", coord)
	end)
end

