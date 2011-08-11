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
	add_npc("zomb", factions.team2);
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
spawn_player = function()
	add_player(get_selected_unittype(), factions.team1, 1);
end;


--
-- Game start
--
bind_gamestart(function()
	add_timer(2500, spawn_player);
	add_timer(5000, start_round);
end);


--
-- Handle unit deaths
--
bind_playerdied(function()
	show_alert_message("Just not good enough I see...");
	add_timer(2500, spawn_player);
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



