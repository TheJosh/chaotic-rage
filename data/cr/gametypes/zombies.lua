----
----  Lua script for the gametype "Zombies".
----

num_zombies = 0;
num_wanted = 0;
num_dead = 0;
timer = 0;
round = 0;
score_table = 0;
score_labels = {};


do_score = function()
	score_labels.round.data = round;
	score_labels.alive.data = num_zombies - num_dead;
	score_labels.dead.data = num_dead;
end;


--
-- Spwans zombies
--
spawn_func = function()
	t = random(1, 5);
	
	if (t == 1) then
		add_npc("zomb", "zombie", factions.team2);
	elseif (t == 2) then
		add_npc("zomb_fast", "zombie", factions.team2);
	elseif (t == 3) then
		add_npc("zomb_baby", "zombie", factions.team2);
	elseif (t == 4) then
		add_npc("zomb_facebook", "zombie", factions.team2);
	elseif (t == 5) then
		add_npc("zomb_robot", "zombie", factions.team2);
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
	
	-- Cool label for new round
	lab = add_label(0, 350, "Round " .. round);
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
	
	-- Do an ammo drop
	if round % 3 == 0 then
		show_alert_message("Would you like some more ammo?");
		ammo_drop()
	end;
	
	-- Lets get spawning
	timer = add_interval(500, spawn_func);
	
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
	
	add_label(800, 50, "Round");
	score_labels.round = add_label(900, 50, "0");

	add_label(800, 70, "Alive");
	score_labels.alive = add_label(900, 70, "0");

	add_label(800, 90, "Dead");
	score_labels.dead = add_label(900, 90, "0");
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



