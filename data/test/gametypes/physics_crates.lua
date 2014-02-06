----
----  Physics test - 100 crates
----
debug_physics(true)
debug_framerate(true)


bind_gamestart(function()
	-- Make some crates!
	for i = 0, 100, 1 do
		ammo_drop()
	end
	
	-- The game doesn't last long
	add_interval(1000 * 10, function()
		game_over(1)
	end)
end)
