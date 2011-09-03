----
----  Lua script for the gametype "Zombies".
----

num_zombies = 0;
num_wanted = 0;
num_dead = 0;
timer = 0;
round = 0;


--
-- Spwans zombies
--
spawn_func = function()
	t = random(1, 3);
	
	if (t == 1) then
		add_npc("zomb", factions.team2);
	elseif (t == 2) then
		add_npc("zomb_fast", factions.team2);
	elseif (t == 3) then
		add_npc("zomb_baby", factions.team2);
	end;
	
	num_zombies = num_zombies + 1;
	if num_zombies >= num_wanted then remove_timer(timer) end;
end;


--
-- Starts a new round
--
start_round = function()
	num_zombies = 0;
	num_wanted = num_wanted + 10;
	num_dead = 0;
	round = round + 1;
	
	show_alert_message("Starting Round " .. round);
	
	timer = add_interval(500, spawn_func);
end;


--
-- Spawn a player
--
bind_playerjoin(function(slot)
	add_player("robot", factions.team1, slot);
end);


--
-- Game start
--
bind_gamestart(function()
	add_timer(5000, start_round);
end);


--
-- Handle unit deaths
--
bind_playerdied(function()
	show_alert_message("Just not good enough I see...");

	add_timer(2000, function()
		add_player("maniac", factions.team1, 1);
	end);
end);


--
-- Handle unit deaths
--
bind_npcdied(function()
	num_dead = num_dead + 1;
	
	-- Is the round over?
	if num_dead == num_wanted then
		show_alert_message("I guess you got them this time...");
		add_timer(2500, start_round);
	end;
end);



