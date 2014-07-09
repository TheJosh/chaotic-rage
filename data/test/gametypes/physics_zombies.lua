----
----  Physics test - zombies
----
debug_framerate(true)


bind_gamestart(function()
	-- Make some zombies!
	for i = 0, 50, 1 do
		game.addNpc("zomb", "zombie", factions.team2);
	end
	
	-- The game doesn't last long
	add_interval(1000 * 20, function()
		game_over(1)
	end)
end)
