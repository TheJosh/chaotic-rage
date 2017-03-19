----
----  Physics test - zombies
----
debug_framerate(true)


bind_gamestart(function()
	weather.disableRandom();
	daynight.disableCycle();
	
	add_interval(1000 * 20, function()
		terminate()
	end)
end)
