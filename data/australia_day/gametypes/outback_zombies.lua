----
----  Lua script for the gametype "Outback Zombies".
----

num_zombies = 0;
num_dead = 0;
num_achive = 10;
spawn_delay = 3000;
score_table = 0;
score_labels = {};


do_score = function()
	score_labels.alive.data = num_zombies - num_dead;
	score_labels.dead.data = num_dead;
end;


do_achivements = function()
	-- Cool label for new round
	lab = add_label(0, 350, "You've killed " .. num_dead .. " zombie roos!");
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
			
			-- Do an ammo drop
			show_alert_message("Do you need some more ammo?");
			ammo_drop()
		end;
	end);
end;

--
-- Spwans zombies
--
spawn_func = function()
	game.addNpc("zomb_roo", "zombie", factions.team2);
	num_zombies = num_zombies + 1;
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
bind_gamestart(function()
	add_label(800, 70, "Alive");
	score_labels.alive = add_label(900, 70, "0");

	add_label(800, 90, "Dead");
	score_labels.dead = add_label(900, 90, "0");
	
	timer = add_interval(spawn_delay, spawn_func);
end);


--
-- Handle unit deaths
--
bind_playerdied(function(slot)
	show_alert_message("Just not good enough I see...", slot);
	
	do_score();
	
	add_timer(2000, function()
		game.addPlayer(get_selected_unittype(), factions.team1, slot);
	end);
end);


--
-- Handle npc deaths
--
bind_npcdied(function()
	num_dead = num_dead + 1;
	
	-- Speed the game up a little
	if spawn_delay > 500 then
		spawn_delay = spawn_delay - 50;
		remove_timer(timer);
		timer = add_interval(spawn_delay, spawn_func);
	end;
	
	do_score();
	
	if num_dead == num_achive then
		do_achivements();
		num_achive = num_achive + 10;
	end;
end);
