----
----  Lua script for the gametype "Zombies".
----


num_dead = 0
num_npcs = 25


-- Spwans NPCs
spawn_ais = function()
	for i = 0, num_npcs, 1 do
		game.addNpc("robot", "human", factions.individual);
	end;
end;

-- Bring in some ammo crates
spawn_ammos = function()
	game.addPickupRand("weapon_machinegun");
	game.addPickupRand("weapon_pistol");
	game.addPickupRand("weapon_rocket_launcher");
	game.addPickupRand("weapon_flamethrower");
	game.addPickupRand("weapon_proxi_mine");
end;

--
-- Game start
--
bind_gamestart(function()
	-- Show msg
	lab = add_label(0, 350, "Three")
	lab.align = 2
	lab.r = 0.7
	lab.g = 0.1
	lab.b = 0.1
	spawn_ammos()
	
	-- Animation
	add_timer(1000, function()
		lab.data = "Two"
		lab.r = 0.8
		spawn_ammos()
	end);
	add_timer(2000, function()
		lab.data = "One"
		lab.r = 0.9
		spawn_ammos()
	end);
	add_timer(3000, function()
		lab.data = "Fight!"
		lab.r = 1.0
		spawn_ais()			-- bring in the AIs
	end);
	add_timer(4000, function()
		lab.visible = false
	end);
end);


--
-- Handle unit deaths
--
bind_npcdied(function()
	num_dead = num_dead + 1;
	
	if num_dead == 1 then
		show_alert_message("The first death")
	else
		show_alert_message(num_dead .. " deaths")
	end
	
	if num_dead == num_npcs - 1 then
		show_alert_message("We have a victor!")
		add_timer(2000, function()
			game_over(1);
		end)
		return
	end
	
		
	if num_dead % 5 == 0 then
		show_alert_message("Let's throw them some more ammo")
		ammo_drop()
	end
end);

