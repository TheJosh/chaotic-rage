----
----  Lua script for the gametype "Zombies".
----


--
-- Spawn a player
--
bind_playerjoin(function(slot)
	game.addPlayer(get_selected_unittype(), factions.team1, slot);
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

		local obj = game.addObject(t, coord.x, coord.z)

		add_interval(500, function()
			obj.visible = not obj.visible
		end)
	end)
end

