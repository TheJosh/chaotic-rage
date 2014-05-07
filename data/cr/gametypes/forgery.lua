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
	mouse_pick(function(coord)
		prompt_text("Picked at " .. coord.x .. " " .. coord.y .. " " .. coord.z, function() end);
	end);
end);
