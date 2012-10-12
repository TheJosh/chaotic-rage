----
----  Lua script for the gametype "Zombies".
----

num_zombies = 0;
num_wanted = 0;
num_dead = 0;
timer = 0;
round = 0;
score_table = 0;


do_score = function()
	set_data_value(score_table, 1, 0, round);
	set_data_value(score_table, 1, 1, num_zombies - num_dead);
	set_data_value(score_table, 1, 2, num_dead);
end;


--
-- Spwans zombies
--
spawn_func = function()
	t = random(1, 4);
	
	if (t == 1) then
		add_npc("zomb", "zombie", factions.team2);
	elseif (t == 2) then
		add_npc("zomb_fast", "zombie", factions.team2);
	elseif (t == 3) then
		add_npc("zomb_baby", "zombie", factions.team2);
	elseif (t == 4) then
		add_npc("zomb_facebook", "zombie", factions.team2);
	end;
	
	num_zombies = num_zombies + 1;
	if num_zombies >= num_wanted then remove_timer(timer) end;
	
	do_score();
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
	
	if round % 3 == 0 then
		show_alert_message("Would you like some more ammo?");
		ammo_drop()
	end;
	
	timer = add_interval(500, spawn_func);
	
	set_data_value(score_table, 0, 0, "Round");
	set_data_value(score_table, 0, 1, "Alive");
	set_data_value(score_table, 0, 2, "Dead");
	
	do_score();
end;


--
-- Some initial ammo to get them started
--
initial_ammo = function()
	show_alert_message("Here is some ammo...use it wisely!");
	ammo_drop();
end;


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
	add_timer(10000, start_round);
	add_timer(5000, initial_ammo);
	
	score_table = add_data_table(800, 50, 2, 3);
end);


--
-- Handle unit deaths
--
bind_playerdied(function(slot)
	show_alert_message("Just not good enough I see...");
	
	do_score();
	
	add_timer(2000, function()
		add_player(get_selected_unittype(), factions.team1, slot);
	end);
end);


--
-- Handle unit deaths
--
bind_npcdied(function()
	num_dead = num_dead + 1;
	
	do_score();
	
	-- Is the round over?
	if num_dead == num_wanted then
		show_alert_message("I guess you got them...this time");
		add_timer(2500, start_round);
	end;
end);



