----
----  Lua script for the gametype "Zombies".
----

num_zombies = 0;
num_zombie_players = 0;
num_wanted = 0;
num_dead = 0;
num_lives = 3;
spawn_cluster_num = 3;
spawn_cluster_gap = 3000;
round = 0;
round_max = 0;
score_table = 0;
score_labels = {};
rain_rate = 1000;
player_zombies = {};


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
	zone = game.getRandomSpawnZone(factions.team2)
	if zone == nil then
		return
	end

	for i=1, spawn_cluster_num, 1
	do
		-- may need to run this in a timer instead of three at once
		t = random_arg("zomb", "zomb_fast", "zomb_health", "zomb_strong")
		game.addNpcZone(t, "zombie", factions.team2, zone)
	end

	num_zombies = num_zombies + spawn_cluster_num

	if num_zombies < num_wanted then
		add_timer(spawn_cluster_gap, spawn_func);
	end

	do_score()
end;


--
-- Starts a new round
--
start_round = function()
	num_zombies = 0;

	-- Spawn the player zombies, if there is a coordinate in the list
	position = table.remove(player_zombies, 1)
	while position ~= nil do
		game.addNpcCoord(get_selected_unittype(), "zombie", factions.team2, position)
		num_zombie_players = num_zombie_players + 1;
		num_zombies = num_zombies + 1;
		position = table.remove(player_zombies, 1)
	end

	round = round + 1;
	num_dead = 0;

	-- increase size of clusters
	if round >= 7 then
		spawn_cluster_num = spawn_cluster_num + 1
	end;

	-- increase number of zombies
	-- increase is always in the size of a cluster
	if num_wanted < 40 then
		num_wanted = num_wanted + spawn_cluster_num + num_zombie_players;
	end;

	-- start off with a few extras
	if round == 1 then
		num_wanted = num_wanted + spawn_cluster_num
	end;

	-- decrease gap (ms) between clusters
	if spawn_cluster_gap > 1000 then
		spawn_cluster_gap = spawn_cluster_gap - 100
	end;

	-- Start getting darker round three
	if round == 3 then
		daynight.animate(0.25, 10.0);
	end;

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
	lab.font = "Digitalt"
	lab.size = 70.0

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
		game.addPickupRand("ammo_current");
	end;

	-- Lets get spawning
	add_timer(spawn_cluster_gap, spawn_func);

	do_score();
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

	add_timer(2500, start_round);

	add_label(-200, 50, "Round");
	score_labels.round = add_label(-70, 50, "0");

	add_label(-200, 70, "Alive");
	score_labels.alive = add_label(-70, 70, "0");

	add_label(-200, 90, "Dead");
	score_labels.dead = add_label(-70, 90, "0");

	add_label(-200, 110, "Lives");
	score_labels.lives = add_label(-70, 110, num_lives);

	weather.disableRandom();
	daynight.disableCycle();
end);


--
-- Handle unit deaths
--
bind_playerdied(function(slot)
	num_lives = num_lives - 1
	do_score();

	if (num_lives == 0) then
		lab = add_label(0, 350, "FAILURE");
		lab.align = 2;
		lab.r = 0.9; lab.g = 0.1; lab.b = 0.1;
		lab.font = "Digitalt"
		lab.size = 70.0
		add_timer(5000, function()
		  game_over(0);
		end)
		return
	end;

	show_alert_message("Just not good enough I see...", slot);

	add_timer(2000, function()
		game.addPlayer(get_selected_unittype(), factions.team1, slot);
	end);

	player = game.getPlayerFromSlot(slot)
	if player ~= nil then
		table.insert(player_zombies, player.position);
	end
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
		num_wanted = num_wanted - num_zombie_players;
		num_zombie_players = 0;
		add_timer(2500, start_round);
	end;
end);
