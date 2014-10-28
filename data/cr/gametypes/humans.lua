----
----  Lua script for the gametype "Humans".
----

num_humans = 0
num_dead = 0
timer = 0
round = 0


--
-- Spwans zombies
--
spawn_humans_func = function()
	game.addNpc("robot", "zombie", factions.team2);
	num_humans = num_humans + 1;
end;


--
-- Game start
--
bind_gamestart(function()
	add_interval(10000, spawn_humans_func)
end);


--
-- Spawn in a zombie
--
spawn_player_func = function(slot)
	game.addPlayer("zomb", factions.team1, slot)
	
	-- Show msg
	lab = add_label(0, 350, "Find All Humans");
	lab.align = 2;
	lab.r = 0.9; lab.g = 0.1; lab.b = 0.1
	
	-- Animation
	add_timer(500, function()
		lab.data = "Kill All Humans"
	end);
	add_timer(1000, function()
		lab.a = 0
	end);
end;


--
-- Spawn a player
--
bind_playerjoin(function(slot)
	spawn_player_func(slot);
end);


--
-- Handle unit deaths
--
bind_playerdied(function(slot)
	add_timer(2000, function()
		spawn_player_func(slot);
	end);
end);


--
-- Handle unit deaths
--
bind_npcdied(function()
	num_dead = num_dead + 1
end);
