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
	add_timer(5000, doPick);
end);


function doPick()
	mouse_pick(function(coord)
		local t = random_arg("crate_wood", "crate_steel", "crate_sealed")
		add_object(t, coord)
	end)
end

