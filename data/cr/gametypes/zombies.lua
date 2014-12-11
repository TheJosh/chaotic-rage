----
----  Lua script for the gametype "Zombies".
----

num_zombies = 0;
num_wanted = 0;
num_dead = 0;
num_lives = 10;
spawn_rate = 3000;
timer = 0;
round = 0;
round_max = 0;
score_table = 0;
score_labels = {};
rain_rate = 1000;


do_score = function()
	score_labels.round.data = round;
	score_labels.alive.data = num_zombies - num_dead;
	score_labels.dead.data = num_dead;

	score_labels.lives.data = num_lives;
	score_labels.lives.r = 0.9;
	score_labels.lives.g = num_lives / 10;
	score_labels.lives.b = num_lives / 10;
end;


--
-- Spawns zombies
--
spawn_func = function()
	t = random_arg("zomb", "zomb_fast", "zomb_health", "zomb_strong")
	
	game.addNpc(t, "zombie", factions.team2)
	
	num_zombies = num_zombies + 1
	if num_zombies >= num_wanted then remove_timer(timer) end
	
	do_score()
end;


--
-- Starts a new round
--
start_round = function()
	num_zombies = 0;
	if num_wanted < 30 then
		num_wanted = num_wanted + 3;
	end;
	if spawn_rate > 1000 then
		spawn_rate = spawn_rate - 100;
	end;
	num_dead = 0;
	round = round + 1;
	
	-- start raining after 5th round
	if round >= 5 then
		if rain_rate < 5000 then
			rain_rate = rain_rate + 1500
		end;
		weather.startRain(rain_rate)
	end;
	
	-- Cool label
	if round > round_max then
		lab = add_label(0, 350, "Winner!");
	else
		lab = add_label(0, 350, "Round " .. round);
	end;
	lab.align = 2;
	lab.r = 0.9; lab.g = 0.1; lab.b = 0.1;

	-- Fade out the message
	anim = add_interval(100, function()
		lab.a = lab.a - 0.03;

		if lab.a <= 0.0 then
			-- We should make this actually remove the label
			-- but it needs to be network aware and faction aware
			-- needs more thought first
			remove_timer(anim);
		end;
	end);

	-- Winner?
	if round > round_max then
		game_over(1);
	end;
	
	-- Do an ammo drop
	if round % 3 == 0 then
		show_alert_message("Would you like some more ammo?");
		ammo_drop()
	end;
	
	-- Lets get spawning
	timer = add_interval(spawn_rate, spawn_func);
	
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
	game.addPlayer(get_selected_unittype(), factions.team1, slot);
end);


--
-- Game start
--
bind_gamestart(function(r_max)
	round_max = r_max;

	add_timer(10000, start_round);
	add_timer(5000, initial_ammo);
	
	add_label(-200, 50, "Round");
	score_labels.round = add_label(-70, 50, "0");

	add_label(-200, 70, "Alive");
	score_labels.alive = add_label(-70, 70, "0");

	add_label(-200, 90, "Dead");
	score_labels.dead = add_label(-70, 90, "0");
	
	add_label(-200, 110, "Lives");
	score_labels.lives = add_label(-70, 110, num_lives);

	weather.disableRandom();
end);


--
-- Handle unit deaths
--
bind_playerdied(function(slot)
	show_alert_message("Just not good enough I see...", slot);

	num_lives = num_lives - 1
	if (num_lives == 0) then
		game_over(0);
	end;

	do_score();
	
	add_timer(2000, function()
		game.addPlayer(get_selected_unittype(), factions.team1, slot);
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
