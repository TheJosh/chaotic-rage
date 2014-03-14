----
----  You're a Zombie, happily going about eating humans
----  Suddenly, the other zombies start attacking each other!
----  It's a Zombie Betrayal!
----



--
-- Game start
--
bind_gamestart(function()
	-- Show msg
	lab = add_label(0, 350, "What's going on?...")
	lab.align = 2
	lab.r = 0.7
	lab.g = 0.1
	lab.b = 0.1
	
	-- Animation
	add_timer(1000, function()
		lab.data = "...I dunno..."
		lab.r = 0.8
	end);
	add_timer(2000, function()
		lab.data = "...Looks like a..."
		lab.r = 0.9
	end);
	add_timer(3000, function()
		lab.data = "Zombie Betrayal!"
		lab.r = 1.0
		
		-- bring in the AIs
		for i = 0, 25, 1 do
			add_npc("zomb", "human", factions.individual);
		end;
		
	end);
	add_timer(4000, function()
		lab.visible = false
	end);
end);


--
-- Spawn a player
--
bind_playerjoin(function(slot)
	add_player("zomb", factions.individual, slot);
end);


--
-- Handle unit deaths
--
bind_playerdied(function(slot)
	add_timer(2000, function()
		game_over();
	end);
end);


