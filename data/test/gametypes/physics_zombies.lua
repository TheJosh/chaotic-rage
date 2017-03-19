----
----  Physics test - zombies
----
debug_framerate(true)


bind_gamestart(function()
	weather.disableRandom();
	daynight.disableCycle();
	
	-- Make some zombies!
	for i = 0, 50, 1 do
		game.addNpc("zomb", "zombie", factions.team2);
	end
	
	-- The game doesn't last long
	add_interval(1000 * 20, function()
		terminate()
	end)
end)
